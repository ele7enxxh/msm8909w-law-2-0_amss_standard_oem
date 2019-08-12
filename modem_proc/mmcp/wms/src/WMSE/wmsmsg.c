/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsmsg.c -- Message Group

  The WMS module which implements the User API for SMS. This source file
  implements the message group functions which process the client
  commands of the message group.
  -------------

    Copyright (c) 1998-2014 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsmsg.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/
#include "mmcp_variation.h"
#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmsmsgi.h"
#include "wmstsi.h"
#include "wmspriv.h"
#include "wmscl.h"
#include "wmscfg.h"
#include "wmsdc.h"
#include "wmsdbg.h"
#include "msg.h"
#include "err.h"
#include "string.h"
#include "wmsnv.h"
#include "wmscm.h"
#include "wmsts.h"
#include "bit.h"
#include "event.h"
#include "mmgsdisessionlib.h"

#include "wmsutils.h"
#include "cm.h"
#include "stringl.h"

#ifdef FEATURE_CDSMS
#include "wmstscdma.h"
#include "mccdma.h"
#endif /* FEATURE_CDSMS */

#include "wmsbc.h"
#include "wmssim.h"

#ifdef FEATURE_GWSMS
#include "wmscm_gw.h"
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GSTK
#include "gstk_exp.h"
#include "gstk_exp_v.h"
#endif /* FEATURE_GSTK */

#ifdef FEATURE_MMGSDI
#include "pbmlib.h"
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_GWSMS_BROADCAST
#include "log_codes_gsm.h"
#endif /* FEATURE_GWSMS_BROADCAST */

#include "cmutil.h"

#ifdef FEATURE_SMS_PWS_OVER_LTE
#include "lte_rrc_ext_msg.h"
#include "msgr_rex.h"
#include "dsm_item.h"
#endif /* FEATURE_SMS_PWS_OVER_LTE */

#include "simlock_modem_lib.h"
#include "wms_modem.h"
#include "mm.h"

/*===========================================================================
========================== MACROS   =========================================
===========================================================================*/
#define WMS_RELATIVE_VALIDITY_IMMEDIATE          246
#define WMS_MAX_RETRY_COUNT                      1
/*===========================================================================
======================== LOCAL DATA =========================================
===========================================================================*/
#define WMS_MSG_TRANSPORT_ARR_MAX 1
static wms_msg_transport_s_type msg_transports[WMS_MSG_TRANSPORT_ARR_MAX];
#ifdef FEATURE_GWSMS
#include "mn_sms.h"
#endif /* FEATURE_GWSMS */
static wms_msg_s_type  *msg_s_ptr;
static wms_cfg_s_type  *cfg_s_ptr;
#ifdef FEATURE_CDSMS
static wms_dc_s_type   *dc_s_ptr;
#endif /* FEATURE_CDSMS */

static wms_msg_event_info_s_type         msg_event_info;

#ifdef FEATURE_SMS_PWS_OVER_LTE
/* WMS cache to hold most recent Cell Information */
static wms_cell_info_s_type current_cell_info;
#endif


#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS_DEPERSO)
static wms_ack_info_s_type               wms_msg_Gack_info;
#endif /* defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS_DEPERSO) */

#ifdef FEATURE_CDSMS
static uint8                             ruim_data[NV_SMS_DATA_SIZ];
static wms_msg_ruim_sms_s_type           ruim_sms;
static wms_tl_message_type               cdma_tl;
static wms_cdma_message_s_type           cdma_cl;
static wms_raw_ts_data_s_type            cdma_raw_bd;
#endif /* FEATURE_CDSMS */

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)
static wms_client_ts_data_s_type         wms_msg_Gclient_ts;
#endif /* defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS) */

#ifdef FEATURE_GWSMS
static wms_raw_ts_data_s_type            raw_ts;
static uint8                             sim_data[WMS_MAX_LEN];
static wms_msg_sim_sms_unpacked_s_type   sim_sms_unpacked;
static wms_msg_sim_sms_s_type            sim_sms;
static wms_msg_sim_smsr_s_type           sim_smsr;
#endif /* FEATURE_GWSMS */

/* Timer for clearing the mo record periodically every 30 sec */
rex_timer_type  wms_retry_timer;

/* Define a special outbound buffer types to isolate WMS client data
 * from internal WMS data.  This is done to protect from two specific
 * scenarios:
 *    1) Data tampering by clients causing other clients to receive bad data
 *    2) Clients calling free() on WMS buffers causing WMS to crash later
 *
 * These buffers will be used to hold data passed to the client during the callbacks.
 * Solution for 1) For each client callback called, the data will be re-copied
 *                 into the buffer to ensure each client receives un-tainted data.
 * Solution for 2) The buffer structure is set up so that there is never a valid
 *                 pointer for a client to free().  This will cause any client
 *                 attempting to free to cause an exception in the client's context.
 */
typedef struct wms_msg_outbound_event_s
{
  uint32                     unused_offset_do_not_remove;
  wms_msg_event_info_s_type  ev_info;
  char                       ev_ext[WMS_ALPHA_ID_MAX];
} wms_msg_outbound_event_s_type;

typedef struct wms_msg_outbound_parse_s
{
  uint32                     unused_offset_do_not_remove;
  wms_client_message_s_type  ev_info;
} wms_msg_outbound_parse_s_type;

#ifdef FEATURE_GWSMS
/* -----------------------------------------------------------------------
** Local Function Definitions
** ----------------------------------------------------------------------- */
static wms_cmd_err_e_type  wms_msg_fill_sc_address (
   wms_prov_app_info_s_type  *appInfo_ptr,
   wms_address_s_type*       sc_address_ptr );

static __inline
void wms_msg_release_mt_record(uint32 index)
{
   WMS_MEM_FREEIF(msg_s_ptr->gw_mt_rec[index]);
}

boolean wms_msg_map_mt_record
(
  sys_modem_as_id_e_type        as_id,
  wms_transaction_id_type       tid,
  uint32                        * index_ptr
);

#endif /* FEATURE_GWSMS */


#ifdef FEATURE_CDSMS
static boolean wms_msg_convert_plus_address
(
  wms_address_s_type      *wms_addr
);

static boolean wms_msg_convert_address_to_plus
(
  wms_address_s_type      *wms_addr
);


static wms_msg_cdma_message_record_type *wms_msg_cdma_get_mo_rec(void);
static wms_msg_cdma_message_record_type *wms_msg_cdma_get_free_mt_rec(void);
static wms_msg_cdma_message_record_type *wms_msg_cdma_map_mo_rec(uint8  seq_num);
static wms_msg_cdma_message_record_type *wms_msg_cdma_map_mt_rec(uint8 seq_num);
static wms_msg_cdma_message_record_type *wms_msg_cdma_get_next_mo_rec_before_delete(wms_msg_cdma_message_record_type *rec_ptr);
static void wms_msg_cdma_free_mt_msg_rec(wms_msg_cdma_message_record_type *rec_ptr);
static wms_status_e_type wms_msg_get_next_unused_seq_num
(
  uint8  start_seq_num,
  uint8* unused_seq_num_ptr
);
#endif /* FEATURE_CDSMS */

/*===========================================================================
======================== FUNCTIONS ==========================================
===========================================================================*/


/*===========================================================================
FUNCTION wms_msg_suspend_ps_call_is_supported

DESCRIPTION
  This is a helper function used to determine whether the suspend/resume
  PS call functions are relevant to a specific device mode.

  Some devices have only one radio and must share it between subscriptions.
  For these devices to send SMS calls, active data calls on the other
  subscription must be suspended.

DEPENDENCIES
  The device mode must have been read from EFS and stored in the WMS
  config structure.

RETURN VALUE
  Returns TRUE for DSDS devices (single radio) and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean wms_msg_suspend_ps_call_is_supported(
  void
)
{
  cfg_s_ptr = wms_cfg_s_ptr();

  if (WMS_DEVICE_MODE_DSDS == cfg_s_ptr->device_mode)
  {
    return TRUE;
  }

  return FALSE;
} /* wms_msg_suspend_ps_call_is_supported() */

void wms_check_valid_message_index(wms_message_index_type index)
{
  MSG_ERROR_1("Invalid message index %d", index);
}

#ifdef FEATURE_GWSMS_BROADCAST
/*===========================================================================

FUNCTION WMSDIAG_CB_MSG_SEND_LOG

DESCRIPTION
  This function accepts a cb_data_ind_type structure and creates a
  wmsdiag_cb_receive_log_type log that contains all the log information in
  the format specified in the Diag ICD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void wms_msg_gw_cb_send_log
(
  const wms_raw_ts_data_s_type* raw_bd_ptr
)
{
  wms_msg_gw_cb_receive_log_type* log_rsp;

  if (raw_bd_ptr == NULL)
  {
    MSG_ERROR_0("raw_bd_ptr null pointer!");
  }
  else
  {
    log_rsp = (wms_msg_gw_cb_receive_log_type*) log_alloc(LOG_WMS_RECEIVE_CB,
                                                          sizeof(wms_msg_gw_cb_receive_log_type) );

    if (log_rsp != NULL)
    {
      (void)memscpy( log_rsp->cb_data,
                     WMS_GW_CB_PAGE_SIZE,
                     raw_bd_ptr->data,
                     WMS_GW_CB_PAGE_SIZE);

      log_commit (log_rsp);

      MSG_HIGH_0("cb log packet send successfully");
    }
    else
    {
      MSG_ERROR_0("Failed to allocate log_rsp packet for cb receive msg");
    }
  }
}
#endif /*FEATURE_GWSMS_BROADCAST*/


wms_cmd_err_e_type wms_msg_map_memory_store_index(
   sys_modem_as_id_e_type as_id,
   uint32 index_count,
   uint32 index,
   uint32* mapped_index_ptr

)
{
   uint32 lower_bound = 0;
   uint32 upper_bound = index_count;

   if ( !asid_is_valid(as_id))
   {
     return WMS_CMD_ERR_AS_ID;
   }

   if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
   {
      lower_bound = as_id * index_count / MAX_AS_IDS;
      upper_bound = lower_bound + index_count / MAX_AS_IDS;
      index += lower_bound;
   }

   if (index < lower_bound || index >= upper_bound)
   {
      return WMS_CMD_ERR_MSG_INDEX;
   }

   if (NULL != mapped_index_ptr)
   {
     *mapped_index_ptr = index;
   }

   return WMS_CMD_ERR_NONE;
}


wms_cmd_err_e_type wms_msg_map_gw_ram_index(
   sys_modem_as_id_e_type as_id,
   uint32 index,
   uint32* mapped_index_ptr

)
{
   return wms_msg_map_memory_store_index(
      as_id,
      WMS_MAX_RAM_MESSAGES,
      index,
      mapped_index_ptr
      );
}

#ifdef FEATURE_GWSMS
wms_cmd_err_e_type wms_msg_map_gw_nv_index(
   sys_modem_as_id_e_type as_id,
   uint32 index,
   uint32* mapped_index_ptr
)
{
   return wms_msg_map_memory_store_index(
      as_id,
      cfg_s_ptr->nv_gw_max_slots,
      index,
      mapped_index_ptr
      );
}

wms_cmd_err_e_type wms_msg_map_gw_ram_status_rpt_index(
   sys_modem_as_id_e_type as_id,
   uint32 index,
   uint32* mapped_index_ptr
)
{
   return wms_msg_map_memory_store_index(
      as_id,
      WMS_MAX_RAM_STATUS_REPORTS,
      index,
      mapped_index_ptr
      );
}

wms_cmd_err_e_type wms_msg_find_free_gw_ram_index(
   sys_modem_as_id_e_type as_id,
   uint32* index_ptr,
   uint32* mapped_index_ptr
)
{
   uint32 i;
   uint32 lower_bound = 0;
   uint32 upper_bound = WMS_MAX_RAM_MESSAGES;
   wms_cmd_err_e_type err = WMS_CMD_ERR_MSG_RAM_FULL;

   if ( !asid_is_valid(as_id))
   {
     return WMS_CMD_ERR_AS_ID;
   }

   if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
   {
      lower_bound = as_id * WMS_MAX_RAM_MESSAGES / MAX_AS_IDS;
      upper_bound = lower_bound + WMS_MAX_RAM_MESSAGES / MAX_AS_IDS;
   }

   for (i = lower_bound; i < upper_bound; i++)
   {
      if (NULL == cfg_s_ptr->ram_gw_sms[i])
      {
         /* found a free slot */
         cfg_s_ptr->ram_gw_sms[i] = (wms_msg_sim_sms_s_type*) wms_mem_malloc(sizeof(wms_msg_sim_sms_s_type));

         if(NULL == cfg_s_ptr->ram_gw_sms[i])
         {
            return WMS_CMD_ERR_NULL_PTR;
         }

         if (NULL != index_ptr)
         {
            *index_ptr = i - lower_bound;
         }
         if (NULL != mapped_index_ptr)
         {
            *mapped_index_ptr = i;
         }
         err = WMS_CMD_ERR_NONE;
         break;
      }
   } /* for */

   return err;
}

wms_cmd_err_e_type wms_msg_find_free_gw_ram_status_rpt_index(
   sys_modem_as_id_e_type as_id,
   uint32* index_ptr,
   uint32* mapped_index_ptr
)
{
   uint32 i;
   uint32 lower_bound = 0;
   uint32 upper_bound = WMS_MAX_RAM_STATUS_REPORTS;
   wms_cmd_err_e_type err = WMS_CMD_ERR_MSG_RAM_FULL;

   if ( !asid_is_valid(as_id))
   {
     return WMS_CMD_ERR_AS_ID;
   }

   if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
   {
      lower_bound = as_id * WMS_MAX_RAM_STATUS_REPORTS / MAX_AS_IDS;
      upper_bound = lower_bound + WMS_MAX_RAM_STATUS_REPORTS / MAX_AS_IDS;
   }

   for (i = lower_bound; i < upper_bound; i++)
   {
      if (cfg_s_ptr->ram_status_rpts[i] == FALSE)
      {
         /* found a free slot */
         if (NULL != index_ptr)
         {
            *index_ptr = i - lower_bound;
         }
         if (NULL != mapped_index_ptr)
         {
            *mapped_index_ptr = i;
         }
         err = WMS_CMD_ERR_NONE;
         break;
      }
   } /* for */

   return err;
}


wms_cmd_err_e_type wms_msg_find_free_gw_nv_index(
   sys_modem_as_id_e_type as_id,
   uint32* index_ptr,
   uint32* mapped_index_ptr
)
{
   uint32 i;
   uint32 lower_bound = 0;
   uint32 upper_bound = cfg_s_ptr->nv_gw_max_slots;
   wms_cmd_err_e_type err = WMS_CMD_ERR_MSG_NV_FULL;

   if ( !asid_is_valid(as_id))
   {
     return WMS_CMD_ERR_AS_ID;
   }

   if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
   {
     lower_bound = as_id * cfg_s_ptr->nv_gw_max_slots / MAX_AS_IDS;
     upper_bound = lower_bound + cfg_s_ptr->nv_gw_max_slots / MAX_AS_IDS;
   }

   for (i = lower_bound; i < upper_bound; i++)
   {
      if (WMS_TAG_NONE == cfg_s_ptr->nv_gw_tags[i])
      {
         /* found a free slot */
         if (NULL != index_ptr)
         {
            *index_ptr = i - lower_bound;
         }
         if (NULL != mapped_index_ptr)
         {
            *mapped_index_ptr = i;
         }
         err = WMS_CMD_ERR_NONE;
         break;
      }
   } /* for */

   return err;
}
#endif /* FEATURE_GWSMS */

wms_cmd_err_e_type wms_msg_find_free_sms_rec_index(
  wms_prov_app_info_s_type *appInfo_ptr,
  uint32                   *index_ptr
)
{
  uint32 i;
  wms_cmd_err_e_type err = WMS_CMD_ERR_MSG_SIM_FULL;

  if (NULL == appInfo_ptr) {
    err = WMS_CMD_ERR_NULL_PTR;
    return err;
  }

  for (i = 0; i < (uint32) appInfo_ptr->sms_rec_count; i++)
  {
    if (WMS_TAG_NONE == appInfo_ptr->sms_rec_tags[i])
    {
      err = WMS_CMD_ERR_NONE;
      /* found a free slot */
      break;
    }
  }

  if (NULL != index_ptr)
  {
    *index_ptr = i;
  }

  return err;
}

static wms_cmd_err_e_type wms_msg_do_client_ack
(
  wms_cmd_type          * cmd_ptr
);

#ifdef FEATURE_GWSMS
boolean wms_msg_map_mo_record
(
  sys_modem_as_id_e_type as_id,
  uint16   smi,
  uint32  *index_ptr
);
#endif /* FEATURE_GWSMS */

//wms_cmd_err_e_type wms_msg_do_write(
//  wms_prov_app_info_s_type     *appInfo_ptr;
//  wms_write_mode_e_type         write_mode, /* IN */
//  wms_client_message_s_type    *msg_ptr   /* IN-OUT */
//);

/*
*/
wms_msg_s_type* wms_msg_s_ptr
(
  void
)
{
  static wms_msg_s_type  msg_s;

  return &msg_s;

} /* wms_msg_s_ptr() */

#ifdef FEATURE_GSTK
/*===========================================================================
FUNCTION wms_msg_send_gstk_evt

DESCRIPTION
  Used to post GSTK Event to WMS Command Queue for Processing.

DEPENDENCIES
  none

RETURN VALUE
  wms_status_e_type

SIDE EFFECTS
  None.
===========================================================================*/
wms_status_e_type wms_msg_send_gstk_evt
(
  gstk_cmd_from_card_type  * gstk_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_MSG_GSTK_EVT;
    cmd_ptr->hdr.client_id   = (wms_client_id_type)WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;

    cmd_ptr->cmd.msg_gstk_evt.gstk_evt = * gstk_ptr;

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_msg_send_gstk_evt() */
#endif /* FEATURE_GSTK */

#if defined(FEATURE_GWSMS_PP_DOWNLOAD) || ((defined(FEATURE_UIM_TOOLKIT_UTK) || \
    defined(FEATURE_CCAT)) || defined(FEATURE_GSTK))
/*
*/
void wms_msg_gstk_evt_cb
(
  gstk_cmd_from_card_type *    gstk_ptr
)
{
  wms_message_mode_e_type      msg_mode;

  if (gstk_ptr == NULL)
  {
    MSG_ERROR_0("gstk_ptr null pointer in wms_msg_gstk_evt_cb!");
  }
  else
  {
    switch( gstk_ptr->hdr_cmd.command_id )
    {
      case GSTK_SMS_PP_DOWNLOAD_IND_RSP:
      {
        wms_gstk_user_data_s_type gstk_user_data;

        if (NULL == (void *)gstk_ptr->hdr_cmd.user_data)
        {
          break;
        }

        gstk_user_data = *((wms_gstk_user_data_s_type *)gstk_ptr->hdr_cmd.user_data);
        wms_mem_free((void *)gstk_ptr->hdr_cmd.user_data);
        msg_mode = gstk_user_data.msg_mode;

        if(msg_mode == WMS_MESSAGE_MODE_GW)
        {
#ifdef FEATURE_GWSMS_PP_DOWNLOAD
          wms_cmd_type *wms_cmd_buf_ptr = (wms_cmd_type *)wms_mem_malloc(sizeof (wms_cmd_type));

          if (NULL == wms_cmd_buf_ptr)
          {
            MSG_ERROR_1("malloc failed in wms_msg_gstk_evt_cb. wms_cmd_buf_ptr=0x%08x", wms_cmd_buf_ptr);
          }
          else
          {
            uim_sw1_type resp_sw1 = (uim_sw1_type)gstk_ptr->cmd.sms_pp_download_rsp.sw1;
            uim_sw2_type resp_sw2 = (uim_sw2_type)gstk_ptr->cmd.sms_pp_download_rsp.sw2;

            MSG_HIGH_2( "wms_msg_gstk_evt_cb(): resp_sw1 = %d, resp_sw2 = %d",
                        resp_sw1,
                        resp_sw2 );


            wms_cmd_buf_ptr->hdr.as_id = gstk_user_data.as_id;
            wms_cmd_buf_ptr->hdr.cmd_id        = WMS_CMD_MSG_ACK;
            wms_cmd_buf_ptr->hdr.client_id     = (wms_client_id_type)WMS_CLIENT_TYPE_INTERNAL;
            wms_cmd_buf_ptr->hdr.appInfo_ptr   = wms_get_appinfo_by_asid_and_message_mode(gstk_user_data.as_id, WMS_MESSAGE_MODE_GW);
            wms_cmd_buf_ptr->hdr.cmd_cb        = NULL;
            wms_cmd_buf_ptr->hdr.user_data     = NULL;

            wms_cmd_buf_ptr->cmd.msg_ack.ack_info.message_mode   = WMS_MESSAGE_MODE_GW;
            wms_cmd_buf_ptr->cmd.msg_ack.ack_info.transaction_id = gstk_user_data.pp_download_tid;
            wms_cmd_buf_ptr->cmd.msg_ack.transport_type = WMS_MSG_TRANSPORT_TYPE_INTERNAL;

            if( ((SW1_NORMAL_END == resp_sw1) && (SW2_NORMAL_END == resp_sw2))||
                (SW1_WARNINGS1 == resp_sw1)  || (SW1_WARNINGS2 == resp_sw1) ||
                (SW1_PROBLEM == resp_sw1) )
            {
              /* Envelop command sucess - TS31.111 */
              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.success = TRUE;
              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.tpdu_type = WMS_TPDU_DELIVER_REPORT_ACK;

              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.ack.user_data_header_present = FALSE;
              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.ack.mask = WMS_TPDU_MASK_PID | WMS_TPDU_MASK_DCS;
              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.ack.pid = gstk_user_data.pp_download_pid;
              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.ack.dcs = gstk_user_data.pp_download_dcs;

              if( gstk_ptr->cmd.sms_pp_download_rsp.response_data_present == TRUE )
              {
                wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.ack.mask |= WMS_TPDU_MASK_USER_DATA;
                wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.ack.user_data.num_headers = 0;
                wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.ack.user_data.sm_len = gstk_ptr->cmd.sms_pp_download_rsp.ack.length;
                (void)memscpy( wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.ack.user_data.sm_data,
                               gstk_ptr->cmd.sms_pp_download_rsp.ack.length,
                               gstk_ptr->cmd.sms_pp_download_rsp.ack.ack,
                               gstk_ptr->cmd.sms_pp_download_rsp.ack.length );
              }

              if( wms_msg_do_client_ack( wms_cmd_buf_ptr ) != WMS_CMD_ERR_NONE )
              {
                MSG_ERROR_0("wms_msg_do_client_ack Failed!");
              }
            }
            else if( (SW1_BUSY == resp_sw1) && (SW2_NORMAL_END == resp_sw2) )
            {
              /* SIM Application Toolkit Busy - TS31.111 */
              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.success = FALSE;
              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.rp_cause = WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED;
              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.tpdu_type = WMS_TPDU_DELIVER_REPORT_ERROR;

              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.error.user_data_header_present = FALSE;
              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.error.mask = WMS_TPDU_MASK_PID | WMS_TPDU_MASK_DCS;
              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.error.pid = gstk_user_data.pp_download_pid;
              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.error.dcs = gstk_user_data.pp_download_dcs;
              wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.error.tp_cause = WMS_TP_CAUSE_SAT_BUSY;

              if( gstk_ptr->cmd.sms_pp_download_rsp.response_data_present == TRUE )
              {
                wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.error.mask |= WMS_TPDU_MASK_USER_DATA;
                wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.error.user_data.num_headers = 0;
                wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.error.user_data.sm_len = gstk_ptr->cmd.sms_pp_download_rsp.ack.length;
                (void)memscpy( wms_cmd_buf_ptr->cmd.msg_ack.ack_info.u.gw.u.error.user_data.sm_data,
                               gstk_ptr->cmd.sms_pp_download_rsp.ack.length,
                               gstk_ptr->cmd.sms_pp_download_rsp.ack.ack,
                               gstk_ptr->cmd.sms_pp_download_rsp.ack.length );
              }

              if( wms_msg_do_client_ack( wms_cmd_buf_ptr ) != WMS_CMD_ERR_NONE )
              {
                MSG_ERROR_0("wms_msg_do_client_ack Failed!");
              }
            }
            else
            {
              /* Authentication have failed in the SIM card and no further processing related to the Proof of Receipt
                 should take place - TS31.111.
              */
              uint32 index;
              MSG_ERROR_0("No ack needed for failed envelope command.");

              if( wms_msg_map_mt_record( wms_cmd_buf_ptr->hdr.as_id, wms_cmd_buf_ptr->cmd.msg_ack.ack_info.transaction_id, &index) )
              {
                /* Clear the record. */
                wms_msg_release_mt_record(index);
              }
            }
          }
          WMS_MEM_FREEIF(wms_cmd_buf_ptr);
#endif /* FEATURE_GWSMS_PP_DOWNLOAD */
        }
#ifdef FEATURE_CDSMS
        else if(msg_mode == WMS_MESSAGE_MODE_CDMA)
        {
#if (defined(FEATURE_UIM_TOOLKIT_UTK) || defined(FEATURE_CCAT)) || defined(FEATURE_GSTK)
          wms_client_ts_data_s_type         *wms_client_ts_ptr = NULL;
          wms_client_message_s_type         *pp_download_user_ack_cl_msg_ptr = NULL;

          wms_client_ts_ptr = (wms_client_ts_data_s_type *)wms_mem_malloc(sizeof(wms_client_ts_data_s_type));
          pp_download_user_ack_cl_msg_ptr = (wms_client_message_s_type *)wms_mem_malloc(sizeof(wms_client_message_s_type));

          if ( (NULL == wms_client_ts_ptr) || (NULL == pp_download_user_ack_cl_msg_ptr))
          {
            MSG_ERROR_2( "malloc failed: wms_client_ts_ptr=0x%08x, pp_download_user_ack_cl_msg_ptr=0x%08x",
                         wms_client_ts_ptr,
                         pp_download_user_ack_cl_msg_ptr );
          }
          else
          {
            wms_client_ts_ptr->format      = WMS_FORMAT_CDMA;
            wms_client_ts_ptr->u.cdma.mask = WMS_MASK_BD_MSG_ID;

            /* Set the message_id info */
            wms_client_ts_ptr->u.cdma.message_id.type        = WMS_BD_TYPE_USER_ACK;
            wms_client_ts_ptr->u.cdma.message_id.id_number   = gstk_user_data.pp_download_msg_id;
            wms_client_ts_ptr->u.cdma.message_id.udh_present = FALSE;

            if (GSTK_ENVELOPE_CMD_SUCCESS == gstk_ptr->cmd.sms_pp_download_rsp.general_resp)
            {
              MSG_HIGH_0("PP Download via Envelope was successful");
              /* No response codes required */
            }
            else
            {
              wms_client_ts_ptr->u.cdma.mask |= WMS_MASK_BD_USER_RESP;
              if( gstk_ptr->cmd.sms_pp_download_rsp.general_resp == GSTK_ENVELOPE_CMD_FAIL )
              {
                wms_client_ts_ptr->u.cdma.user_response = (wms_user_response_type)WMS_RSP_DATA_DOWNLOAD_ERROR;
              }
              else /* BUSY */
              {
                wms_client_ts_ptr->u.cdma.user_response = (wms_user_response_type)WMS_RSP_APPLICATION_TOOLKIT_BUSY;
              }
            }

            if(gstk_ptr->cmd.sms_pp_download_rsp.response_data_present)
            {
              wms_client_ts_ptr->u.cdma.mask |= WMS_MASK_BD_USER_DATA;
              wms_client_ts_ptr->u.cdma.user_data.data_len = MIN( gstk_ptr->cmd.sms_pp_download_rsp.ack.length,
                                                             WMS_CDMA_USER_DATA_MAX);
              wms_client_ts_ptr->u.cdma.user_data.encoding = WMS_ENCODING_OCTET;
              (void)memscpy( wms_client_ts_ptr->u.cdma.user_data.data,
                             wms_client_ts_ptr->u.cdma.user_data.data_len,
                             gstk_ptr->cmd.sms_pp_download_rsp.ack.ack,
                             wms_client_ts_ptr->u.cdma.user_data.data_len);
              wms_client_ts_ptr->u.cdma.user_data.number_of_digits = wms_client_ts_ptr->u.cdma.user_data.data_len;
            }

            pp_download_user_ack_cl_msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
            pp_download_user_ack_cl_msg_ptr->msg_hdr.index     = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
            pp_download_user_ack_cl_msg_ptr->msg_hdr.tag       = WMS_TAG_MO_NOT_SENT;
            pp_download_user_ack_cl_msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;

            /* Fill in cached info from envelope origination */
            if(wms_ts_encode(wms_client_ts_ptr, &pp_download_user_ack_cl_msg_ptr->u.cdma_message.raw_ts) == WMS_OK_S)
            {
              pp_download_user_ack_cl_msg_ptr->u.cdma_message.teleservice = gstk_user_data.pp_download_teleservice;
              pp_download_user_ack_cl_msg_ptr->u.cdma_message.address = gstk_user_data.pp_download_address;
              pp_download_user_ack_cl_msg_ptr->u.cdma_message.subaddress = gstk_user_data.pp_download_subaddress;
              pp_download_user_ack_cl_msg_ptr->u.cdma_message.is_tl_ack_requested = FALSE;
              pp_download_user_ack_cl_msg_ptr->u.cdma_message.is_service_present  = FALSE;
              pp_download_user_ack_cl_msg_ptr->u.cdma_message.is_mo               = TRUE;
              pp_download_user_ack_cl_msg_ptr->u.cdma_message.service = WMS_SRV_UNKOWN;

              (void)wms_msg_send( (wms_client_id_type)WMS_CLIENT_TYPE_INTERNAL,
                                  NULL,
                                  NULL,
                                  WMS_SEND_MODE_CLIENT_MESSAGE,
                                  pp_download_user_ack_cl_msg_ptr );
              MSG_HIGH_0("UTK ENVELOPE USER ACK SENT OK");
            }
            else
            {
              MSG_ERROR_0("CONVERSION TO RAW DATA FAILED");
              MSG_ERROR_0("PROBLEM SENDING UTK USER ACK");
            }
          }
          WMS_MEM_FREEIF(wms_client_ts_ptr);
          WMS_MEM_FREEIF(pp_download_user_ack_cl_msg_ptr);

#endif /* (FEATURE_UIM_TOOLKIT_UTK||FEATURE_CCAT) && FEATURE_GSTK */
        }
#endif /* FEATURE_CDSMS */
        break;
      }

#ifdef FEATURE_GWSMS_MO_CONTROL
      case GSTK_MO_SMS_CTRL_IND_RSP:
        if( wms_msg_send_gstk_evt( gstk_ptr ) != WMS_OK_S )
        {
          MSG_ERROR_0( "WMS out of buffer");
        }
        break;
#endif /* FEATURE_GWSMS_MO_CONTROL */

      case GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP:
        MSG_HIGH_0("Recieved CB Data Download Indication");
        break;

      default:
        /* other commands/responses are ignored */
        break;

    } /* switch */
  }

  /* done */
  return;
} /* wms_msg_gstk_evt_cb() */

#endif /* FEATURE_GWSMS_PP_DOWNLOAD || ((F_UIM_TOOLKIT_UTK||F_CCAT) && F_GSTK) */


/*===========================================================================
FUNCTION wms_msg_filter_events

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
boolean wms_msg_filter_events
(
  wms_client_id_type       client_id,
  wms_client_s_type        * client_ptr,
  wms_msg_event_e_type     event
)
{
  boolean   ok_to_send_event = FALSE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_3( "MSG: client_id=%d, event=%d, notify type = %d",
              client_id,
              event,
              client_ptr->msg_notify_type );

  if( WMS_PM_NOTIFY_ALL == client_ptr->msg_notify_type )
  {
    ok_to_send_event = TRUE;
  }
  else if( (WMS_PM_NOTIFY_CUSTOM_FILTER == client_ptr->msg_notify_type) &&
           ( client_ptr->msg_event_mask & ((uint32)1 << (uint32)event) ) )
  {
    ok_to_send_event = TRUE;
  }
  else if( WMS_PM_NOTIFY_DEFAULT_FILTER == client_ptr->msg_notify_type )
  {
    switch( event )
    {
      case WMS_MSG_EVENT_RECEIVED_MESSAGE:
      case WMS_MSG_EVENT_SUBMIT_REPORT:
      case WMS_MSG_EVENT_STATUS_REPORT:
      case WMS_MSG_EVENT_MT_MESSAGE_ERROR:
      case WMS_MSG_EVENT_ACK_REPORT:
      case WMS_MSG_EVENT_DUPLICATE_CB_PAGE:
        MSG_HIGH_0("PM: This event matches internal default filter");
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
    MSG_ERROR_2( "Blocking msg event %d for client %d",
                 event,
                 client_id );
  }

  return ok_to_send_event;

} /* wms_msg_filter_events() */

/*===========================================================================
FUNCTION wms_msg_prep_and_copy_ob_msg_event

DESCRIPTION
  Make a heap-based copy of the outbound event data provided to the function
  wms_msg_event_notify() into a buffer to be sent to clients. The copy is
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
static boolean wms_msg_prep_and_copy_ob_msg_event(
  wms_msg_event_e_type            event,
  wms_msg_event_info_s_type      *msg_event_info_ptr,
  wms_msg_outbound_event_s_type **ob_evt
)
{
  if ( (NULL == ob_evt) || (NULL == msg_event_info_ptr) )
  {
     return FALSE;
  }

  // Allocate a new buffer or zero the old
  if (NULL == *ob_evt)
  {
    *ob_evt = wms_mem_malloc(sizeof(wms_msg_outbound_event_s_type));
  }

  if (NULL == *ob_evt)
  {
    MSG_HIGH_0("Malloc failed");
    return FALSE;
  }
  else {
    memset(*ob_evt, 0, sizeof(wms_msg_outbound_event_s_type) );
  }

  // Copy event
  (void)memscpy( &((*ob_evt)->ev_info),
                 sizeof(wms_msg_event_info_s_type),
                 msg_event_info_ptr,
                 sizeof(wms_msg_event_info_s_type));

  // Copy any extras (alpha)
  {
    if (event == WMS_MSG_EVENT_SUBMIT_REPORT)
    {
      if ( (msg_event_info_ptr->submit_report_info.alpha_id.len > 0) &&
           (NULL != msg_event_info_ptr->submit_report_info.alpha_id.data) )
      {
        // Copy alpha to ev_ext
        (void)memscpy( (*ob_evt)->ev_ext,
                       msg_event_info_ptr->submit_report_info.alpha_id.len,
                       msg_event_info_ptr->submit_report_info.alpha_id.data,
                       msg_event_info_ptr->submit_report_info.alpha_id.len);
        // Fix the pointer in the copy
        (*ob_evt)->ev_info.submit_report_info.alpha_id.data = (uint8 *)(*ob_evt)->ev_ext;
      }
    }
    else if (event == WMS_MSG_EVENT_SEND)
    {
      if ( (msg_event_info_ptr->status_info.alpha_id.len > 0) &&
           (NULL != msg_event_info_ptr->status_info.alpha_id.data) )
      {
        // Copy alpha to ev_ext
        (void)memscpy( (*ob_evt)->ev_ext,
                       msg_event_info_ptr->status_info.alpha_id.len,
                       msg_event_info_ptr->status_info.alpha_id.data,
                       msg_event_info_ptr->status_info.alpha_id.len);
        // Fix the pointer in the copy
        (*ob_evt)->ev_info.status_info.alpha_id.data = (uint8 *)(*ob_evt)->ev_ext;
      }
    }
    else if ( ( (event == WMS_MSG_EVENT_WRITE_TEMPLATE)  || 
                (event == WMS_MSG_EVENT_READ_TEMPLATE)   ||
                (event == WMS_MSG_EVENT_SMSC_ADDR_CHANGE) ) &&
                (WMS_MESSAGE_MODE_GW == msg_event_info_ptr->status_info.message.msg_hdr.message_mode) )
    {
      if ( (msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.len > 0) &&
           (NULL != msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.data) )
      {
        // Copy alpha to ev_ext
        (void)memscpy( (*ob_evt)->ev_ext,
                       msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.len,
                       msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.data,
                       msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.len);
        // Fix the pointer in the copy
        (*ob_evt)->ev_info.status_info.message.u.gw_template.alpha_id.data = (uint8 *)(*ob_evt)->ev_ext;
      }
    }
    else if (event == WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION)
    {
      int count = 0;
      int buffer_size = 0;

      /* Copying MWI summary information. */
      count = msg_event_info_ptr->transport_mwi_info.summary_tuple_count;

      if (count > 0)
      {
        buffer_size = count * sizeof(wms_msg_mwi_summary_s_type);
        (*ob_evt)->ev_info.transport_mwi_info.mwi_msg_summary = (wms_msg_mwi_summary_s_type *)wms_mem_malloc(buffer_size);
        MSG_HIGH_3("count = %d, buffer_size = %d, summary type size = %d", 
                   count, 
                   buffer_size, 
                   sizeof(wms_msg_mwi_summary_s_type));

        if (NULL != (*ob_evt)->ev_info.transport_mwi_info.mwi_msg_summary)
        {
          (void)memscpy ((*ob_evt)->ev_info.transport_mwi_info.mwi_msg_summary,
                         buffer_size,
                         msg_event_info_ptr->transport_mwi_info.mwi_msg_summary,
                         buffer_size);
        }
        else
        {
          MSG_ERROR_0("Memory heap is exhausted");
          return FALSE;
        }
      }
      else
      {
        (*ob_evt)->ev_info.transport_mwi_info.mwi_msg_summary = NULL;
        (*ob_evt)->ev_info.transport_mwi_info.summary_tuple_count = 0;
        MSG_HIGH_1("invalid number of summary instances %d", count);
        return FALSE;
      }

      /* Copying MWI details information. */
      count = msg_event_info_ptr->transport_mwi_info.details_tuple_count;
      if (count > 0)
      {
        buffer_size = count * sizeof(wms_msg_mwi_detail_s_type);
        (*ob_evt)->ev_info.transport_mwi_info.mwi_msg_details = (wms_msg_mwi_detail_s_type *)wms_mem_malloc(buffer_size);
        MSG_HIGH_3("count = %d, buffer_size = %d, detail type size = %d", 
                   count, 
                   buffer_size, 
                   sizeof(wms_msg_mwi_detail_s_type));

        if (NULL != (*ob_evt)->ev_info.transport_mwi_info.mwi_msg_details)
        {
          (void)memscpy ((*ob_evt)->ev_info.transport_mwi_info.mwi_msg_details,
                         buffer_size,
                         msg_event_info_ptr->transport_mwi_info.mwi_msg_details,
                         buffer_size);
        }
        else
        {
          MSG_ERROR_0("Memory heap is exhausted");
          return FALSE;
        }
      }
      else
      {
        /* Details information is optional and may not be present. */
        (*ob_evt)->ev_info.transport_mwi_info.mwi_msg_details = NULL;
        (*ob_evt)->ev_info.transport_mwi_info.details_tuple_count = 0;
      }

      MSG_HIGH_2("Outbound event: summary = %d, details = %d", 
                 (*ob_evt)->ev_info.transport_mwi_info.summary_tuple_count,
                 (*ob_evt)->ev_info.transport_mwi_info.details_tuple_count);
    }
  } // Copy extras

  return TRUE;
} /* wms_msg_prep_and_copy_ob_msg_event() */

/*===========================================================================
FUNCTION wms_msg_prep_and_copy_ob_msg_parse

DESCRIPTION
  Make a heap-based copy of the outbound event data provided to the function
  wms_msg_event_notify() into a buffer to be sent to clients. The copy is
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
static boolean wms_msg_prep_and_copy_ob_msg_parse(
  wms_client_message_s_type      *cl_msg_ptr,
  wms_msg_outbound_parse_s_type **ob_evt
)
{
  if ( (NULL == ob_evt) || (NULL == cl_msg_ptr) )
  {
     return FALSE;
  }

  // Allocate a new buffer or zero the old
  if (NULL == *ob_evt)
  {
    *ob_evt = wms_mem_malloc(sizeof(wms_msg_outbound_parse_s_type));
  }

  if (NULL == *ob_evt)
  {
    MSG_HIGH_0("Malloc failed");
    return FALSE;
  }
  else {
    memset(*ob_evt, 0, sizeof(wms_msg_outbound_parse_s_type));
  }

  // Copy event
  (void)memscpy( &((*ob_evt)->ev_info),
                 sizeof(wms_client_message_s_type),
                 cl_msg_ptr,
                 sizeof(wms_client_message_s_type));

  return TRUE;
} /* wms_msg_prep_and_copy_ob_msg_parse() */

/*
** TRUE:  shared; can continue with processing this event
** FALSE: not-shared; stop processing this event
*/
boolean wms_msg_event_notify
(
  wms_msg_event_e_type        event,
  wms_msg_event_info_s_type   *msg_event_info_ptr
)
{
  wms_client_s_type             *client_ptr;
  uint32                         i;
  boolean                        shared = TRUE;
  wms_msg_outbound_event_s_type *ob_evt = NULL;   // Outbound event data

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (msg_event_info_ptr == NULL)
  {
    MSG_ERROR_0("msg_event_info_ptr null pointer!");
    return FALSE;
  }


  MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Notify: %d %s", event,
                wms_dbg_get_msg_event_string((uint16)event));

  {
     sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
     switch (event)
     {
     case WMS_MSG_EVENT_SEND:
     case WMS_MSG_EVENT_READ:
     case WMS_MSG_EVENT_WRITE:
     case WMS_MSG_EVENT_DELETE:
     case WMS_MSG_EVENT_DELETE_ALL:
     case WMS_MSG_EVENT_MODIFY_TAG:
     case WMS_MSG_EVENT_READ_TEMPLATE:
     case WMS_MSG_EVENT_WRITE_TEMPLATE:
     case WMS_MSG_EVENT_DELETE_TEMPLATE:
     case WMS_MSG_EVENT_DELETE_TEMPLATE_ALL:
     case WMS_MSG_EVENT_READ_STS_REPORT:
     case WMS_MSG_EVENT_WRITE_STS_REPORT:
     case WMS_MSG_EVENT_DELETE_STS_REPORT:
     case WMS_MSG_EVENT_DELETE_STS_REPORT_ALL:
     case WMS_MSG_EVENT_SMSC_ADDR_CHANGE:
        as_id = msg_event_info_ptr->status_info.as_id;
        break;
     case WMS_MSG_EVENT_ACK:
        as_id = msg_event_info_ptr->ack_info.as_id;
        break;
     case WMS_MSG_EVENT_RECEIVED_MESSAGE:
     case WMS_MSG_EVENT_STATUS_REPORT:
        as_id = msg_event_info_ptr->mt_message_info.as_id;
        break;
     case WMS_MSG_EVENT_SUBMIT_REPORT:
        as_id = msg_event_info_ptr->submit_report_info.as_id;
        break;
     case WMS_MSG_EVENT_MT_MESSAGE_ERROR:
        as_id = msg_event_info_ptr->mt_message_error.as_id;
        break;
     case WMS_MSG_EVENT_ACK_REPORT:
        as_id = msg_event_info_ptr->ack_report_info.as_id;
        break;
     case WMS_MSG_EVENT_TRANSPORT_REG:
        as_id = msg_event_info_ptr->transport_reg_info.as_id;
        break;
     default:
        MSG_ERROR_1("non dsds event %d", event);
        break;
     }
     MSG_HIGH_1("as_id = %d", as_id);
  }

  /* Notify clients
  */
  for( i=0; i<(uint32)WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr_fetch( (wms_client_id_type) i );
    if( client_ptr != NULL &&
        client_ptr->activated == TRUE &&
        client_ptr->msg_event_cb )
    {
      if (cfg_s_ptr->admin_sms_only &&
          (uint32)WMS_CLIENT_TYPE_QMI == i && WMS_MSG_EVENT_TRANSPORT_REG == event)
      {
         MSG_HIGH_0("Only admin sms can be sent over ims - send deregister event to QMI");
         msg_event_info_ptr->transport_reg_info.is_registered = FALSE;
      }
      if( TRUE == wms_msg_filter_events( (wms_client_id_type) i, client_ptr, event ))
      {
        // Make copy of outbound data FOR EACH CLIENT. The event data cannot be trusted
        // after a client has touched it. This way clients cannot corrupt the downstream
        // client notification data and any data, buffers, or pointers do not have to be
        // trusted or valid after the callback notification and the copy can be simply discarded.
        if ( wms_msg_prep_and_copy_ob_msg_event(event, msg_event_info_ptr, &ob_evt) && (NULL != ob_evt) )
        {
          client_ptr->msg_event_cb( event, &ob_evt->ev_info, &shared );
        }
      }

      if(shared == FALSE)
      {
        /* Message is consumed by the client, do not share this message with
        ** any other clients.
        */
        break;
      }
    }
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }

  WMS_MEM_FREEIF(ob_evt);

#if defined(FEATURE_SMS_PWS_OVER_LTE) || (defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS))
  if (WMS_MSG_EVENT_ETWS_NOTIFICATION == event)
  {
    switch(msg_event_info_ptr->etws_msg_info.type)
    {
      case WMS_ETWS_NOTIFICATION_PRIMARY:
        {
          wms_evt_etws_prim_msg_s_type          prim_evt;
          wms_etws_primary_notification_s_type *etws_prim_msg;

          etws_prim_msg = wms_mem_malloc(sizeof(wms_etws_primary_notification_s_type));

          if (NULL != etws_prim_msg)
          {
            wms_status_e_type  decode_status;

            decode_status = wms_ts_decode_etws_primary_notification( msg_event_info_ptr->etws_msg_info.data,
                                                                     msg_event_info_ptr->etws_msg_info.data_len,
                                                                     etws_prim_msg );
            if (WMS_OK_S == decode_status)
            {
              prim_evt.serial_number         = etws_prim_msg->cb_header_info.raw_serial_number;
              prim_evt.message_id            = etws_prim_msg->cb_header_info.cb_srv_id;
              prim_evt.warning_type          = (uint16)etws_prim_msg->warning_type;
              prim_evt.security_info_present = etws_prim_msg->security_info_present;

              event_report_payload(EVENT_SMS_ETWS_PRIM_RECEIVED, sizeof(prim_evt), (void *)&prim_evt);
            }

            wms_mem_free(etws_prim_msg);
          }
          else
          {
            MSG_ERROR_0("No memory for ETWS primary payload report");
          }
        }
        break;

      case WMS_ETWS_NOTIFICATION_SECONDARY_UMTS_FORMAT:
        {
          wms_evt_cbs_msg_s_type                   sec_evt;
          wms_etws_secondary_notification_s_type  *etws_sec_msg;

          etws_sec_msg = wms_mem_malloc(sizeof(wms_etws_secondary_notification_s_type));

          if (NULL != etws_sec_msg)
          {
            wms_status_e_type  decode_status;

            decode_status = wms_ts_decode_etws_secondary_notification( msg_event_info_ptr->etws_msg_info.data,
                                                                       msg_event_info_ptr->etws_msg_info.data_len,
                                                                       WMS_ETWS_NOTIFICATION_FORMAT_UMTS,
                                                                       etws_sec_msg );
            if (WMS_OK_S == decode_status)
            {
              sec_evt.message_id    = etws_sec_msg->cb_header_info.cb_srv_id;
              sec_evt.message_code  = etws_sec_msg->cb_header_info.message_code;
              sec_evt.geo_scope     = (uint8)etws_sec_msg->cb_header_info.geo_scope;
              sec_evt.update_number = etws_sec_msg->cb_header_info.update_number;
              sec_evt.total_pages   = etws_sec_msg->cb_header_info.total_pages;
              sec_evt.page_number   = etws_sec_msg->cb_header_info.page_number;
              sec_evt.dcs_data      = etws_sec_msg->cb_header_info.cb_dcs.raw_dcs_data;
              sec_evt.user_alert    = etws_sec_msg->activate_emergency_user_alert;
              sec_evt.popup         = etws_sec_msg->activate_popup;
              sec_evt.format        = (uint8)etws_sec_msg->format;
              sec_evt.data_len      = (uint16)etws_sec_msg->cb_data_len;

              event_report_payload(EVENT_SMS_CBS_MSG_RECEIVED, sizeof(sec_evt), (void *)&sec_evt);
            }

            wms_mem_free(etws_sec_msg);
          }
          else
          {
            MSG_ERROR_0("No memory for ETWS secondary payload report");
          }
        }
        break;

      default:
        break;
    }
  }

#endif /*defined(FEATURE_SMS_PWS_OVER_LTE) || (defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS))*/

#ifdef FEATURE_FUZZ_TEST
  if (WMS_MSG_EVENT_RECEIVED_MESSAGE == event)
  {
      wms_evt_sms_mt_notify_s_type evt;
      wms_client_ts_data_s_type    client_ts_data;
      boolean                      send_diag_event = FALSE;
      uint8 data_len = 0;

      memset(evt.user_data, 0, sizeof(evt.user_data));

      switch(msg_event_info_ptr->mt_message_info.message.msg_hdr.message_mode)
      {
#ifdef FEATURE_CDSMS
      case WMS_MESSAGE_MODE_CDMA:
            if( WMS_OK_S ==
                wms_ts_decode( &msg_event_info_ptr->mt_message_info.message.u.cdma_message.raw_ts,
                               &client_ts_data ) )
            {
              data_len = MIN(sizeof(evt.user_data), client_ts_data.u.cdma.user_data.data_len);
              (void)memscpy( evt.user_data,
                             data_len,
                             client_ts_data.u.cdma.user_data.data,
                             data_len);
              send_diag_event = TRUE;
            }
            else
            {
              MSG_ERROR_0("SMS Fuzzing: Decoding failed for CDMA MT SMS");
            }
         break;
#endif /*FEATURE_CDSMS*/

#ifdef FEATURE_GWSMS
      case WMS_MESSAGE_MODE_GW:
         if ( WMS_OK_S ==
              wms_ts_decode( &msg_event_info_ptr->mt_message_info.message.u.gw_message.raw_ts_data,
                             &client_ts_data ) )
         {
           if (msg_event_info_ptr->mt_message_info.message.u.gw_message.is_broadcast)
           {
             data_len = MIN(sizeof(evt.user_data), client_ts_data.u.gw_cb.user_data.sm_len);
             (void)memscpy( evt.user_data,
                            data_len,
                            client_ts_data.u.gw_cb.user_data.sm_data,
                            data_len);
           }
           else
           {
              data_len = MIN(sizeof(evt.user_data), client_ts_data.u.gw_pp.u.deliver.user_data.sm_len);
              (void)memscpy( evt.user_data,
                             data_len,
                             client_ts_data.u.gw_pp.u.deliver.user_data.sm_data,
                             data_len);
           }
           send_diag_event = TRUE;
         }
         else
         {
           MSG_ERROR_0("SMS Fuzzing: Decoding failed for GW MT SMS");
         }
         break;
#endif /*FEATURE_GWSMS*/

      default:
         break;
      }
      if(TRUE == send_diag_event)
      {
        event_report_payload(EVENT_SMS_MT_NOTIFY, sizeof(evt), (void*)&evt);
      }
  }
#endif/*FEATURE_FUZZ_TEST*/

  /* done */
  return shared;

} /* wms_msg_event_notify() */

wms_client_type_e_type wms_msg_parse_msg
(
  wms_client_message_s_type   *cl_msg_ptr
)
{
  wms_client_s_type             *client_ptr;
  uint32                         i;
  wms_client_type_e_type         client_id = WMS_CLIENT_TYPE_MAX;
  boolean                        shared = TRUE;
  wms_msg_outbound_parse_s_type *ob_evt = NULL;   // Outbound event data

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_0("wms_msg_parse_msg() called");

  /* Notify clients
  */
  for( i=0; i<(uint8)WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr_fetch( (wms_client_id_type) i );
    if( client_ptr != NULL &&
        client_ptr->activated == TRUE &&
        client_ptr->parse_msg_cb )
    {
      // Make copy of outbound data FOR EACH CLIENT. The event data cannot be trusted
      // after a client has touched it. This way clients cannot corrupt the downstream
      // client notification data and any data, buffers, or pointers do not have to be
      // trusted or valid after the callback notification and the copy can be simply discarded.
      if ( wms_msg_prep_and_copy_ob_msg_parse(cl_msg_ptr, &ob_evt) && (NULL != ob_evt) )
      {
        client_ptr->parse_msg_cb( &ob_evt->ev_info, &shared );
      }

      if(shared == FALSE)
      {
        /* Message is consumed by the client, do not share this message with
        ** any other clients.
        */
        client_id = (wms_client_type_e_type)i;
        break;
      }
    }
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }

  WMS_MEM_FREEIF(ob_evt);

  /* done */
  return client_id;
} /* wms_msg_parse_msg */


/*
*/
void wms_msg_event_notify_single_client
(
  wms_client_type_e_type      client,
  wms_msg_event_e_type        event,
  wms_msg_event_info_s_type   *msg_event_info_ptr
)
{
  wms_client_s_type           *client_ptr;

    MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Notify: %d %s", event,
                  wms_dbg_get_msg_event_string((uint16)event));

  /* Notify the client
  */
  client_ptr = wms_client_ptr_fetch( (wms_client_id_type) client );
  if( client_ptr != NULL &&
      client_ptr->activated == TRUE &&
      client_ptr->msg_event_cb )
  {
    if( TRUE == wms_msg_filter_events( (wms_client_id_type) client, client_ptr, event ))
    {
       wms_msg_outbound_event_s_type *ob_evt = NULL;   // Outbound event data

       // Make copy of outbound data FOR EACH CLIENT. The event data cannot be trusted
       // after a client has touched it. This way clients cannot corrupt the downstream
       // client notification data and any data, buffers, or pointers do not have to be
       // trusted or valid after the callback notification and the copy can be simply discarded.
       if ( wms_msg_prep_and_copy_ob_msg_event(event, msg_event_info_ptr, &ob_evt) && (NULL != ob_evt) )
       {
         client_ptr->msg_event_cb( event, &ob_evt->ev_info, NULL );
       }

       if (NULL != ob_evt && WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION == event)
       {
           /* Free the summary and detail buffers if memory was allocated for
               WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION event. */
           WMS_MEM_FREEIF(ob_evt->ev_info.transport_mwi_info.mwi_msg_summary);
           WMS_MEM_FREEIF(ob_evt->ev_info.transport_mwi_info.mwi_msg_details);
       }

       WMS_MEM_FREEIF(ob_evt);
    }
  }

  WMS_CLIENT_PTR_RELEASEIF(client_ptr);

  /* done */
  return;

} /* wms_msg_event_notify_single_client() */

/*=========================================================================
FUNCTION wms_msg_check_sc_addr_is_changed()                     (INTERNAL)

DESCRIPTION
  Compares the two smsc templates given as input.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If the smsc addresses don't match.
  FALSE -If the smsc addresses match.

SIDE EFFECTS
  None
=========================================================================*/
boolean wms_msg_check_sc_addr_is_changed(
  wms_address_s_type *sc_addr_ptr1,
  wms_address_s_type *sc_addr_ptr2
)
{
  /*-------------------------------------------------------------------------
  Check if the SMSC addresses match -  Must compare each member to see if there is a 
  difference.  Binary comparison won't work because the sc_addr.digits buffer data should be
  ignored after sc_addr.number_of_digits count.
  -------------------------------------------------------------------------*/

  if ((sc_addr_ptr1->number_type      == sc_addr_ptr2->number_type) &&
      (sc_addr_ptr1->number_plan      == sc_addr_ptr2->number_plan) &&
      (sc_addr_ptr1->number_of_digits == sc_addr_ptr2->number_of_digits)&&
      (!memcmp(&(sc_addr_ptr1->digits), &(sc_addr_ptr2->digits), sc_addr_ptr1->number_of_digits)))
  {
    MSG_HIGH_0( "SMSC addresses match" );
    return FALSE;
  }
  
  return TRUE;
}  /* wms_msg_check_sc_addr_is_changed */

#ifdef FEATURE_GWSMS

wms_cmd_err_e_type wms_msg_check_tpdu_type
(
  wms_client_message_s_type      * msg_ptr,     /* IN */
  boolean                        store_message  /* IN */
)
{
  wms_cmd_err_e_type                cmd_err = WMS_CMD_ERR_NONE;
  if (msg_ptr == NULL)
  {
    MSG_ERROR_0("msg_ptr null pointer!");
    return WMS_CMD_ERR_NONE;
  }

  switch(msg_ptr->u.gw_message.raw_ts_data.tpdu_type)
  {
    case WMS_TPDU_SUBMIT:
      if ( ! WMS_IS_MO( msg_ptr->msg_hdr.tag ) )
      {
        cmd_err = WMS_CMD_ERR_MSG_TAG;
        break;
      }
      if ((msg_ptr->u.gw_message.raw_ts_data.data[0] & 0x03)!= WMS_MESSAGE_TYPE_SUBMIT)
      {
        cmd_err= WMS_CMD_ERR_MSG_TPDU_TYPE;
      }
      break;

    case WMS_TPDU_DELIVER:
    case WMS_TPDU_RESERVED:
      if ( ! WMS_IS_MT( msg_ptr->msg_hdr.tag ) )
      {
        cmd_err = WMS_CMD_ERR_MSG_TAG;
        break;
      }
      if( ( (msg_ptr->u.gw_message.raw_ts_data.data[0] & 0x03) != WMS_MESSAGE_TYPE_DELIVER)
          && ( (msg_ptr->u.gw_message.raw_ts_data.data[0] & 0x03) != WMS_MESSAGE_TYPE_RESERVED) )

      {
        cmd_err= WMS_CMD_ERR_MSG_TPDU_TYPE;
      }
      break;

    case WMS_TPDU_COMMAND:
      if (store_message)
      {
        cmd_err = WMS_CMD_ERR_MSG_TPDU_TYPE;
      }
      else
      {
        if ( ! WMS_IS_MO( msg_ptr->msg_hdr.tag ) )
        {
          cmd_err = WMS_CMD_ERR_MSG_TAG;
          break;
        }
        if ((msg_ptr->u.gw_message.raw_ts_data.data[0] & 0x03)!= WMS_MESSAGE_TYPE_COMMAND)
        {
          cmd_err= WMS_CMD_ERR_MSG_TPDU_TYPE;
        }
      }
      break;

    case WMS_TPDU_STATUS_REPORT:
      if (store_message)
      {
        cmd_err = WMS_CMD_ERR_MSG_TPDU_TYPE;
      }
      else
      {
        if ( msg_ptr->msg_hdr.tag != WMS_TAG_STATUS_RPT )
        {
          cmd_err = WMS_CMD_ERR_MSG_TAG;
          break;
        }
        if ((msg_ptr->u.gw_message.raw_ts_data.data[0] & 0x03)!=  WMS_MESSAGE_TYPE_STATUS_REPORT)
        {
          cmd_err= WMS_CMD_ERR_MSG_TPDU_TYPE;
        }
      }
      break;

    default:
      MSG_HIGH_0("Unrecognized command for TPDU type");
      cmd_err= WMS_CMD_ERR_MSG_TPDU_TYPE;
      break;
  }
  return cmd_err;
}

#endif /* FEATURE_GWSMS */


#ifdef FEATURE_CDSMS
/* Check for duplicate messages.
** Return value of TRUE means a duplicate was found, else FALSE.
*/
boolean wms_msg_cdma_check_dups
(
  const wms_tl_message_type     *new_tl_msg
)
{
#ifdef FEATURE_CDSMS_DUP_DETECTION


  boolean                           dup_found = TRUE;
  uint32                            i;

  MSG_HIGH_0("Checking for duplicate messages!");


  if (new_tl_msg == NULL)
  {
    MSG_ERROR_0("new_tl_msg null pointer!");
    return FALSE;
  }


  if (cdma_tl.teleservice == WMS_TELESERVICE_WAP ||
      (wms_cm_CT_features_supported(wms_get_appinfo_1x_asid()) &&
       cdma_tl.teleservice == WMS_TELESERVICE_CT_CDMA_WAP)
     )
  {
    /* No dup detection to be done for the above teleservices
    */
    return FALSE;
  }


  /* Chech each message in the info cache for a duplicate */
  for(i=0; i<WMS_MAX_CDMA_MESSAGES_IN_MEM; i++)
  {
    /* Initialize dup_found for every check */
    dup_found = FALSE;

    /* Only compare to valid messages in the dup detect list */
    if( NULL == cfg_s_ptr->dup_detect[i] )
    {
      /* Go on to the next index in cache */
      continue;
    }

    /* Check matching msg id and address parameters against the message
    ** from the dup detection cache (mandatory fields).
    */
    if(new_tl_msg->cl_bd.mask & WMS_MASK_BD_MSG_ID)
    {
      if(new_tl_msg->cl_bd.message_id.id_number ==
         cfg_s_ptr->dup_detect[i]->msg_id.id_number)
      {
        /* Check address (mandatory field) */
        if((new_tl_msg->address.digit_mode  ==
            cfg_s_ptr->dup_detect[i]->address.digit_mode) &&
           (new_tl_msg->address.number_mode ==
            cfg_s_ptr->dup_detect[i]->address.number_mode) &&
           (new_tl_msg->address.number_of_digits ==
            cfg_s_ptr->dup_detect[i]->address.number_of_digits))
        {
          if(new_tl_msg->address.number_of_digits == 0)
          {
            /* might be an address for broadcast where the number of digits will
            ** be 0, might be a match.
            */
            dup_found = TRUE;
          }
          else
          {
            int k;
            for(k=0; k<new_tl_msg->address.number_of_digits; k++)
            {
              if(new_tl_msg->address.digits[k] ==
                cfg_s_ptr->dup_detect[i]->address.digits[k])
              {
                /* Possible match */
                dup_found = TRUE;
              }
              else
              {
                /* Mismatch in address digits */
                dup_found = FALSE;
                break; /* get out of this loop early */
              }
            }
          }
        }
        else
        {
          /* Address does not match, therefore this is not a duplicate */
          dup_found = FALSE;
        }
      }
      else /* Msg ID does not match with this msg from cache */
      {
        dup_found = FALSE;
      }

      /* Keep checking we may have a duplicate (optional parameters) */
      if(dup_found)
      {
        /* Possible duplicate, check mc timestamp
        ** These instances are considered duplicates:
        ** (1) Msg ids and address are the same and timestamp is the same.
        ** (2) Msg ids and address are the same and both messages do not have
        **     a mc timestamp.
        */
        if((new_tl_msg->cl_bd.mask & WMS_MASK_BD_MC_TIME) &&
           (cfg_s_ptr->dup_detect[i]->cl_mask & WMS_MASK_BD_MC_TIME) &&
           (new_tl_msg->cl_bd.mc_time.year ==
              cfg_s_ptr->dup_detect[i]->mc_time.year)   &&
           (new_tl_msg->cl_bd.mc_time.month ==
              cfg_s_ptr->dup_detect[i]->mc_time.month)  &&
           (new_tl_msg->cl_bd.mc_time.day ==
              cfg_s_ptr->dup_detect[i]->mc_time.day)    &&
           (new_tl_msg->cl_bd.mc_time.hour ==
              cfg_s_ptr->dup_detect[i]->mc_time.hour)   &&
           (new_tl_msg->cl_bd.mc_time.minute ==
              cfg_s_ptr->dup_detect[i]->mc_time.minute) &&
           (new_tl_msg->cl_bd.mc_time.second ==
              cfg_s_ptr->dup_detect[i]->mc_time.second))
        {
          /* Timestamp & msg_id match */
          dup_found = TRUE;
        }
        else if(((new_tl_msg->cl_bd.mask & WMS_MASK_BD_MC_TIME) == 0) &&
                ((cfg_s_ptr->dup_detect[i]->cl_mask & WMS_MASK_BD_MC_TIME) == 0))
        {
          /* No timestamp & msg_id match */
          dup_found = TRUE;
        }
        else
        {
          /* MC timestamp mis-match, this is not a duplicate. */
          dup_found = FALSE;
        }
      }

      /* Keep checking we may have a duplicate (optional parameters) */
      if(dup_found)
      {
        /* Possible duplicate, check subaddress only if it is available */
        if(new_tl_msg->mask & WMS_MASK_TL_SUBADDRESS)
        {
          /* Subaddress is available in the new message, check if the cached
          ** message has a subaddress to compare
          */
          if(cfg_s_ptr->dup_detect[i]->tl_mask & WMS_MASK_TL_SUBADDRESS)
          {
            /* Compare all subparameters of the subaddress */
            if((new_tl_msg->subaddress.number_of_digits ==
                  cfg_s_ptr->dup_detect[i]->sub_address.number_of_digits) &&
               (new_tl_msg->subaddress.type ==
                  cfg_s_ptr->dup_detect[i]->sub_address.type) &&
               (new_tl_msg->subaddress.odd ==
                  cfg_s_ptr->dup_detect[i]->sub_address.odd))
            {
              int n;

              /* Compare the digits */
              for(n=0; n<new_tl_msg->subaddress.number_of_digits; n++)
              {
                if(new_tl_msg->subaddress.digits[n] ==
                    cfg_s_ptr->dup_detect[i]->sub_address.digits[n])
                {
                  /* Possible match */
                  dup_found = TRUE;
                }
                else
                {
                  dup_found = FALSE;
                  /* get out of this loop */
                  break;
                }
              }
            }
            else
            {
              /* Sub-address mismatch */
              dup_found = FALSE;
            }
          }
          else
          {
            /* New msg has sub-address, but msg from cache does not. */
            dup_found = FALSE;
          }
        }
        else if(cfg_s_ptr->dup_detect[i]->tl_mask & WMS_MASK_TL_SUBADDRESS)
        {
          /* The new message does not have a sub-address, but our message info
          ** in cache does, therefore this is not a duplicate.
          */
          dup_found = FALSE;
        }
        else
        {
          /* Both messages do not have a sub-address parameter to check.
          ** Keep dup_found as TRUE;
          */
        }
      }

      /* Final Check with dup found.  If we found a duplicate do an early
      ** return.  Otherwise, keep checking.
      */
      if(dup_found)
      {
        MSG_HIGH_0("Duplicate Message found!");
        return dup_found;
      }
    }
    else /* No Msg ID field, this is an invalid message. */
    {
      MSG_ERROR_0("Invalid Message in duplicate detect");
      return FALSE;
    }
  }

  /* If we got this far, a duplicate was not found. */
  return dup_found;
#else /* FEATURE_CDSMS_DUP_DETECTION */
  return FALSE;
#endif /* FEATURE_CDSMS_DUP_DETECTION */
} /* wms_msg_cdma_check_dups() */

/*=========================================================================
FUNCTION
  wms_msg_cdma_next_seq_num

DESCRIPTION
  This function returns the next SMS TL sequence number to be used for
  message origination, which increments by 1 each time, starting from 0.
  It goes back to 0 again when the max sequence number is reached.

DEPENDENCIES
  None

RETURN VALUE
  The next sequence number to be used.

SIDE EFFECTS
  Increment the internal sequence number.

=========================================================================*/
uint8  wms_msg_cdma_next_seq_num
(
  void
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_s_ptr->tl_seq_num = (msg_s_ptr->tl_seq_num+1) % WMS_TL_SEQ_NUM_MAX;

  return msg_s_ptr->tl_seq_num;

} /* end of wms_msg_cdma_next_seq_num() */

/*=========================================================================
FUNCTION
  wms_msg_cdma_next_non_tl_seq_num

DESCRIPTION
  This function returns the next SMS NON TL sequence number to be used for
  message origination, which increments by 1 each time, starting from 0.
  It goes back to 0 again when the max sequence number is reached.

DEPENDENCIES
  None

RETURN VALUE
  The next sequence number to be used for message not requesting a tl ack.

SIDE EFFECTS
  Increment the internal sequence number.

=========================================================================*/
uint8  wms_msg_cdma_next_non_tl_seq_num
(
  void
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_s_ptr->non_tl_seq_num = (msg_s_ptr->non_tl_seq_num+1)%WMS_TL_SEQ_NUM_MAX;

  /* The bit wise OR will signify this is a message with no tl_ack requested */
  return msg_s_ptr->non_tl_seq_num | WMS_DUMMY_SEQ_NUM;

} /* end of wms_msg_cdma_next_non_tl_seq_num() */

/*===========================================================================
FUNCTION   wms_msg_cdma_have_mo_rec

DESCRIPTION
  Indication for MO record memory allocation limit defined by
  "WMS_MAX_MO_MESSAGES".

DEPENDENCIES
  None

RETURNS
  FALSE - Reached limit.
  TRUE -  Within limit.

SIDE EFFECTS
  None
===========================================================================*/
static boolean wms_msg_cdma_have_mo_rec(
void
)
{
  boolean result = FALSE;

  MSG_HIGH_1("wms_msg_cdma_have_mo_rec(): current rec link count = %d", msg_s_ptr->cdma_mo_rec.count);

  if ( WMS_MAX_MO_MESSAGES > msg_s_ptr->cdma_mo_rec.count )
  {
    result = TRUE;
  }

  return result;

} /* wms_msg_cdma_have_mo_rec() */


/*===========================================================================
FUNCTION   wms_msg_cdma_get_mo_rec

DESCRIPTION
   Malloc for a CDMA MO record and add to linked list.

DEPENDENCIES
  None

RETURNS
  NULL - No memory is allocated.
  Non-NULL - New allocated mo record.

SIDE EFFECTS
  None
===========================================================================*/
static wms_msg_cdma_message_record_type *wms_msg_cdma_get_mo_rec(
  void
)
{
  wms_msg_cdma_message_record_type *new_rec_ptr = NULL;

  /* Proceed only if list count does not exceed the allocation limit.*/
  if ( wms_msg_cdma_have_mo_rec() )
  {
    new_rec_ptr = (wms_msg_cdma_message_record_type *)wms_mem_malloc(sizeof(wms_msg_cdma_message_record_type));

    if ( NULL == new_rec_ptr )
    {
      MSG_HIGH_0("wms_msg_cdma_get_mo_rec(): failed");
    }
    else
    {
      /* Set the initial default values */
      new_rec_ptr->state   = WMS_MSG_STATE_IDLE;
      new_rec_ptr->seq_num = WMS_DUMMY_SEQ_NUM;
      new_rec_ptr->resend_msg = FALSE;
      new_rec_ptr->client_msg_id =  WMS_DUMMY_MESSAGE_NUMBER;
      new_rec_ptr->ch_pref = WMS_CLIENT_CH_NONE;
      new_rec_ptr->dc_so = WMS_DC_SO_AUTO;
      new_rec_ptr->prev = NULL;
      new_rec_ptr->next = NULL;

      /* Add new record to linked list.*/
      if ( NULL == msg_s_ptr->cdma_mo_rec.head_ptr )
      {
        /* First record link. */
        msg_s_ptr->cdma_mo_rec.head_ptr = new_rec_ptr;
        msg_s_ptr->cdma_mo_rec.tail_ptr = new_rec_ptr;
      }
      else
      {
        /* Add new record to the tail end of list. */
        msg_s_ptr->cdma_mo_rec.tail_ptr->next = new_rec_ptr;
        new_rec_ptr->prev = msg_s_ptr->cdma_mo_rec.tail_ptr;
        msg_s_ptr->cdma_mo_rec.tail_ptr = new_rec_ptr;
      }

      msg_s_ptr->cdma_mo_rec.count++;

      MSG_HIGH_2( "wms_msg_cdma_get_mo_rec(): new allocated rec = 0x%x, current rec link count = %d)",
                  new_rec_ptr, msg_s_ptr->cdma_mo_rec.count );
    }
  }

  return new_rec_ptr;
} /* wms_msg_cdma_get_mo_rec() */

/*===========================================================================
FUNCTION   wms_msg_cdma_map_mo_rec

DESCRIPTION
  Search MO record linked list for a link that is active and has matching
  sequence number.

DEPENDENCIES
  None

RETURNS
  NULL - No matching record found
  Non-NULL - Matching record found

SIDE EFFECTS
  None
===========================================================================*/
static wms_msg_cdma_message_record_type *wms_msg_cdma_map_mo_rec(
  uint8  seq_num
)
{
  wms_msg_cdma_message_record_type *rec_ptr = msg_s_ptr->cdma_mo_rec.head_ptr;

  while ( NULL != rec_ptr )
  {
    if ( (seq_num == rec_ptr->seq_num) &&
         (WMS_MSG_STATE_IDLE != rec_ptr->state) )
    {
      MSG_HIGH_2( "wms_msg_cdma_map_mo_rec(): Found matched seq_num %d in MO rec 0x%x",
                  rec_ptr->seq_num, rec_ptr );
      break;
    }
    rec_ptr = rec_ptr->next;
  }

  return rec_ptr;
} /* wms_msg_cdma_map_mo_rec() */

/*===========================================================================
FUNCTION   wms_msg_waiting_for_mo_or_mt_status

DESCRIPTION
  Check if there is any pending MO SMS or TL ack waiting for MO status

DEPENDENCIES
  None

RETURNS
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
static boolean wms_msg_waiting_for_mo_or_mt_status(
  void
)
{
  boolean wait_for_mo_status = FALSE;
  wms_msg_cdma_message_record_type *rec_ptr;

  MSG_HIGH_2( "msg_s_ptr->mc_msg.rec_ptr = 0x%x, msg_s_ptr->mc_msg.is_ack = %d",
              msg_s_ptr->mc_msg.rec_ptr, msg_s_ptr->mc_msg.is_ack );

  rec_ptr = msg_s_ptr->cdma_mo_rec.head_ptr;

  // Check for pending MO SMS first
  while ( NULL != rec_ptr )
  {
    if ( WMS_MSG_STATE_LAYER2_IN_PROGRESS == rec_ptr->state )
    {
      MSG_HIGH_2( "wms_msg_waiting_for_mo_or_mt_status(): Found MO rec 0x%x with seq_num %d waiting for MO status",
                  rec_ptr, rec_ptr->seq_num );
      wait_for_mo_status = TRUE;
      break;
    }
    rec_ptr = rec_ptr->next;
  }

  // If not found, check for pending TL ack
  if (!wait_for_mo_status)
  {
    int i;
    for (i = 0; i < WMS_MAX_MT_MESSAGES; i++)
    {
      rec_ptr = msg_s_ptr->cdma_mt_rec[i];
      if ( (NULL != rec_ptr) &&
           (WMS_MSG_STATE_LAYER2_IN_PROGRESS == rec_ptr->state) )
      {
        MSG_HIGH_2( "wms_msg_waiting_for_mo_or_mt_status(): Found MT rec %d with seq_num %d waiting for MO status",
                    i, rec_ptr->seq_num);
        wait_for_mo_status = TRUE;

        break;
      }
    }
  }

  return wait_for_mo_status;
} /* end of wms_msg_waiting_for_mo_or_mt_status() */

/*===========================================================================
FUNCTION   wms_msg_cdma_get_next_mo_rec_before_delete

DESCRIPTION
  Free MO msg record from linked list.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static wms_msg_cdma_message_record_type *wms_msg_cdma_get_next_mo_rec_before_delete(
  wms_msg_cdma_message_record_type *delete_rec_ptr
)
{
   wms_msg_cdma_message_record_type *rec_ptr = NULL;
   wms_msg_cdma_message_record_type *delete_rec_ptr_next = NULL;
   boolean                           follow_on_dc = TRUE; 

  /* nothing to do if either delete link node or list is emply. */
  if( (NULL == delete_rec_ptr) || (NULL == msg_s_ptr->cdma_mo_rec.head_ptr) )
  {
    MSG_ERROR_2( "delete_rec_ptr NULL(0x%x) or head_ptr NULL(0x%x)",
                 delete_rec_ptr, msg_s_ptr->cdma_mo_rec.head_ptr);
    return NULL;
  }

  /* Traverse through list to find a matched record link */
  rec_ptr = msg_s_ptr->cdma_mo_rec.head_ptr;
  while ( NULL != rec_ptr )
  {
    if ( rec_ptr == delete_rec_ptr )
    {
      /* the delete record node is good so get the next record link
         before deleting it.
      */
      delete_rec_ptr_next = delete_rec_ptr->next;

      if ( NULL != delete_rec_ptr->prev )
      {
        if ( NULL != delete_rec_ptr->next)
        {
          /* "#=X=#", Both previous and next links are valid, so just bypass
             the delele link without changing the head/tail ptr.
          */
          delete_rec_ptr->prev->next = delete_rec_ptr->next;
          delete_rec_ptr->next->prev = delete_rec_ptr->prev;
        }
        else
        {
          /* "#=X=0", only the previous link is valid, so prev is now the
              last link in the list.
          */
          delete_rec_ptr->prev->next = NULL;
          msg_s_ptr->cdma_mo_rec.tail_ptr = delete_rec_ptr->prev;
        }
      }
      else
      {
        if ( NULL != delete_rec_ptr->next )
        {
          /* 0=X=#, only the next link is valid, not the previous one, so
             "next" is now the first link in "list".
          */
          delete_rec_ptr->next->prev = NULL;
          msg_s_ptr->cdma_mo_rec.head_ptr = delete_rec_ptr->next;
        }
        else
        {
          /* 0=X=0, Neither previous nor next links are valid, so the list
             is now empty.
          */
          msg_s_ptr->cdma_mo_rec.head_ptr = NULL;
          msg_s_ptr->cdma_mo_rec.tail_ptr = NULL;
        }
      }

      MSG_HIGH_2( "wms_msg_cdma_get_next_mo_rec_before_delete(): rec 0x%x with seq_num %d is deleted.",
                  delete_rec_ptr, delete_rec_ptr->seq_num);
      follow_on_dc = delete_rec_ptr->follow_on_dc;
      msg_s_ptr->cdma_mo_rec.count--;
      WMS_MEM_FREEIF( delete_rec_ptr->ota );
      wms_mem_free( delete_rec_ptr );
      break;
    }

    rec_ptr = rec_ptr->next;
  } /* end while() */

  if ( NULL == rec_ptr )
  {
    /* Search list has exausted and no matching record found. */
    MSG_HIGH_1("wms_msg_cdma_get_next_mo_rec_before_delete(): rec 0x%x not found.", delete_rec_ptr);
    return NULL;
  }
  // If call is active,
  // AND If auto disconnect is not turned on,
  // AND If call was initiated by us
  if( dc_s_ptr->call_active &&
    dc_s_ptr->wms_initiated_call )
  {
    if ( !follow_on_dc ) 
    {
      MSG_HIGH_0("follow on DC not set - Initiate traffic call tear down.");
      if( WMS_OK_S != wms_dc_disconnect ( (wms_client_id_type) WMS_CLIENT_TYPE_INTERNAL,
                                           NULL,
                                           NULL ) )
      {
        MSG_ERROR_0("wms_dc_disconnect failed!!");
      }
    }
    else if ( !dc_s_ptr->auto_disc_active )
    {
      cdma_mo_next_retry_rec_s_type next_retry_rec;
      boolean                       busy;

      // AND MO queues are free
      busy = wms_msg_cdma_mo_busy( &next_retry_rec );
      if ( ((WMS_MSG_STATE_QUEUED == next_retry_rec.state) &&
        (WMS_CLIENT_CH_PREF_ACH_ONLY == next_retry_rec.ch_pref)) ||
        !busy )
      {
        MSG_HIGH_0("WMS initiated traffic call tear down.");
        // dc_s_ptr->wms_initiated_call will be reset after the call is actually disconnected
        if( WMS_OK_S != wms_dc_disconnect ( (wms_client_id_type) WMS_CLIENT_TYPE_INTERNAL,
          NULL,
          NULL ) )
        {
          // Ideally, this should not fail.  However, if it did,
          // it would be re-tried the next time we are clearing the
          // records, and more importantly, in real networks, the
          // network itself would force and tear down the traffic
          // channel itself ...so no extra processing needed.
          MSG_ERROR_0("wms_dc_disconnect failed!!");
        }
      }
    }
  }

  if ( !wms_msg_waiting_for_mo_or_mt_status() )
  {
    msg_s_ptr->mc_msg.is_valid = FALSE;
    msg_s_ptr->mc_msg.rec_ptr = NULL;
  }

  return delete_rec_ptr_next;
} /* end of wms_msg_cdma_get_next_mo_rec_before_delete() */

/*
*/
/*===========================================================================
FUNCTION   wms_msg_cdma_get_free_mt_rec

DESCRIPTION
  Get free CDMA MT rec.

DEPENDENCIES
  None

RETURNS
  NULL - No free record found
  Non-NULL - Free record found

SIDE EFFECTS
  None
===========================================================================*/
static wms_msg_cdma_message_record_type* wms_msg_cdma_get_free_mt_rec(void)
{
  int32   i;

  for( i=0; i<WMS_MAX_MT_MESSAGES; i++ )
  {
    if( NULL == msg_s_ptr->cdma_mt_rec[i] )
    {
       msg_s_ptr->cdma_mt_rec[i] = (wms_msg_cdma_message_record_type*) wms_mem_malloc(sizeof(wms_msg_cdma_message_record_type));

       if(NULL == msg_s_ptr->cdma_mt_rec[i])
       {
          break;
       }

       msg_s_ptr->cdma_mt_rec[i]->state   = WMS_MSG_STATE_IDLE;
       msg_s_ptr->cdma_mt_rec[i]->seq_num = WMS_DUMMY_SEQ_NUM;
       msg_s_ptr->cdma_mt_rec[i]->resend_msg = FALSE;
       msg_s_ptr->cdma_mt_rec[i]->ch_pref       = WMS_CLIENT_CH_PREF_ACH_OR_TCH;
       msg_s_ptr->cdma_mt_rec[i]->client_msg_id = WMS_DUMMY_MESSAGE_NUMBER;
       rex_def_timer( &msg_s_ptr->cdma_mt_rec[ i ]->timer,
                      rex_self(),
                      WMS_MT_ACK_TIMER_SIG );

       return msg_s_ptr->cdma_mt_rec[i];
    }
  }

  return NULL;

} /* end of wms_msg_cdma_get_free_mt_rec() */

/*
*/
/*===========================================================================
FUNCTION   wms_msg_cdma_map_mt_rec

DESCRIPTION
  Search the MT record queue for the record that is active and has a matching
  sequence number.

DEPENDENCIES
  None

RETURNS
  NULL - No matching record found
  Non-NULL - Matching record found

SIDE EFFECTS
  None
===========================================================================*/
static wms_msg_cdma_message_record_type* wms_msg_cdma_map_mt_rec
(
  uint8 seq_num
)
{
  int32   i;
  wms_msg_cdma_message_record_type  *rec_ptr = NULL;

  for( i=0; i<WMS_MAX_MT_MESSAGES; i++ )
  {
    if( NULL != msg_s_ptr->cdma_mt_rec[i] &&
        seq_num == msg_s_ptr->cdma_mt_rec[i]->seq_num &&
        WMS_MSG_STATE_IDLE != msg_s_ptr->cdma_mt_rec[i]->state )
    {
      /* found the matching record */
      rec_ptr = msg_s_ptr->cdma_mt_rec[i];
      break;
    }
  }

  return rec_ptr;

} /* end of wms_msg_cdma_map_mt_rec() */

/*===========================================================================
FUNCTION   wms_msg_cdma_free_mt_msg_rec

DESCRIPTION
  Free msg record.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Reset the record.
===========================================================================*/
static void wms_msg_cdma_free_mt_msg_rec
(
  wms_msg_cdma_message_record_type *rec_ptr
)
{
  int32   i;

  if(NULL == rec_ptr)
  {
    MSG_ERROR_0("Null rec_ptr!");
    return;
  }

  for( i=0; i<WMS_MAX_MT_MESSAGES; i++ )
  {
    if( rec_ptr == msg_s_ptr->cdma_mt_rec[i] )
    {
      MSG_HIGH_1("clr msg rec with seq_num = %d ", rec_ptr->seq_num);

      (void) rex_clr_timer( & rec_ptr->timer );
      /* May not needed but for safety only */
      WMS_MEM_FREEIF( rec_ptr->ota );
      WMS_MEM_FREEIF( msg_s_ptr->cdma_mt_rec[i] );
      break;
    }
  }

  if(WMS_MAX_MT_MESSAGES == i)
  {
     MSG_HIGH_0("Matching record not found");
     return;
  }
  /* Need to reset state of mc_msg.is_valid to unblocked the next
     MO message since dealocation of this MT record could be triggered
     by the retry logic which timed-out waiting for L2 ACK.
  */
  if ( !wms_msg_waiting_for_mo_or_mt_status() )
  {
    msg_s_ptr->mc_msg.is_valid = FALSE;
    msg_s_ptr->mc_msg.rec_ptr = NULL;
  }
} /* end of wms_msg_cdma_free_mt_msg_rec() */


/*===========================================================================
FUNCTION wms_msg_cdma_ok_to_send_sms

DESCRIPTION
  Check if MC is ready to send an SMS message.

DEPENDENCIES
  None

RETURNS
  True: ready; False: not ready.

SIDE EFFECTS
  None

===========================================================================*/
boolean wms_msg_cdma_ok_to_send_sms
(
  void
)
{

  return ( msg_s_ptr->mc_msg.is_valid == FALSE );

} /* end of wms_msg_cdma_ok_to_send_sms() */

/*===========================================================================
FUNCTION   wms_msg_choose_bearer

DESCRIPTION
  Choose which bearer to use for sending messages

DEPENDENCIES
  None

RETURNS
  Bearer to use

SIDE EFFECTS
  Applicable for 1x Only. Donot use with GW.

===========================================================================*/
static wms_bearer_e_type wms_msg_choose_bearer
(
  wms_client_type_e_type client_id,
  boolean is_retry
)
{
  wms_bearer_e_type       chosen_bearer = WMS_BEARER_MAX;
  wms_client_s_type      *client_ptr = wms_client_ptr_fetch((wms_client_id_type)client_id);

  MSG_HIGH_1("Msg is undergoing retry: %d", is_retry);

  if (NULL == client_ptr)
  {
    MSG_ERROR_0("NULL client pointer");
    chosen_bearer = WMS_BEARER_CDMA_1X;
    cfg_s_ptr->last_used_bearer = chosen_bearer;
    return chosen_bearer;
  }

  MSG_HIGH_4( "In wms_msg_choose_bearer(): preferred_bearer = %d, last_used_bearer = %d, allowed_bearer = %d, client_id = %d ",
              client_ptr->preferred_bearer,
              client_ptr->last_used_bearer,
              client_ptr->allowed_bearer,
              client_id);

  /* If a bearer is preferred and it is active, use it */
  if( wms_client_active_bearer(client_ptr, client_ptr->preferred_bearer) )
  {
    chosen_bearer = client_ptr->preferred_bearer;
    MSG_HIGH_1("Using the active preferred bearer: %d", client_ptr->preferred_bearer);
  }
  /* If no preferred bearer is active, then check for last used bearer */
  else if( wms_client_active_bearer(client_ptr, client_ptr->last_used_bearer) )
  {
    chosen_bearer = client_ptr->last_used_bearer;
    MSG_HIGH_1("Using the last used bearer: %d", client_ptr->last_used_bearer);
  }
  /* If last used bearer not active, then check for allowed bearer */
  else if ( wms_client_active_bearer(client_ptr, client_ptr->allowed_bearer) )
  {
    chosen_bearer = client_ptr->allowed_bearer;
    MSG_HIGH_1("Using the allowed bearer: %d", client_ptr->allowed_bearer);
  }
  /* Pick a default if nothing is available */
  else
  {
#ifdef FEATURE_CDSMS
    if ( wms_client_active_bearer(client_ptr, WMS_BEARER_LTE) )
    {
      chosen_bearer = WMS_BEARER_LTE;
    }
    else
    {
      chosen_bearer = WMS_BEARER_CDMA_1X;
    }
    MSG_HIGH_1("No active bearer found. Default to %d bearer", chosen_bearer);
#else /* FEATURE_CDSMS */
      chosen_bearer = WMS_BEARER_GW_CS;
#endif /* FEATURE_CDSMS */
  }

  MSG_HIGH_1("Chosen bearer = %d", chosen_bearer);
  client_ptr->last_used_bearer = chosen_bearer;
  cfg_s_ptr->last_used_bearer = chosen_bearer;

  wms_client_ptr_release(client_ptr);

  return chosen_bearer;

} /* wms_msg_choose_bearer() */

/*===========================================================================
FUNCTION   wms_msg_have_active_bearer

DESCRIPTION
  Check if there is any active bearer to send SMS over

DEPENDENCIES
  None

RETURNS
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean wms_msg_have_active_bearer(void)
{
   if (cfg_s_ptr != NULL)
   {
      return (cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_1X] ||
              cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_EVDO] ||
              cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_WLAN] ||
              (cfg_s_ptr->active_bearers[WMS_BEARER_LTE] &&
               cfg_s_ptr->sms_over_s102));
   }
   else
   {
      return FALSE;
   }
}

boolean wms_is_cdma_admin_msg(const wms_OTA_message_type* OTA_ptr)
{
  boolean admin_msg = FALSE;
  wms_client_message_s_type  *cl_msg_ptr = 
    (wms_client_message_s_type *) wms_mem_malloc( sizeof(wms_client_message_s_type) );

  if (NULL != cl_msg_ptr)
  {
    if (NULL != OTA_ptr && WMS_FORMAT_CDMA == OTA_ptr->format)
    {
      if (WMS_OK_S ==  wms_ts_cdma_OTA2cl(OTA_ptr->data, OTA_ptr->data_len, cl_msg_ptr) &&
          (wms_cfg_check_wap_push_message( cl_msg_ptr ) ||
           WMS_TELESERVICE_CATPT == cl_msg_ptr->u.cdma_message.teleservice))
      {
        MSG_HIGH_0("wms_is_cdma_admin_msg() returns TRUE");
        admin_msg = TRUE;
      }
    }

    wms_mem_free( cl_msg_ptr );
  }

  return admin_msg;
}

/*===========================================================================
FUNCTION   wms_msg_cdma_send_sms_OTA

DESCRIPTION
  Send a raw SMS message to the MC task and update the local MC message info.

DEPENDENCIES
  None

RETURNS
  TRUE:   success
  FALSE:  failure

SIDE EFFECTS
  Update the local MC msg info buffer and reset the DTC idle timer.

===========================================================================*/
wms_status_e_type wms_msg_cdma_send_sms_OTA
(
  boolean                            is_ack,
  wms_msg_cdma_message_record_type   * rec_ptr,
  const wms_OTA_message_type         * OTA_ptr,
  const wms_address_s_type           * address_ptr
)
{
  wms_status_e_type  st = WMS_OK_S;
  wms_bearer_e_type  bearer;
  const wms_OTA_message_type *ota_ptr = OTA_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rec_ptr == NULL || OTA_ptr == NULL || address_ptr == NULL)
  {
    MSG_HIGH_0("Null pointer: wms_msg_send_CDMA_OTA");
    st = WMS_NULL_PTR_S;
    return st;
  }

  MSG_HIGH_2( "Sending SMS OTA: len=%d, fmt=%d",
              OTA_ptr->data_len,
              OTA_ptr->format );

  if( ! wms_msg_cdma_ok_to_send_sms() )
  {
    MSG_HIGH_3( "Send OTA: MC is not ready. rec = 0x%x, seg_num = %d, is_ack = %d",
                rec_ptr, rec_ptr->seq_num, is_ack);
    st = WMS_NETWORK_NOT_READY_S;
  }
  else
  {
    msg_s_ptr->mc_msg.is_ack  = is_ack;
    msg_s_ptr->mc_msg.rec_ptr = rec_ptr;

    MSG_HIGH_1("Record transport type: %d", rec_ptr->transport_type);

    if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == rec_ptr->transport_type)
    {
      sys_modem_as_id_e_type    ims_as_id     = wms_get_appinfo_1x_asid();
      wms_msg_transport_s_type* transport_ptr = wms_msg_choose_registered_transport( ims_as_id );
      wms_cm_s_type             *cm_s_ptr     = wms_cm_s_ptr();

      MSG_HIGH_3("call info: asubs_id = %d, type = %d, state = %d", 
                  cm_s_ptr->call_info.asubs_id,
                  cm_s_ptr->call_info.call_type,
                  cm_s_ptr->call_info.call_state ); 

      // Has transport registered?
      if (NULL == transport_ptr)
      {
        MSG_ERROR_0("Transport layer has not yet registered!");
        return WMS_TRANSPORT_NOT_READY_S;
      }
      else if ( (CM_CALL_TYPE_VOICE == cm_s_ptr->call_info.call_type)  &&
                (CM_CALL_STATE_IDLE != cm_s_ptr->call_info.call_state) &&  
                (cm_s_ptr->call_info.asubs_id == ims_as_id)    &&
                wms_cfg_check_1xsrlte_or_ehrpd_oper_mode(ims_as_id) )
      {
        MSG_ERROR_0("Can't send SMS over IMS while in 1xSRLTE mode");
        return WMS_IMS_NOT_ALLOWED_IN_SRLTE_S;
      }
      // Does transport allow admin sms only?
      else if( (WMS_MSG_TRANSPORT_NW_REG_STATUS_LIMITED_SRV == transport_ptr->nw_reg_status ||
                cfg_s_ptr->admin_sms_only) && !wms_is_cdma_admin_msg(OTA_ptr))
      {
        MSG_ERROR_0("Can't send non-admin message while IMS in limited service");
        return WMS_TRANSPORT_NOT_ALLOWED_S;
      }
      else
      {
         wms_ota_msg_s_type ota_msg;
         ota_msg.format = OTA_ptr->format;
         ota_msg.address = *address_ptr;
         MSG_HIGH_3( "Send CDMA digit_mode = %d, number_type = %d, number_plan = %d",
                     ota_msg.address.digit_mode,
                     ota_msg.address.number_type,
                     ota_msg.address.number_plan );
         ota_msg.data_len = MIN(OTA_ptr->data_len, WMS_MAX_LEN);
         (void)memscpy( (char *)ota_msg.data,
                        ota_msg.data_len,
                        (char *)OTA_ptr->data,
                        ota_msg.data_len );
         msg_s_ptr->mc_msg.bearer = WMS_BEARER_IMS_3GPP2;
         msg_s_ptr->mc_msg.is_valid = TRUE;
         (void) transport_ptr->mo_sms_cb( transport_ptr->as_id, &ota_msg );
         return WMS_OK_S;
      }
    }

    if (dc_s_ptr->is_voice_data_call_conn && (cfg_s_ptr->simul_sms_voice_over_1x_allowed == FALSE))
    {
      MSG_HIGH_0("Can't send SMS message during active 1x voice call");
      return WMS_1X_AND_VOICE_NOT_ALLOWED_S;
    }
    else
    {
      if ( wms_msg_have_active_bearer() )
      {
        wms_bearer_e_type last_used_bearer = cfg_s_ptr->last_used_bearer;
        boolean is_retry = (rec_ptr->retry_time_left != (sint31)msg_s_ptr->mo_retry_period);

        if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102 == rec_ptr->transport_type)
        {
          bearer = WMS_BEARER_LTE;	  
        }
        else
        {
          bearer = wms_msg_choose_bearer(rec_ptr->client_id, is_retry);
        }

        if( WMS_BEARER_CDMA_1X == bearer )
        {
          rec_ptr->transport_type = WMS_MSG_TRANSPORT_TYPE_CS;

          /* In case SMS is first sent over DO, and it fails, it gets retransmitted over 1X, when re-sent
            over 1X TL Ack needs to be requested, and bearer reply option needs to be added to the MO SMS.*/
          if( (WMS_BEARER_CDMA_EVDO == last_used_bearer) && is_retry && !is_ack )
          {
            st = wms_ts_decode_CDMA_tl( ota_ptr,
                                        &cdma_tl,
                                        &cdma_raw_bd);

            if (WMS_OK_S == st && 0 == ( cdma_tl.mask & WMS_MASK_TL_BEARER_REPLY_OPTION ) )
            {
              MSG_HIGH_0("wms_msg_cdma_send_sms_OTA:Retrying sending SMS with BRO set (TL Ack requested) ");

              cdma_tl.mask |= WMS_MASK_TL_BEARER_REPLY_OPTION;
              cdma_tl.bearer_reply_seq_num = (uint8)wms_msg_cdma_next_seq_num();
              st = wms_msg_get_next_unused_seq_num( cdma_tl.bearer_reply_seq_num,
                                                      &cdma_tl.bearer_reply_seq_num );
              if( WMS_OK_S == st )
              {
                st = wms_ts_encode_CDMA_tl( & cdma_tl,
                                            & cdma_raw_bd,
                                            rec_ptr->ota );
              }
              if( WMS_OK_S == st )
              {
                ota_ptr = rec_ptr->ota;
                rec_ptr->seq_num = cdma_tl.bearer_reply_seq_num;
              }
            }
          }

          /* reset the idle timer if auto disconnect is enabled
          */
          if( WMS_OK_S == st && TRUE == dc_s_ptr->auto_disc_active )
          {
            (void) rex_set_timer( & dc_s_ptr->idle_timer, dc_s_ptr->idle_timeout );
          }
        }

        if ( (WMS_OK_S == st) &&
             wms_msg_suspend_ps_call_is_supported() )
        {
          if (!is_retry)
          {
            cm_call_cmd_err_e_type suspend_status = CM_CALL_CMD_ERR_NOERR;

            suspend_status = wms_cm_suspend_pscall(wms_get_appinfo_1x_asid());

            if (CM_CALL_CMD_ERR_NOERR != suspend_status)
            {
              st = WMS_NETWORK_NOT_READY_S;
            }
          }
        }

        if (WMS_OK_S == st)
        {
          msg_s_ptr->mc_msg.bearer = bearer;

          // Send SMS to CM:
          msg_s_ptr->mc_msg.is_valid = TRUE;
          MSG_HIGH_2( "SMS sent over bearer = %d, ch_pref = %d",
                      bearer,
                      rec_ptr->ch_pref );
          wms_cm_sms_send_cdma_msg2( address_ptr, OTA_ptr, bearer, rec_ptr->ch_pref );
        }
      }
      else
      {
        st = WMS_PHONE_NOT_READY_S;
      }
    }
  }

  return st;
} /* end of wms_msg_cdma_send_sms_OTA() */


/*===========================================================================
FUNCTION   wms_msg_cdma_map_status

DESCRIPTION
  Map the status code to the code that is to be transferred in the encoded
  message.

DEPENDENCIES
  None

RETURNS
  error class and TL status.

SIDE EFFECTS
  None
===========================================================================*/
void wms_msg_cdma_map_status
(
  wms_status_e_type       st, /* IN */
  wms_error_class_e_type  * error_class_ptr, /* OUT */
  wms_cdma_tl_status_e_type    * tl_status_ptr   /* OUT */
)
{

  if(error_class_ptr == NULL || tl_status_ptr == NULL )
  {
    MSG_HIGH_0("Null pointer: wms_msg_send_CDMA_OTA");
    return;
  }

  switch( st )
  {
    case WMS_OK_S:
      *error_class_ptr = WMS_ERROR_NONE;
      break;

    case WMS_OUT_OF_RESOURCES_S:
      *error_class_ptr = WMS_ERROR_TEMP;
      *tl_status_ptr = WMS_TL_DESTINATION_RESOURCE_SHORTAGE_S;
      break;

    case WMS_INVALID_TRANSACTION_ID_S:
    case WMS_INVALID_FORMAT_S:
      *error_class_ptr = WMS_ERROR_PERM;
      *tl_status_ptr = WMS_TL_OTHER_TERMINAL_PROBLEM_S;
      break;

    case WMS_INVALID_PARM_SIZE_S:
      *error_class_ptr = WMS_ERROR_PERM;
      *tl_status_ptr = WMS_TL_UNEXPECTED_PARM_SIZE_S;
      break;

    case WMS_INVALID_USER_DATA_SIZE_S:
      *error_class_ptr = WMS_ERROR_PERM;
      *tl_status_ptr = WMS_TL_USER_DATA_SIZE_ERROR_S;
      break;

    case WMS_INVALID_PARM_VALUE_S:
      *error_class_ptr = WMS_ERROR_PERM;
      *tl_status_ptr = WMS_TL_UNRECOGNIZED_PARM_VALUE_S;
      break;

    case WMS_MISSING_PARM_S:
      *error_class_ptr = WMS_ERROR_PERM;
      *tl_status_ptr = WMS_TL_MISSING_MANDATORY_PARM_S;
      break;

    case WMS_NETWORK_NOT_READY_S:
      *error_class_ptr = WMS_ERROR_TEMP;
      *tl_status_ptr = WMS_TL_NETWORK_FAILURE_S;
      break;

    case WMS_TERMINAL_BLOCKED_S:
      *error_class_ptr = WMS_ERROR_TEMP;
      *tl_status_ptr = WMS_TL_SMS_TERMINATION_DENIED_S;
      break;

    case WMS_TERMINAL_BUSY_S:
      *error_class_ptr = WMS_ERROR_TEMP;
      *tl_status_ptr = WMS_TL_DESTINATION_BUSY_S;
      break;

    case WMS_GENERAL_ERROR_S:
    default:
      *error_class_ptr = WMS_ERROR_TEMP;
      *tl_status_ptr = WMS_TL_OTHER_GENERAL_PROBLEMS_S;
      break;
  }

  /* done */
  return;

} /* end of wms_msg_cdma_map_status() */


/*
*/
wms_status_e_type wms_msg_map_cmd_err_to_status
(
  wms_cmd_err_e_type   cmd_err
)
{
  wms_status_e_type  st;
  switch( cmd_err )
  {
    case WMS_CMD_ERR_NONE:
      st = WMS_OK_S;
      break;

    case WMS_CMD_ERR_NO_RESOURCE:
      st = WMS_OUT_OF_RESOURCES_S;
      break;

    case WMS_CMD_ERR_MSG_MEMORY_FULL:
      st = WMS_OUT_OF_RESOURCES_S;
      break;

    case WMS_CMD_ERR_MSG_TRANSACTION_ID:
      st = WMS_INVALID_TRANSACTION_ID_S;
      break;

    case WMS_CMD_ERR_MSG_TPDU_TYPE:
      st = WMS_INVALID_TPDU_TYPE_S;
      break;

    case WMS_CMD_ERR_MSG_FORMAT:
      st = WMS_INVALID_FORMAT_S;
      break;

    case WMS_CMD_ERR_CLIENT_ID:

    case WMS_CMD_ERR_CFG_NV_WRITE:
    case WMS_CMD_ERR_CFG_NV_READ:
    case WMS_CMD_ERR_CFG_ROUTE:
    case WMS_CMD_ERR_CFG_MEM_STORE:
    case WMS_CMD_ERR_CFG_MSG_CLASS:

    case WMS_CMD_ERR_MSG_MEMORY_STORE:
    case WMS_CMD_ERR_MSG_INDEX:
    case WMS_CMD_ERR_MSG_EMPTY_MESSAGE:
    case WMS_CMD_ERR_MSG_TAG:
    case WMS_CMD_ERR_MSG_ENCODE:
    case WMS_CMD_ERR_MSG_DECODE:
    case WMS_CMD_ERR_MSG_SEND_MODE:
    case WMS_CMD_ERR_MSG_WRITE_MODE:

    case WMS_CMD_ERR_MSG_SIM_WRITE:
    case WMS_CMD_ERR_MSG_SIM_READ:
    case WMS_CMD_ERR_MSG_SIM_DELETE:
    case WMS_CMD_ERR_MSG_RUIM_WRITE:
    case WMS_CMD_ERR_MSG_RUIM_READ:
    case WMS_CMD_ERR_MSG_RUIM_DELETE:
    case WMS_CMD_ERR_MSG_NV_WRITE:
    case WMS_CMD_ERR_MSG_NV_READ:
    case WMS_CMD_ERR_MSG_NV_DELETE:

    case WMS_CMD_ERR_BC_BAD_PREF:
    case WMS_CMD_ERR_BC_CANNOT_ACTIVATE:
    case WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL:
    case WMS_CMD_ERR_BC_NV_WRITE:
    case WMS_CMD_ERR_BC_BAD_SRV_ID:
    case WMS_CMD_ERR_BC_TABLE_FULL:
    case WMS_CMD_ERR_BC_DUPLICATE_SRV:

    case WMS_CMD_ERR_DC_BAD_STATE:
    case WMS_CMD_ERR_DC_BAD_CALL_ID:
    default:
      st = WMS_GENERAL_ERROR_S;
      break;
  }

  return st;

} /* wms_msg_map_cmd_err_to_status() */

/*===========================================================================
FUNCTION   wms_msg_get_next_unused_seq_num

DESCRIPTION
  Search the next unused sequence number starting with specified number.
  Note that the sequence number returned by wms_msg_cdma_next_seq_num()
  might be used already in the case when sending a large number of
  messages that causes the sequence number to wrap around.

DEPENDENCIES
  None

RETURNS
  wms_status_e_type

SIDE EFFECTS
  None
===========================================================================*/
static wms_status_e_type wms_msg_get_next_unused_seq_num
(
  uint8  start_seq_num,
  uint8* unused_seq_num_ptr
)
{

  /* find the next available seq_num */
  uint8               seq_num = start_seq_num;
  boolean             done_searching = FALSE;
  boolean             record_found = FALSE;
  wms_status_e_type   st = WMS_OK_S;
  wms_msg_cdma_message_record_type  *rec_ptr = NULL;

  if (NULL == unused_seq_num_ptr)
  {
    return WMS_NULL_PTR_S;
  }

  do
  {
    /* Search for record using seq_num */
    rec_ptr = wms_msg_cdma_map_mo_rec( seq_num );

    if (NULL == rec_ptr)
    {
      /* No record found, seq_num not in use.
       * The search for unused seq_num is done. */
      done_searching = TRUE;

      /* A free MO record must also exist before returning successfully */
      if ( wms_msg_cdma_have_mo_rec() )
      {
         /* Free MO record exists, use this seq_num */
         MSG_HIGH_1("free MO record available!, MO seq = %d", seq_num);
         *unused_seq_num_ptr = seq_num;
         record_found  = TRUE;
      }
      else
      {
         MSG_HIGH_0("No free MO record available!");
      }
    }
    else /* NULL != rec_ptr */
    {
       /* Found a record using the seq_num: seq_num in use */

       /* Try the next seq_num */
       seq_num = (seq_num + 1) % WMS_TL_SEQ_NUM_MAX;

       if (seq_num == start_seq_num)
       {
         /* The search has looped back to the starting point. Searching is done. */
         MSG_HIGH_0("All seq_num in use!");
         done_searching = TRUE;
       }
    }
  } while (!done_searching);

  if (!record_found)
  {
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
}

/*===========================================================================
FUNCTION   wms_msg_cdma_ack_msg_i

DESCRIPTION
  Send an ack message corresponding to the MT record.

DEPENDENCIES
  None

RETURNS
  Status of the ack.

SIDE EFFECTS
  Update the MT record's state.

===========================================================================*/
wms_status_e_type  wms_msg_cdma_ack_msg_i
(
  wms_msg_cdma_message_record_type       * rec_ptr,
  wms_error_class_e_type                   error_class,
  wms_cdma_tl_status_e_type                tl_status
)
{
  byte                           seq_num;
  wms_status_e_type              st = WMS_OK_S;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( rec_ptr == NULL )
  {
    MSG_ERROR_0("Null rec_ptr!");
    return WMS_NULL_PTR_S;  /* SHORT-RETURN */
  }

  MSG_HIGH_2( "cdma_ack_msg_i: seq_n=%d, tid=0x%lx",
              rec_ptr->seq_num,
              rec_ptr->user_data );

  seq_num = rec_ptr->seq_num;

  if( rec_ptr->state == WMS_MSG_STATE_IDLE )
  {
    MSG_HIGH_1( "ack msg: seq %d not active!", seq_num);
    st = WMS_INVALID_TRANSACTION_ID_S;
  }
  else if( WMS_CLIENT_CH_NONE == rec_ptr->ch_pref )
  {
    MSG_HIGH_1( "Unsupported channel perefrence, ch_pref = %d", rec_ptr->ch_pref);
    st = WMS_MO_PREF_CHN_UNSUPPORTED_S;
  }
  else
  {
    /* fill in TL msg structure
    */
    cdma_tl.tl_message_type          = WMS_TL_TYPE_ACK;
    cdma_tl.mask                     = WMS_MASK_TL_CAUSE_CODES;
    cdma_tl.cause_code.reply_seq_num = seq_num;
    cdma_tl.cause_code.error_class   = error_class;
    cdma_tl.cause_code.tl_status     = tl_status;

    // address should always be present
    cdma_tl.mask |= WMS_MASK_TL_ADDRESS;
    cdma_tl.address = rec_ptr->address;

    if( rec_ptr->subaddress.number_of_digits != 0 )
    {
      cdma_tl.mask |= WMS_MASK_TL_SUBADDRESS;
      cdma_tl.subaddress = rec_ptr->subaddress;
    }

    /* encode msg without BD data
    */
    cdma_tl.is_mo = TRUE;
    st = wms_ts_encode_CDMA_tl( & cdma_tl,
                                NULL,  /* No BD */
                                & ruim_sms.ota );

    if( st == WMS_OK_S )
    {
      msg_s_ptr->mc_msg.tid         = (wms_transaction_id_type) rec_ptr->user_data;
      msg_s_ptr->mc_msg.error_class = error_class;
      msg_s_ptr->mc_msg.tl_status   = tl_status;

      st = wms_msg_cdma_send_sms_OTA( TRUE,   /* is a TL ack */
                                      rec_ptr,
                                      & ruim_sms.ota,
                                      & cdma_tl.address );
    }

  } /* if state==IDLE - else */

  /* done */
  return st;

} /* end of wms_msg_cdma_ack_msg_i() */

/*===========================================================================
FUNCTION wms_msg_cdma_send_MO

DESCRIPTION
   It stores MO SMS into CDMA REC and sends MO SMS request to lower layer

RETURN VALUE
   wms_cmd_err_e_type

===========================================================================*/
wms_cmd_err_e_type wms_msg_cdma_send_MO
(
  boolean                          send_status_to_clients,
  wms_cmd_type                    *cmd_ptr,
  wms_send_mode_e_type             send_mode,
  const wms_client_msg_hdr_s_type *msg_hdr_ptr,
  const wms_tl_message_type       *tl_ptr,
  const wms_OTA_message_type      *ota_ptr,
  wms_msg_transport_type_e_type    transport_type
)
{
  wms_msg_cdma_message_record_type  *rec_ptr = NULL;
  wms_cmd_err_e_type                cmd_err = WMS_CMD_ERR_NONE;
  wms_status_e_type                 st;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_HIGH_0("wms_msg_cdma_send_MO()");

  if( (NULL == cmd_ptr) || (NULL == msg_hdr_ptr) || (NULL == tl_ptr) || (NULL == ota_ptr) )
  {
    MSG_HIGH_0("Null pointer: wms_msg_send_CDMA_MO");
    return WMS_CMD_ERR_NULL_PTR;
  }

  /* --------------------------------------------------------------- */
  /* ----------------- acquire a free msg record ------------------- */
  /* --------------------------------------------------------------- */
  rec_ptr = wms_msg_cdma_get_mo_rec();

  if( NULL == rec_ptr )
  {
    cmd_err = WMS_CMD_ERR_NO_RESOURCE;
  }
  else
  {
    rec_ptr->ota = (wms_OTA_message_type*)wms_mem_malloc(sizeof(wms_OTA_message_type));
    if ( NULL != rec_ptr->ota )
    {
      (void)memscpy( (void *)rec_ptr->ota,
                     sizeof(wms_OTA_message_type),
                     (void *)ota_ptr,
                     sizeof(wms_OTA_message_type));

      if( !(tl_ptr->mask & WMS_MASK_TL_BEARER_REPLY_OPTION) )
      {
        /* ---------------- TL ack is NOT requested ------------- */
        MSG_HIGH_0("snd msg: No ACK requested");
      }
      else
      {
        /* ---------------- TL ack is requested ------------- */
        MSG_HIGH_0("MO msg tl_ack requested");
      }

      rec_ptr->seq_num = tl_ptr->bearer_reply_seq_num;
      rec_ptr->send_status_to_clients = send_status_to_clients;
      rec_ptr->user_data    = cmd_ptr->hdr.user_data;
      rec_ptr->appInfo_ptr  = cmd_ptr->hdr.appInfo_ptr;
      rec_ptr->msg_hdr      = *msg_hdr_ptr;
      rec_ptr->send_mode    = send_mode;
      rec_ptr->client_id    = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
      rec_ptr->ch_pref      = cmd_ptr->cmd.msg_send.ch_pref;

      rec_ptr->address      = tl_ptr->address;
      rec_ptr->subaddress   = tl_ptr->subaddress;
      /* If retry logic is disabled, set the retry_time_left to the default value
         to prevent WMS from waiting forever if mo status(L2 ack) is not received.
      */
      if ( 0 == msg_s_ptr->mo_retry_period )
      {
        rec_ptr->retry_time_left = WMS_MAX_MO_STATUS_WAIT_TIME;
      }
      else
      {
        rec_ptr->retry_time_left = msg_s_ptr->mo_retry_period;
      }

      if (cmd_ptr->hdr.cmd_id == WMS_CMD_MSG_SEND)
      {
        rec_ptr->resend_msg    = cmd_ptr->cmd.msg_send.resend_msg;
        rec_ptr->client_msg_id = cmd_ptr->cmd.msg_send.client_msg_id;
      }

      /* Initialize the Prev Report to Timer Expired Status */
      rec_ptr->prev_report.user_data = rec_ptr->user_data;
      rec_ptr->prev_report.client_id = rec_ptr->client_id;
      rec_ptr->prev_report.report_status = WMS_RPT_MO_RETRY_PERIOD_EXPIRED;
      rec_ptr->prev_report.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
      rec_ptr->prev_report.alpha_id.len = 0;
      rec_ptr->prev_report.alpha_id.data = NULL;
      rec_ptr->prev_report.message_mode = WMS_MESSAGE_MODE_CDMA;
      rec_ptr->transport_type = transport_type;

      /* --------------------------------------------------------------- */
      /* ----------------- Now we are ready to send the message -------- */
      /* --------------------------------------------------------------- */
      st = wms_msg_cdma_send_sms_OTA( FALSE,  /* not a TL Ack msg */
                                      rec_ptr,
                                      ota_ptr,
                                      & tl_ptr->address );

      if( st == WMS_OK_S )
      {
        MSG_HIGH_0("MO msg State: L2 in Progress");
        rec_ptr->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;

        if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == rec_ptr->transport_type ||
          WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102 == rec_ptr->transport_type)
        {        
          wms_prov_app_info_s_type *appInfo_ptr = NULL;

          appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

          if (NULL != appInfo_ptr)
          {
            if (0 == appInfo_ptr->mo_sms_count)
            {
              wms_mm_sms_call_info_T mm_status;

              memset(&mm_status, 0x00, sizeof(mm_status));

              mm_status.as_id = SYS_MODEM_AS_ID_1;

              if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == rec_ptr->transport_type) 
              {
                mm_status.sms_over_ip_status = TRUE;
              }
              else
              {
                mm_status.sms_over_s102 = TRUE;
              }

              (void)wms_cm_gw_send_sms_status_to_mm(mm_status);
            }
            appInfo_ptr->mo_sms_count++; 
          }            		
        }

        /* Wait for the remaining of RETRY Period to hear back from CM */
        rec_ptr->retry_wait_time = 0;
        wms_set_retry_timer();
      }
      else if( (WMS_NETWORK_NOT_READY_S == st) || (WMS_PHONE_NOT_READY_S == st) )
      {
        rec_ptr->state = WMS_MSG_STATE_QUEUED;

        /* retry again in 1 second */
        rec_ptr->retry_wait_time = (sint31)1;
        MSG_HIGH_1("L2/NW is busy. Try again in %d secs", rec_ptr->retry_wait_time);
        wms_set_retry_timer();
      }
      else if ( WMS_IMS_NOT_ALLOWED_IN_SRLTE_S == st ) 
      {
        /* Faking WMS_MSG_STATE_QUEUED */  
        rec_ptr->state = WMS_MSG_STATE_QUEUED;
        rec_ptr->retry_time_left = (sint31)1;
        rec_ptr->retry_wait_time = (sint31)0;
        rec_ptr->prev_report.report_status = WMS_RPT_IMS_NOT_ALLOWED_IN_SRLTE;
        wms_set_retry_timer();
      }
      else if ( WMS_TRANSPORT_NOT_READY_S == st )
      {
        cmd_err = WMS_CMD_ERR_TRANSPORT_NOT_READY;
      }
      else if ( WMS_TRANSPORT_NOT_ALLOWED_S == st )
      {
        cmd_err = WMS_CMD_ERR_TRANSPORT_NOT_ALLOWED;
      }
      else if ( WMS_1X_AND_VOICE_NOT_ALLOWED_S == st )
      {
        cmd_err = WMS_CMD_ERR_1X_AND_VOICE_NOT_ALLOWED;
      }
      else
      {
        /* Declare Failure */
        cmd_err = WMS_CMD_ERR_NULL_PTR;
      }
    }
    else
    {
      cmd_err = WMS_CMD_ERR_NO_RESOURCE;
    }

    if ( WMS_CMD_ERR_NONE != cmd_err )
    {
      (void)wms_msg_cdma_get_next_mo_rec_before_delete( rec_ptr );
    }
  }

  return cmd_err;

} /* wms_msg_cdma_send_MO() */

/*===========================================================================
FUNCTION wms_msg_send_3gpp2_sms_over_ims

DESCRIPTION
   Check if 3GPP2 SMS can be sent over IMS

RETURN VALUE
   boolean

===========================================================================*/
boolean wms_msg_send_3gpp2_sms_over_ims(const wms_OTA_message_type* OTA_ptr)
{
   boolean send_to_ims = FALSE;

   wms_msg_transport_s_type* transport_ptr = wms_msg_choose_registered_transport( wms_get_appinfo_1x_asid() );
   if (transport_ptr != NULL)
   {
      send_to_ims = TRUE;
      if( (WMS_MSG_TRANSPORT_NW_REG_STATUS_LIMITED_SRV == transport_ptr->nw_reg_status ||
          cfg_s_ptr->admin_sms_only) && !wms_is_cdma_admin_msg(OTA_ptr) )
      {
        // If 1x/LTE is available, send the message to 1x/S102
        if ( wms_msg_have_active_bearer() )
        {
           send_to_ims = FALSE;
        }
      }
   }

   MSG_HIGH_1("wms_msg_send_3gpp2_sms_over_ims returns %d", send_to_ims);
   return send_to_ims;
}

/*===========================================================================
FUNCTION wms_msg_cdma_send_mo_channel

DESCRIPTION
   It is wrapper around wms_msg_cdma_send_MO fuction to decide either send
   CDMA MO SMS over Access Channel or Traffic Channel

RETURN VALUE
   wms_cmd_err_e_type

===========================================================================*/
wms_cmd_err_e_type wms_msg_cdma_send_mo_channel
(
  boolean                          send_status_to_clients,
  wms_cmd_type                     *cmd_ptr,
  wms_send_mode_e_type             send_mode,
  const wms_client_msg_hdr_s_type  *msg_hdr_ptr,
  const wms_tl_message_type        *tl_ptr,
  const wms_OTA_message_type       *ota_ptr
)
{
   wms_cmd_err_e_type cmd_err = WMS_CMD_ERR_NONE;

   if ( (NULL == cmd_ptr) || (NULL == ota_ptr) || (NULL == tl_ptr) || (NULL == msg_hdr_ptr) )
   {
      MSG_ERROR_0("In wms_msg_cdma_send_mo_channel(): Null pointer");
      return  WMS_CMD_ERR_NULL_PTR;
   }

   MSG_HIGH_1("wms_msg_cdma_send_mo_channel(): AP transport = %d",
              cmd_ptr->cmd.msg_send.transport_type);

   if (WMS_MSG_TRANSPORT_TYPE_INTERNAL == cmd_ptr->cmd.msg_send.transport_type)
   {
      if (wms_msg_send_3gpp2_sms_over_ims(ota_ptr))
      {
         cmd_ptr->cmd.msg_send.transport_type = WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS;
      }
      else
      {
         cmd_ptr->cmd.msg_send.transport_type = WMS_MSG_TRANSPORT_TYPE_CS;
      }
   }

   MSG_HIGH_3( "Chosen transport = %d, client_id = %d, ch_pref = %d",
                cmd_ptr->cmd.msg_send.transport_type,
                cmd_ptr->hdr.client_id,
                cmd_ptr->cmd.msg_send.ch_pref );

   // Allow MO SMS over IMS even in LPM mode.
   if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS != cmd_ptr->cmd.msg_send.transport_type &&
       cfg_s_ptr->lpm)
    {   
      return WMS_CMD_ERR_MSG_LPM;
    }

   /* Special handling for hVoLTE mode. */
   if ( ( WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS != cmd_ptr->cmd.msg_send.transport_type ) && 
          wms_cfg_check_hvolte_oper_mode() )
   {
      /* Reject non-IMS MO SMS if device is in HVoLTE mode and VoLTE call is active on-going on same sub, since
         1X bearer won't be available. */
      if ( ( ( NULL != cfg_s_ptr ) && ( !cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_1X] ) ) &&
               wms_cm_check_active_volte_call( SYS_MODEM_AS_ID_1, SYS_SYS_MODE_LTE ) )
      {
        MSG_ERROR_0("Can't send SMS over 1X during an active VoLTE call in hVoLTE mode");
        return WMS_CMD_ERR_TRANSPORT_NOT_READY;
      }

      /* Reject DAN SMS attempt from IMS - since IMS request to send DAN SMS on traffic channel will cause CM to
         abort LTE and move to 1X. */
      if ( ( WMS_CLIENT_CH_PREF_TCH_ONLY             == cmd_ptr->cmd.msg_send.ch_pref ) &&
           ( (wms_client_id_type)WMS_CLIENT_TYPE_IMS == cmd_ptr->hdr.client_id ) )
      {
        MSG_ERROR_0("Can't send DAN SMS over 1X in hVoLTE mode");
        return WMS_CMD_ERR_TRANSPORT_NOT_READY;
      }
   }

   if ( WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == cmd_ptr->cmd.msg_send.transport_type )
   {
      MSG_HIGH_0("Send 3GPP2 SMS over IMS");
      cmd_err = wms_msg_cdma_send_MO(
                        send_status_to_clients,
                        cmd_ptr,
                        send_mode,
                        msg_hdr_ptr,
                        tl_ptr,
                        ota_ptr,
                        WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS);
   }
   else if ( NULL != cfg_s_ptr &&
             cfg_s_ptr->active_bearers[WMS_BEARER_LTE] && 
             cfg_s_ptr->sms_over_s102 &&
             (cfg_s_ptr->lte_sub_info == SYS_MODEM_AS_ID_1))

   {
     MSG_HIGH_0("Send 3GPP2 SMS over S102 tunnel");
     cmd_err = wms_msg_cdma_send_MO(
                         send_status_to_clients,
                         cmd_ptr,
                         send_mode,
                         msg_hdr_ptr,
                         tl_ptr,
                         ota_ptr,
                         WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102);
   }
   else if ( WMS_CLIENT_CH_PREF_ACH_ONLY == cmd_ptr->cmd.msg_send.ch_pref )
   {
     MSG_HIGH_0("Send 3GPP2 SMS over ACH");
     cmd_err = wms_msg_cdma_send_MO(
                       send_status_to_clients,
                       cmd_ptr,
                       send_mode,
                       msg_hdr_ptr,
                       tl_ptr,
                       ota_ptr,
                       WMS_MSG_TRANSPORT_TYPE_CS);
   }
   else if ( WMS_CLIENT_CH_PREF_ACH_OR_TCH == cmd_ptr->cmd.msg_send.ch_pref )
   {
     MSG_HIGH_0("Send 3GPP2 SMS over ACH/TCH");
     cmd_err = wms_msg_cdma_send_MO(
                       send_status_to_clients,
                       cmd_ptr,
                       send_mode,
                       msg_hdr_ptr,
                       tl_ptr,
                       ota_ptr,
                       WMS_MSG_TRANSPORT_TYPE_CS);
   }
   else if ( WMS_CLIENT_CH_PREF_TCH_ONLY == cmd_ptr->cmd.msg_send.ch_pref )
   {
     wms_status_e_type st = WMS_OK_S;

     MSG_HIGH_0("Send 3GPP2 SMS over TCH");
     /* Send on Traffic Channel */
     /* Request to set up if call is not already active */
     if ( (!dc_s_ptr->call_active ) && (!dc_s_ptr->is_voice_data_call_conn) )
     {
       if (!dc_s_ptr->wms_initiated_call && WMS_DUMMY_CALL_ID == dc_s_ptr->dc_call_id)
       {
         MSG_HIGH_0("Originating SMS call. Waiting for the call to connect...");
         st = wms_dc_connect( (wms_client_id_type) WMS_CLIENT_TYPE_INTERNAL,
                              NULL,
                              NULL,
                              cmd_ptr->cmd.msg_send.dc_so);
         if (WMS_OK_S == st)
         {
           dc_s_ptr->wms_initiated_call = TRUE;
         }
       }
       else
       {
         MSG_HIGH_0("SMS call has been originated. Waiting for the call to connect...");
       }

       if ( WMS_OK_S == st )
       {
         wms_msg_cdma_message_record_type  *rec_ptr = NULL;
         /* --------------------------------------------------------------- */
         /* ----------------- acquire a free msg record ------------------- */
         /* --------------------------------------------------------------- */
         rec_ptr = wms_msg_cdma_get_mo_rec();

         if( NULL == rec_ptr )
         {
            cmd_err = WMS_CMD_ERR_NO_RESOURCE;
         }
         else
         {
           rec_ptr->ota = (wms_OTA_message_type*)wms_mem_malloc(sizeof(wms_OTA_message_type));
           if ( NULL != rec_ptr->ota )
           {
             (void)memscpy( (void *)rec_ptr->ota,
                             sizeof(wms_OTA_message_type),
                             (void *)ota_ptr,
                             sizeof(wms_OTA_message_type));


             /* Save the message in REC */
             rec_ptr->seq_num = tl_ptr->bearer_reply_seq_num;
             rec_ptr->send_status_to_clients = send_status_to_clients;
             rec_ptr->user_data    = cmd_ptr->hdr.user_data;
             rec_ptr->appInfo_ptr  = cmd_ptr->hdr.appInfo_ptr;
             rec_ptr->msg_hdr      = *msg_hdr_ptr;
             rec_ptr->send_mode    = send_mode;
             rec_ptr->client_id    = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
             rec_ptr->ch_pref      = cmd_ptr->cmd.msg_send.ch_pref;
             rec_ptr->dc_so        = cmd_ptr->cmd.msg_send.dc_so;
             rec_ptr->follow_on_dc = cmd_ptr->cmd.msg_send.follow_on_dc; 
             rec_ptr->address      = tl_ptr->address;
             rec_ptr->subaddress   = tl_ptr->subaddress;
             rec_ptr->state        = WMS_MSG_STATE_QUEUED;
             rec_ptr->transport_type = WMS_MSG_TRANSPORT_TYPE_CS;

             /* Wait for the remaining of RETRY Period to hear back from CM */
             rec_ptr->retry_wait_time = (sint31)0;

             /* If retry logic is disabled, set the retry_time_left to the default value
                to prevent WMS from waiting forever if DC failed to connect.
             */
             if ( 0 == msg_s_ptr->mo_retry_period )
             {
               rec_ptr->retry_time_left = WMS_MAX_MO_STATUS_WAIT_TIME;
             }
             else
             {
               rec_ptr->retry_time_left = msg_s_ptr->mo_retry_period;
             }

             if (cmd_ptr->hdr.cmd_id == WMS_CMD_MSG_SEND)
             {
               rec_ptr->resend_msg    = cmd_ptr->cmd.msg_send.resend_msg;
               rec_ptr->client_msg_id = cmd_ptr->cmd.msg_send.client_msg_id;
             }

             /* Initialize the Prev Report to Timer Expired Status */
             rec_ptr->prev_report.user_data = rec_ptr->user_data;
             rec_ptr->prev_report.client_id = rec_ptr->client_id;
             rec_ptr->prev_report.report_status = WMS_RPT_MO_RETRY_PERIOD_EXPIRED;
             rec_ptr->prev_report.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
             rec_ptr->prev_report.alpha_id.len = 0;
             rec_ptr->prev_report.alpha_id.data = NULL;
             rec_ptr->prev_report.message_mode = WMS_MESSAGE_MODE_CDMA;

             if( !(tl_ptr->mask & WMS_MASK_TL_BEARER_REPLY_OPTION) )
             {
               /* ---------------- TL ack is NOT requested ------------- */
               MSG_HIGH_0("snd msg: No ACK requested");
             }
             else
             {
               /* ---------------- TL ack is requested ------------- */
               MSG_HIGH_0("MO msg tl_ack requested");
             }

             MSG_HIGH_1("Msg Saved in REC, SEQ Num %d  ", rec_ptr->seq_num);
             MSG_HIGH_3( "WMS initiating DC status %d, retry time left [%d|%d]",
                         st,
                         msg_s_ptr->mo_retry_period,
                         rec_ptr->retry_time_left);

             /* Set the Global Retry Timer */
             wms_set_retry_timer();
           }
           else
           {
             cmd_err = WMS_CMD_ERR_NO_RESOURCE;
             (void)wms_msg_cdma_get_next_mo_rec_before_delete( rec_ptr );
           }
         }
       }
       else
       {
          MSG_HIGH_0("DC origination failed");
          cmd_err = WMS_CMD_ERR_NO_RESOURCE;
       }
     }
     else
     {
       /* Send on already opened traffic channel */
       MSG_HIGH_2( "Traffic channel is opened, SMS call %d, Voice/Data call %d",
                   dc_s_ptr->call_active,
                   dc_s_ptr->is_voice_data_call_conn );

       cmd_err = wms_msg_cdma_send_MO(
                        send_status_to_clients,
                        cmd_ptr,
                        send_mode,
                        msg_hdr_ptr,
                        tl_ptr,
                        ota_ptr,
                        WMS_MSG_TRANSPORT_TYPE_CS);
     }
   }
   else
   {
     /* This should never happen */
     MSG_HIGH_0("Both Access and Traffic are disabled");
     cmd_err = WMS_CMD_ERR_PREF_CHN_UNSUPPORTED_S;
   }

   return cmd_err;

} /* wms_msg_cdma_send_mo_channel*/



#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
void wms_msg_gw_clear_mo_msg_rec
(
  uint32    rec_index
)
{
  if (NULL != msg_s_ptr->gw_mo_rec[rec_index])
  {
    MSG_HIGH_1("Clear GW MO rec %d", rec_index);

    /* reset any dynamic memory allocations */
    WMS_MEM_FREEIF(msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg);
    WMS_MEM_FREEIF(msg_s_ptr->gw_mo_rec[rec_index]);
  }
}

boolean wms_msg_gw_message_id_is_valid(
  wms_message_number_type message_id
)
{
   if (message_id > 255)
   {
      return FALSE;
   }
   return TRUE;
}

wms_ss_stack_info_s_type *wms_msg_get_current_gw_domain_pref_stack( 
  wms_prov_app_info_s_type *appInfo_ptr 
)
{
  int8  stack_idx;

  if ( NULL != appInfo_ptr) 
  {
    for ( stack_idx = 0; stack_idx < appInfo_ptr->number_of_stacks; stack_idx++ ) 
    {
      if ( appInfo_ptr->stack_info[stack_idx].is_stack_operational ) 
      {
        if  ( ((WMS_GW_DOMAIN_PREF_CS_ONLY == appInfo_ptr->curr_gw_domain_pref)  ||
               (WMS_GW_DOMAIN_PREF_CS_PREFERRED == appInfo_ptr->curr_gw_domain_pref)) &&
              ((SYS_SRV_DOMAIN_CS_ONLY == appInfo_ptr->stack_info[stack_idx].srv_domain) || 
               (SYS_SRV_DOMAIN_CS_PS == appInfo_ptr->stack_info[stack_idx].srv_domain)) )
        {
          return &appInfo_ptr->stack_info[stack_idx];
        }

        if  ( ((WMS_GW_DOMAIN_PREF_PS_ONLY == appInfo_ptr->curr_gw_domain_pref)  || 
               (WMS_GW_DOMAIN_PREF_PS_PREFERRED == appInfo_ptr->curr_gw_domain_pref)) &&
              ((SYS_SRV_DOMAIN_PS_ONLY == appInfo_ptr->stack_info[stack_idx].srv_domain) || 
               (SYS_SRV_DOMAIN_CS_PS == appInfo_ptr->stack_info[stack_idx].srv_domain)) )
        {
          return  &appInfo_ptr->stack_info[stack_idx];
        }
      } /* if (is_stack_operational) */
    }  /* for() */

    for ( stack_idx = 0; stack_idx < appInfo_ptr->number_of_stacks; stack_idx++ ) 
    {
      if (appInfo_ptr->stack_info[stack_idx].is_stack_operational)
      {
        return &appInfo_ptr->stack_info[stack_idx];
      }
    }
  } /* if ( NULL != appInfo_ptr) */

  return NULL;
} /* wms_msg_get_current_gw_domain_pref_stack() */

wms_cmd_err_e_type wms_msg_send_gw(
  uint32                      rec_index,
  wms_client_message_s_type  *cl_msg_ptr, /* Used for 1st MO SMS Attempt */
  boolean                     is_retry
)
{
  static wms_address_s_type   sc_address;
  wms_raw_ts_data_s_type     *raw_ts_ptr     = NULL;
  wms_cmd_err_e_type          cmd_err        = WMS_CMD_ERR_NONE;
  wms_message_number_type     message_number = WMS_DUMMY_MESSAGE_INDEX;
  wms_send_mode_e_type        send_mode      = msg_s_ptr->gw_mo_rec[rec_index]->send_mode;
  uint32                      i              = msg_s_ptr->gw_mo_rec[rec_index]->msg_hdr.index;
  wms_msg_gw_mo_record_type  *mo_rec_ptr     = msg_s_ptr->gw_mo_rec[rec_index];
  sys_modem_as_id_e_type      as_id;
  wms_prov_app_info_s_type   *appInfo_ptr;

  if (NULL == mo_rec_ptr)
  {
    return WMS_CMD_ERR_NULL_PTR;
  }

  as_id = mo_rec_ptr->as_id;
  appInfo_ptr = mo_rec_ptr->appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return WMS_CMD_ERR_AS_ID;
  }

  if (!is_retry)
  {
    if (send_mode == WMS_SEND_MODE_CLIENT_MESSAGE)
    {
      if (cl_msg_ptr != NULL)
      {
        raw_ts_ptr = &cl_msg_ptr->u.gw_message.raw_ts_data;
        sc_address = cl_msg_ptr->u.gw_message.sc_address;
      }
      else
      {
        cmd_err = WMS_CMD_ERR_NULL_PTR;
      }
    }
    else /* SEND FROM MEMORY */
    {
      switch (msg_s_ptr->gw_mo_rec[rec_index]->msg_hdr.mem_store)
      {
        case WMS_MEMORY_STORE_RAM_GW:
          cmd_err = wms_msg_map_gw_ram_index(as_id, i, &i);
          if (WMS_CMD_ERR_NONE != cmd_err)
          {
            break;
          }
          if (wms_ts_unpack_sim_sms(cfg_s_ptr->ram_gw_sms[i],
                                   &sim_sms_unpacked) != TRUE)
          {
            cmd_err = WMS_CMD_ERR_MSG_DECODE;
          }
          break;

        case WMS_MEMORY_STORE_USIM:
        case WMS_MEMORY_STORE_SIM:
          if (wms_sim_mmgsdi_read_data( appInfo_ptr,
                                        appInfo_ptr->sms_file_enum,
                                        (mmgsdi_rec_num_type)i,
                                        appInfo_ptr->sms_rec_len,
                                        sim_data,
                                        sizeof(sim_data)) != MMGSDI_SUCCESS)
          {
            cmd_err = WMS_CMD_ERR_MSG_SIM_READ;
          }
          else
          {
            sim_sms.status = sim_data[0];
            sim_sms.len    = (uint8)MIN(WMS_SIM_SMS_MAX-1, appInfo_ptr->sms_rec_len - 1);

            if (sim_sms.len > 0)
            {
              (void)memscpy( (void *)sim_sms.data,
                             sim_sms.len,
                             sim_data + 1,
                             sim_sms.len);
            }
            else
            {
              cmd_err = WMS_CMD_ERR_BAD_PARM;
            }

          }
          if (cmd_err == WMS_CMD_ERR_NONE)
          {
            if (wms_ts_unpack_sim_sms(&sim_sms, &sim_sms_unpacked ) == FALSE)
            {
              cmd_err = WMS_CMD_ERR_MSG_DECODE;
            }
          }
          break;

        case WMS_MEMORY_STORE_NV_GW:
          cmd_err = wms_msg_map_gw_nv_index(as_id, i, &i);
          if (WMS_CMD_ERR_NONE != cmd_err)
          {
            break;
          }
          if (wms_nv_read_gw_sms(i, sim_data) == FALSE)
          {
            cmd_err = WMS_CMD_ERR_MSG_NV_READ;
          }
          else
          {
            sim_sms.status = sim_data[0];
            sim_sms.len = (uint8)MIN(WMS_SIM_SMS_MAX-1, appInfo_ptr->sms_rec_len - 1);

            if (sim_sms.len > 0)
            {
              (void)memscpy( (void *)sim_sms.data,
                             sim_sms.len,
                             sim_data + 1,
                             sim_sms.len);
            }
            else
            {
              cmd_err = WMS_CMD_ERR_BAD_PARM;
            }

          }

          if (cmd_err == WMS_CMD_ERR_NONE)
          {
            if (wms_ts_unpack_sim_sms( & sim_sms, & sim_sms_unpacked ) == FALSE)
            {
              cmd_err = WMS_CMD_ERR_MSG_DECODE;
            }
          }
          break;

        default:
          cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
          MSG_ERROR_1("Invalid memory store %d", msg_s_ptr->gw_mo_rec[rec_index]->msg_hdr.mem_store);
          break;
      }

      raw_ts_ptr = &sim_sms_unpacked.raw_ts_data;
      sc_address = sim_sms_unpacked.sc_address;
    }

    /* Initialize the Prev Report to Timer Expired Status */
    msg_s_ptr->gw_mo_rec[rec_index]->prev_report.as_id = mo_rec_ptr->as_id;
    msg_s_ptr->gw_mo_rec[rec_index]->prev_report.user_data = msg_s_ptr->gw_mo_rec[rec_index]->user_data;
    msg_s_ptr->gw_mo_rec[rec_index]->prev_report.client_id = msg_s_ptr->gw_mo_rec[rec_index]->client_id;
    msg_s_ptr->gw_mo_rec[rec_index]->prev_report.report_status = WMS_RPT_MO_RETRY_PERIOD_EXPIRED;
    msg_s_ptr->gw_mo_rec[rec_index]->prev_report.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
    msg_s_ptr->gw_mo_rec[rec_index]->prev_report.alpha_id.len = 0;
    msg_s_ptr->gw_mo_rec[rec_index]->prev_report.alpha_id.data = NULL;
    msg_s_ptr->gw_mo_rec[rec_index]->prev_report.message_mode = WMS_MESSAGE_MODE_GW;
  }
  else /* retry == TRUE */
  {
    if (NULL != msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg)
    {
      raw_ts_ptr = &msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg->raw_ts_data;
      sc_address = msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg->sc_address;
    }
    else
    {
      cmd_err = WMS_CMD_ERR_NULL_PTR;
    }
  }

  if (cmd_err == WMS_CMD_ERR_NONE)
  {
    if ( wms_ts_decode( raw_ts_ptr, & wms_msg_Gclient_ts ) != WMS_OK_S )
    {
      cmd_err = WMS_CMD_ERR_MSG_DECODE;
    }
    else if (! WMS_IS_MO_TPDU_TYPE(wms_msg_Gclient_ts.u.gw_pp.tpdu_type))
    {
      cmd_err = WMS_CMD_ERR_MSG_TPDU_TYPE;
    }
    else
    {
      wms_msg_transport_s_type* transport_ptr = wms_msg_choose_registered_transport( mo_rec_ptr->as_id );
      MSG_HIGH_1("MO record transport type: %d", mo_rec_ptr->transport_type);
      if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == mo_rec_ptr->transport_type)
      {
        if (NULL == transport_ptr)
         {
           MSG_ERROR_0("Transport has not yet registered!");
           msg_s_ptr->gw_mo_rec[rec_index]->prev_report.report_status = WMS_RPT_TRANSPORT_NOT_READY;
           return WMS_CMD_ERR_TRANSPORT_NOT_READY;
        }
      }

      if (is_retry)
      {
        /* extract the message number */
        message_number = wms_msg_Gclient_ts.u.gw_pp.u.submit.message_reference;
      }
      else
      {
        /* Handle msg number during resend */
        if (mo_rec_ptr->resend_msg)
        {
          /* Enable reject_duplicates if resending to avoid incrementing message number */
          wms_msg_Gclient_ts.u.gw_pp.u.submit.reject_duplicates = TRUE;

          if ( wms_msg_gw_message_id_is_valid(mo_rec_ptr->client_msg_id) )
          {
            /* Use client-provided message_id if valid */
            wms_msg_Gclient_ts.u.gw_pp.u.submit.message_reference = mo_rec_ptr->client_msg_id;
          }
        }

        /* Don't change msg number for Command or if rej_dup is TRUE
        */
        if (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_SUBMIT &&
            wms_msg_Gclient_ts.u.gw_pp.u.submit.reject_duplicates == FALSE)
        {
          /* use the next message number
          */
          message_number = wms_cfg_get_sim_next_message_number(mo_rec_ptr->appInfo_ptr);
          wms_msg_Gclient_ts.u.gw_pp.u.submit.message_reference = message_number;
        }
        else if (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_COMMAND)
        {
          message_number = wms_cfg_get_sim_next_message_number(mo_rec_ptr->appInfo_ptr);
          wms_msg_Gclient_ts.u.gw_pp.u.command.message_reference = message_number;
        }
        else
        {
          message_number = wms_msg_Gclient_ts.u.gw_pp.u.submit.message_reference;
        }

        wms_msg_Gclient_ts.format = WMS_FORMAT_GW_PP;

        if(wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_SUBMIT)
        {
          if((wms_msg_Gclient_ts.u.gw_pp.u.submit.user_data.sm_len > WMS_SIM_SMS_MAX ) ||
             ((wms_msg_Gclient_ts.u.gw_pp.u.submit.user_data.sm_len > WMS_SMS_UDL_MAX_7_BIT) &&
              (wms_msg_Gclient_ts.u.gw_pp.u.submit.dcs.alphabet == WMS_GW_ALPHABET_7_BIT_DEFAULT)) ||
             ((wms_msg_Gclient_ts.u.gw_pp.u.submit.user_data.sm_len > WMS_SMS_UDL_MAX_8_BIT) &&
              (wms_msg_Gclient_ts.u.gw_pp.u.submit.dcs.alphabet == WMS_GW_ALPHABET_8_BIT)))
          {
            cmd_err = WMS_CMD_ERR_MSG_SEND_MODE;
          }

           /* Check for FDN if it's not proactive command */
          if ((cmd_err == WMS_CMD_ERR_NONE) &&
              (WMS_CLIENT_TYPE_CAT != mo_rec_ptr->client_id))
          {
             cmd_err = wms_msg_mo_sms_fdn_control_func(appInfo_ptr,
                                                       WMS_MESSAGE_MODE_GW,
                                                       &wms_msg_Gclient_ts.u.gw_pp.u.submit.address);
          }
        }

        /* Check for Valid SC Address */
        if ((cmd_err == WMS_CMD_ERR_NONE)&&(sc_address.number_of_digits == 0))
        {
          MSG_HIGH_0("No valid SC address found, filling from template");
          cmd_err = wms_msg_fill_sc_address (mo_rec_ptr->appInfo_ptr, &sc_address );
          if ( cmd_err == WMS_CMD_ERR_NONE )
          {
            (void)memscpy( &sim_sms_unpacked.sc_address,
                           sizeof(wms_address_s_type),
                           &sc_address,
                           sizeof(wms_address_s_type));
            if (cl_msg_ptr != NULL)
            {
              (void)memscpy( &cl_msg_ptr->u.gw_message.sc_address,
                             sizeof(wms_address_s_type),
                             &sc_address,
                             sizeof(wms_address_s_type));
            }
          }
        }

      if ((cmd_err == WMS_CMD_ERR_NONE) && 
            (WMS_CLIENT_TYPE_CAT != mo_rec_ptr->client_id) &&
            (TRUE == cfg_s_ptr->enable_fdn_control))
        {
          /* Check for FDN for SC Address if it's not proactive command */
          cmd_err = wms_msg_mo_sms_fdn_control_func(appInfo_ptr, WMS_MESSAGE_MODE_GW, &sc_address);
        }
      }

  #if defined (FEATURE_GWSMS_MO_CONTROL) && defined (FEATURE_GSTK)
      {
        if (!is_retry && appInfo_ptr->mo_control_enabled)
        {
          if (transport_ptr != NULL && transport_ptr->is_mn_registered)
          {
            // Disable MO control for MO SMS over LTE and eHRPD
            msg_s_ptr->gw_mo_rec[rec_index]->mo_control_pending = FALSE;
          }
          else
          {
            msg_s_ptr->gw_mo_rec[rec_index]->mo_control_pending = TRUE;
          } 
        }

        if ( WMS_CMD_ERR_NONE == cmd_err && TRUE == msg_s_ptr->gw_mo_rec[rec_index]->mo_control_pending)
        {
          gstk_mo_sms_ctrl_ind_ext_type  mo_sms_control_buf;
 
          if (TRUE == appInfo_ptr->mo_waiting_for_gstk_resp)
          {
             /* MO control is not available */
          }
          else if(wms_ts_convert_to_gstk_address(
                  &sc_address,
                  & mo_sms_control_buf.rp_dest_address) == FALSE)
          {
            cmd_err = WMS_CMD_ERR_MSG_ENCODE;
          }
          else if(wms_ts_convert_to_gstk_address(
                  & wms_msg_Gclient_ts.u.gw_pp.u.submit.address,
                  & mo_sms_control_buf.tp_dest_address) == FALSE)
          {
            cmd_err = WMS_CMD_ERR_MSG_ENCODE;
          }
          else
          {
            wms_ss_stack_info_s_type *stack_info_ptr;

            //Select serving sys_mode based on current domain pref.
            stack_info_ptr = wms_msg_get_current_gw_domain_pref_stack( appInfo_ptr );
            if ( NULL == stack_info_ptr ||
                 SYS_SRV_DOMAIN_NO_SRV  == stack_info_ptr->srv_domain )
            {
              cmd_err = WMS_CMD_ERR_NO_NETWORK;
            }
            else
            {
              gstk_status_enum_type  gstk_st;

              mo_sms_control_buf.rat = wms_cm_map_sys_mode_to_gstk_access_tech_rat( stack_info_ptr->sys_mode );
              MSG_HIGH_1("GSTK Acess Tech = %d", mo_sms_control_buf.rat );
#ifdef FEATURE_SMS_PWS_OVER_LTE
              // Use MNC and TAC info from RRC in the case of LTE
              if ( GSTK_ACCESS_TECH_LTE == mo_sms_control_buf.rat )
              {
                uint8 *tac_ptr = (uint8 *)&current_cell_info.tac;

                /* Spec TS 24.301, section 9.9.3.12 specifies:.

                   PLMN ID is a combination of MCC and MNC. It is 3 bytes and it is arranged as:

                     MCC digit 2 + MCC digit 1    ------> 1 byte

                     MNC digit 3 + MCC digit 3    ------> 2 byte

                     MNC digit 2 + MNC digit 1    ------> 3 byte
                */

                /* ----------------------------------------
                ** PLMN octet 1 = MCC digit 2 | MCC digit 1
                ** ---------------------------------------- */
                mo_sms_control_buf.loc_info.gwl_info.mcc_and_mnc[0] = (current_cell_info.selected_plmn.mcc[1] << 4) | 
                                                                      (current_cell_info.selected_plmn.mcc[0] & 0x0F);

                /* ----------------------------------------
                ** PLMN octet 2 = MNC digit 3 | MCC digit 3
                ** ---------------------------------------- */
                mo_sms_control_buf.loc_info.gwl_info.mcc_and_mnc[1] = (current_cell_info.selected_plmn.mnc[2] << 4) | 
                                                                      (current_cell_info.selected_plmn.mcc[2] & 0x0F);

                /* ----------------------------------------
                ** PLMN octet 3 = MNC digit 2 | MNC digit 1
                ** ---------------------------------------- */
                mo_sms_control_buf.loc_info.gwl_info.mcc_and_mnc[2] = (current_cell_info.selected_plmn.mnc[1] << 4) | 
                                                                      (current_cell_info.selected_plmn.mnc[0] & 0x0F);
                
                /* Spec TS 24.301 Section 9.9.3.32 says that the Tracking Area Identity (TAI) is a 6 byte field out of
                   which 2 bytes are defined for the Tracking Area Code (TAC) */
                mo_sms_control_buf.loc_info.gwl_info.loc_area_code[0] = *(tac_ptr+1); 
                mo_sms_control_buf.loc_info.gwl_info.loc_area_code[1] = *tac_ptr;
              }
              // Use RRC cell change event
              else 
#endif /* FEATURE_SMS_PWS_OVER_LTE */                
              if ( GSTK_ACCESS_TECH_UTRAN == mo_sms_control_buf.rat ) 
              {
                wms_cfg_s_type  *cfg_s_ptr  = wms_cfg_s_ptr();

                /* Spec TS 123 003, section 4.1 specifies that Location Area Indentity consists of:
                       MCC: 3 octets
                       MNC: 2/3 octets
                       LA : 2 octets
                */
                mo_sms_control_buf.loc_info.gwl_info.mcc_and_mnc[0] =  cfg_s_ptr->umts_loc_info.mcc_and_mnc[0];
                mo_sms_control_buf.loc_info.gwl_info.mcc_and_mnc[1] =  cfg_s_ptr->umts_loc_info.mcc_and_mnc[1];
                mo_sms_control_buf.loc_info.gwl_info.mcc_and_mnc[2] =  cfg_s_ptr->umts_loc_info.mcc_and_mnc[2];

                mo_sms_control_buf.loc_info.gwl_info.loc_area_code[0] = cfg_s_ptr->umts_loc_info.loc_area_code[0];
                mo_sms_control_buf.loc_info.gwl_info.loc_area_code[1] = cfg_s_ptr->umts_loc_info.loc_area_code[1];
              }
              else if ( GSTK_ACCESS_TECH_GSM == mo_sms_control_buf.rat ) 
              {
                mo_sms_control_buf.loc_info.gwl_info.mcc_and_mnc[0] =  stack_info_ptr->sys_id.id.plmn.identity[0];
                mo_sms_control_buf.loc_info.gwl_info.mcc_and_mnc[1] =  stack_info_ptr->sys_id.id.plmn.identity[1]; 
                mo_sms_control_buf.loc_info.gwl_info.mcc_and_mnc[2] =  stack_info_ptr->sys_id.id.plmn.identity[2];

                mo_sms_control_buf.loc_info.gwl_info.loc_area_code[1] = 
                      (byte)(stack_info_ptr->sys_id.id.plmn_lac.lac & 0x00FF);
                mo_sms_control_buf.loc_info.gwl_info.loc_area_code[0] = 
                      (byte)((stack_info_ptr->sys_id.id.plmn_lac.lac & 0xFF00) >> 8);
              }
              else 
              {
                cmd_err = WMS_CMD_ERR_MSG_GSTK_ACCESS_RAT;
              }

              if ( WMS_CMD_ERR_NONE == cmd_err ) 
              {
                mo_sms_control_buf.loc_info.gwl_info.cell_id[0] = 0xFF;
                mo_sms_control_buf.loc_info.gwl_info.cell_id[1] = 0xFF;
                mo_sms_control_buf.loc_info.gwl_info.cell_id[2] = 0xFF;
                mo_sms_control_buf.loc_info.gwl_info.cell_id[3] = 0xFF;
                mo_sms_control_buf.loc_info.gwl_info.cell_id_len = 0;

                MSG_HIGH_3( "mcc_and_mnc[] = %02x %02x %02x",
                             mo_sms_control_buf.loc_info.gwl_info.mcc_and_mnc[0],
                             mo_sms_control_buf.loc_info.gwl_info.mcc_and_mnc[1],
                             mo_sms_control_buf.loc_info.gwl_info.mcc_and_mnc[2] );

                MSG_HIGH_2( "loc_area_code[] = %02x %02x",
                             mo_sms_control_buf.loc_info.gwl_info.loc_area_code[0],
                             mo_sms_control_buf.loc_info.gwl_info.loc_area_code[1] );

                gstk_st = gstk_slot_send_envelope_mo_sms_ctrl_command_ext(
                                                  (gstk_slot_id_enum_type)appInfo_ptr->mmgsdi_appInfo.slot_id,
                                                  (uint32) rec_index,
                                                  & mo_sms_control_buf,
                                                  (gstk_evt_cb_funct_type) wms_msg_gstk_evt_cb);

                if ( gstk_st == GSTK_SUCCESS )
                {
                  appInfo_ptr->mo_waiting_for_gstk_resp               = TRUE;
                }
                else
                {
                  cmd_err = WMS_CMD_ERR_MSG_SIM_WRITE;
                }
              }
            }
          }
        }
      }
  #endif /* FEATURE_GWSMS_MO_CONTROL && FEATURE_GSTK */
    }
  }


  if((cmd_err == WMS_CMD_ERR_NONE) && (is_retry == FALSE))
  {
    /* Encode to raw ts data and store if appropriate.
    */
    if(wms_ts_encode(&wms_msg_Gclient_ts, raw_ts_ptr) != WMS_OK_S)
    {
      cmd_err = WMS_CMD_ERR_MSG_ENCODE;
    }

    /* Store messages for no retry and send from memory
    */
    if(send_mode == WMS_SEND_MODE_MEMORY_STORE)
    {
      sim_sms_unpacked.status = (uint8)WMS_TAG_MO_NOT_SENT;

      switch(msg_s_ptr->gw_mo_rec[rec_index]->msg_hdr.mem_store)
      {
        case WMS_MEMORY_STORE_RAM_GW:

          if(NULL != cfg_s_ptr->ram_gw_sms[i])
          {
             if(wms_ts_pack_sim_sms(&sim_sms_unpacked,
                                    cfg_s_ptr->ram_gw_sms[i]) != TRUE)
             {
               cmd_err = WMS_CMD_ERR_MSG_ENCODE;
             }
          }
          else
          {
             cmd_err = WMS_CMD_ERR_MSG_INDEX;
          }
          break;

        case WMS_MEMORY_STORE_USIM:
        case WMS_MEMORY_STORE_SIM:
          if(wms_ts_pack_sim_sms(&sim_sms_unpacked, &sim_sms) != TRUE)
          {
            cmd_err = WMS_CMD_ERR_MSG_ENCODE;
          }
          else
          {
            /* Write it back to SIM */
            sim_data[0] = sim_sms.status;
            (void)memscpy( sim_data + 1,
                           WMS_SIM_SMS_MAX,
                           (void *)sim_sms.data,
                           WMS_SIM_SMS_MAX);
            if( wms_sim_mmgsdi_write_data(appInfo_ptr,
                                          appInfo_ptr->sms_file_enum,
                                          (mmgsdi_rec_num_type)i,
                                          sim_data,
                                          (uint32)appInfo_ptr->sms_rec_len ) != MMGSDI_SUCCESS )
            {
              cmd_err = WMS_CMD_ERR_MSG_SIM_WRITE;
            }
            else
            {
              /*The sms reference is required for mapping it to the status reports
              if the status report is requested. Otherwise it is defaulted to WMS_DUMMY_MESSAGE_NUMBER. */
              if( wms_msg_Gclient_ts.u.gw_pp.u.submit.status_report_enabled )
              {
                appInfo_ptr->sms_reference[i] = wms_msg_Gclient_ts.u.gw_pp.u.submit.message_reference;
              }
              else
              {
                appInfo_ptr->sms_reference[i] = WMS_DUMMY_MESSAGE_NUMBER;
              }
              appInfo_ptr->sms_rec_tags[i] = (wms_message_tag_e_type)sim_sms.status;
            }
          }
          break;

        case WMS_MEMORY_STORE_NV_GW:
          if(wms_ts_pack_sim_sms(&sim_sms_unpacked, &sim_sms) != TRUE)
          {
            cmd_err = WMS_CMD_ERR_MSG_ENCODE;
          }
          else
          {
            sim_data[0] = sim_sms.status;
            if (sim_sms.len > 0)
            {
              (void)memscpy( sim_data + 1,
                             MIN(WMS_SIM_SMS_MAX, sim_sms.len),
                             (void *)sim_sms.data,
                             MIN(WMS_SIM_SMS_MAX, sim_sms.len));
            }
            else
            {
              cmd_err = WMS_CMD_ERR_BAD_PARM;
              break;
            }

            if( wms_nv_replace_gw_sms( i, sim_data ) == FALSE )
            {
              cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
            }
          }
          break;

        default:
          cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
          MSG_ERROR_1("Invalid memory store %d", msg_s_ptr->gw_mo_rec[rec_index]->msg_hdr.mem_store);
          break;
      }
    }
  }

  /* We're ready to try and send the message
  */
  if(cmd_err == WMS_CMD_ERR_NONE)
  {
#ifdef FEATURE_GWSMS_MO_CONTROL
    if (msg_s_ptr->gw_mo_rec[rec_index]->mo_control_pending == TRUE)
    {
      if(!is_retry)
      {
        if(send_mode == WMS_SEND_MODE_MEMORY_STORE)
        {
          /* We have to build a client message */
          msg_s_ptr->gw_mo_rec[rec_index]->mo_control_cl_msg.msg_hdr =
          msg_s_ptr->gw_mo_rec[rec_index]->msg_hdr;
          msg_s_ptr->gw_mo_rec[rec_index]->mo_control_cl_msg.u.gw_message.sc_address =
            sc_address;
          msg_s_ptr->gw_mo_rec[rec_index]->mo_control_cl_msg.u.gw_message.is_broadcast = FALSE;
          if (raw_ts_ptr != NULL)
          {
            (void)memscpy( &msg_s_ptr->gw_mo_rec[rec_index]->mo_control_cl_msg.u.gw_message.raw_ts_data,
                           sizeof(wms_raw_ts_data_s_type),
                           raw_ts_ptr,
                           sizeof(wms_raw_ts_data_s_type));
          }
          else
          {
            MSG_ERROR_0("raw_ts_ptr is NULL in wms_msg_send_gw");
                  cmd_err = WMS_CMD_ERR_NULL_PTR;
          } 
        }
        else
        {
          if (cl_msg_ptr != NULL)
          {
            (void)memscpy( &msg_s_ptr->gw_mo_rec[rec_index]->mo_control_cl_msg,
                           sizeof(wms_client_message_s_type),
                           cl_msg_ptr,
                           sizeof(wms_client_message_s_type));
          }
          else
          {
            MSG_ERROR_0("cl_msg_ptr is NULL in wms_msg_send_gw");
            cmd_err = WMS_CMD_ERR_NULL_PTR;
          }
        }
      }
    }
    else
#endif /* FEATURE_GWSMS_MO_CONTROL */
    {
      if ( wms_msg_suspend_ps_call_is_supported() )
      {
        cm_call_cmd_err_e_type suspend_status = CM_CALL_CMD_ERR_NOERR;

        if (!is_retry)
        {
          suspend_status = wms_cm_suspend_pscall(msg_s_ptr->gw_mo_rec[rec_index]->as_id);
        }

        if (CM_CALL_CMD_ERR_NOERR != suspend_status)
        {
          cmd_err = WMS_CMD_ERR_TRANSPORT_NOT_READY;
        }
      }

      if (WMS_CMD_ERR_NONE == cmd_err)
      {
        cmd_err = wms_cm_gw_send_rl_data( mo_rec_ptr, &sc_address,
                                          (uint8)message_number,
                                          raw_ts_ptr );
      }
    }
  }

  if (cmd_err == WMS_CMD_ERR_NONE)
  {
    /* Finally update the mo record pointer */
    msg_s_ptr->gw_mo_rec[rec_index]->smi    = (uint8)message_number;
    MSG_HIGH_2( "GW MO Record %d in use SMI %d",
                rec_index,
                msg_s_ptr->gw_mo_rec[rec_index]->smi );

    if(!is_retry)
    {
      if(msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg == NULL)
      {
        /* allocate space for this gw message */
        msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg =
          wms_mem_malloc(sizeof(wms_gw_message_s_type));

        if(msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg == NULL)
        {
          MSG_ERROR_1("msg_s_ptr->gw_mo_rec[%d].sent_gw_msg is NULL", rec_index);
          cmd_err = WMS_CMD_ERR_NULL_PTR;
          return cmd_err;
        }

      }
      msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg->is_broadcast = FALSE;
      msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg->sc_address   = sc_address;

      if (raw_ts_ptr != NULL)
      {
        (void)memscpy( &msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg->raw_ts_data,
                       sizeof(wms_raw_ts_data_s_type),
                       raw_ts_ptr,
                       sizeof(wms_raw_ts_data_s_type));
      }
      else
      {
        MSG_ERROR_0("raw_ts_ptr NULL Pointer!");
        cmd_err = WMS_CMD_ERR_NULL_PTR;
      }
    }

    /* Set the Global Retry Timer */
    wms_set_retry_timer();
  }

  MSG_HIGH_1("wms_msg_send_gw() returns %d", cmd_err);

  return cmd_err;
} /* wms_msg_send_gw() */


boolean wms_is_gw_admin_msg(wms_raw_ts_data_s_type *raw_ts_ptr)
{
  wms_client_ts_data_s_type  *client_ts_data_ptr = 
    (wms_client_ts_data_s_type *) wms_mem_malloc ( sizeof(wms_client_ts_data_s_type) );

  if (NULL == client_ts_data_ptr || NULL == raw_ts_ptr || WMS_OK_S != wms_ts_decode(raw_ts_ptr, client_ts_data_ptr))
  {
    MSG_ERROR_0("Error occured when calling wms_ts_decode()");
    WMS_MEM_FREEIF(client_ts_data_ptr);
    return FALSE;
  }
  else
  {
    boolean st = FALSE;

    if (WMS_TPDU_SUBMIT == client_ts_data_ptr->u.gw_pp.tpdu_type)
    {

      if (WMS_PID_SIM_DATA_DOWNLOAD == client_ts_data_ptr->u.gw_pp.u.submit.pid)
      {
        MSG_HIGH_0("wms_is_gw_admin_msg() returns TRUE becuse PID = WMS_PID_SIM_DATA_DOWNLOAD");
        st = TRUE;
      }
      else if (client_ts_data_ptr->u.gw_pp.u.submit.user_data_header_present)
      {
        uint8 i = 0;
        for (i = 0; i < client_ts_data_ptr->u.gw_pp.u.submit.user_data.num_headers; i++)
        {
          /* Port addressing scheme:
             2948 - WAP Push connectionless session service (client side)
             2949 - WAP Push secure connectionless session service (client side)
          */
          if ((WMS_UDH_PORT_16 == client_ts_data_ptr->u.gw_pp.u.submit.user_data.headers[i].header_id) &&
              ((2948 == client_ts_data_ptr->u.gw_pp.u.submit.user_data.headers[i].u.wap_16.dest_port) ||
               (2949 == client_ts_data_ptr->u.gw_pp.u.submit.user_data.headers[i].u.wap_16.dest_port)))
          {
            /* Is a WAP Push Message */
            MSG_HIGH_0("wms_is_gw_admin_msg() returns TRUE because it is a WAP msg");
            st = TRUE;
            break;
          }
        }
      }
    }
    else if (WMS_TPDU_DELIVER == client_ts_data_ptr->u.gw_pp.tpdu_type ||
             WMS_TPDU_RESERVED == client_ts_data_ptr->u.gw_pp.tpdu_type)
    {
      if (WMS_PID_SIM_DATA_DOWNLOAD == client_ts_data_ptr->u.gw_pp.u.deliver.pid)
      {
         MSG_HIGH_0("wms_is_gw_admin_msg() returns TRUE becuse PID = WMS_PID_SIM_DATA_DOWNLOAD");
         st = TRUE;
      }
      else if (client_ts_data_ptr->u.gw_pp.u.deliver.user_data_header_present)
      {
        uint8 i = 0;
        for (i = 0; i < client_ts_data_ptr->u.gw_pp.u.deliver.user_data.num_headers; i++)
        {
          if ((WMS_UDH_PORT_16 == client_ts_data_ptr->u.gw_pp.u.deliver.user_data.headers[i].header_id) &&
              ((2948 == client_ts_data_ptr->u.gw_pp.u.deliver.user_data.headers[i].u.wap_16.dest_port) ||
               (2949 == client_ts_data_ptr->u.gw_pp.u.deliver.user_data.headers[i].u.wap_16.dest_port)))
          {
            /* Is a WAP Push Message */
            MSG_HIGH_0("wms_is_gw_admin_msg() returns TRUE because it is a WAP msg");
            st = TRUE;
            break;
          }
        }
      }
    }
    WMS_MEM_FREEIF(client_ts_data_ptr);
    return st; 
  }
}

#endif /* FEATURE_GWSMS */

/*
*/
wms_cmd_err_e_type wms_msg_send_from_memory_store(
  wms_cmd_type   *cmd_ptr,
  uint32          rec_index
)
{
  uint32                      i              = cmd_ptr->cmd.msg_send.message.msg_hdr.index;
  wms_cmd_err_e_type          cmd_err        = WMS_CMD_ERR_NONE;
  sys_modem_as_id_e_type      as_id          = cmd_ptr->hdr.as_id;
  wms_prov_app_info_s_type   *appInfo_ptr   = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     cmd_err =  WMS_CMD_ERR_AS_ID;
     wms_client_cmd_status( cmd_ptr, cmd_err );
     return cmd_err;
  }

  switch ( cmd_ptr->cmd.msg_send.message.msg_hdr.mem_store )
  {

#ifdef FEATURE_CDSMS
    case WMS_MEMORY_STORE_RUIM:
    case WMS_MEMORY_STORE_CSIM:
    {
      if( i >= (uint32) appInfo_ptr->sms_rec_count )
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
      }
      else if( ! WMS_IS_MO( appInfo_ptr->sms_rec_tags[i] ) )
      {
        cmd_err = WMS_CMD_ERR_MSG_TAG;
      }
      else if (wms_sim_mmgsdi_read_data( appInfo_ptr,
                                         appInfo_ptr->sms_file_enum,
                                         (mmgsdi_rec_num_type)i,
                                         appInfo_ptr->sms_rec_len,
                                         ruim_data,
                                         sizeof(ruim_data)) != MMGSDI_SUCCESS)
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_READ;
      }
      else if( wms_ts_cdma_unpack_ruim_sms( ruim_data, & ruim_sms ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_DECODE;
      }
      else if( wms_ts_decode_CDMA_OTA( & ruim_sms.ota, & cdma_tl ) != WMS_OK_S )
      {
        cmd_err = WMS_CMD_ERR_MSG_DECODE;
      }
      else if( ! WMS_IS_MO_CDMA_MSG_TYPE( cdma_tl.cl_bd.message_id.type ) )
      {
        cmd_err = WMS_CMD_ERR_MSG_TPDU_TYPE;
      }
      else
      {
        boolean encoding_required = TRUE;

        if (WMS_BD_TYPE_SUBMIT == cdma_tl.cl_bd.message_id.type &&
            TRUE == appInfo_ptr->in_roaming &&
            wms_cm_CT_features_supported( appInfo_ptr->asid ))
        {
          MSG_HIGH_0("Removing Priority and Reply Option");
          cdma_tl.cl_bd.mask &= ~ WMS_MASK_BD_PRIORITY;
          cdma_tl.cl_bd.mask &= ~ WMS_MASK_BD_REPLY_OPTION;
        }
        /* Don't change msg id if it is CANCELLATION or USER_ACK:
        */
        if( cdma_tl.cl_bd.message_id.type == WMS_BD_TYPE_SUBMIT &&
            cdma_tl.teleservice           != WMS_TELESERVICE_WAP )
        {
          if (cmd_ptr->cmd.msg_send.resend_msg)
          {
            /* If resending message, use client-provided message id if valid
            */
            if ( wms_msg_cdma_message_id_is_valid(cmd_ptr->cmd.msg_send.client_msg_id) )
            {
              /* Valid message id from client, use it and reencode
              */
              // encoding_required = TRUE;
              cdma_tl.cl_bd.message_id.id_number = cmd_ptr->cmd.msg_send.client_msg_id;
            }
            else
            {
              /* Invalid message id from client, use the one already encoded in raw data
              */
              encoding_required = FALSE;
            }
          }
          else
          {
            /* use the next message number
            */
            cdma_tl.cl_bd.message_id.id_number = wms_cfg_get_ruim_next_message_number();
          }
        }

        /* use the next sequence number
        */
        if( cdma_tl.mask & WMS_MASK_TL_BEARER_REPLY_OPTION )
        {
          cdma_tl.bearer_reply_seq_num = wms_msg_cdma_next_seq_num();
        }
        else
        {
          cdma_tl.bearer_reply_seq_num = wms_msg_cdma_next_non_tl_seq_num();
        }

        if( WMS_OK_S != wms_msg_get_next_unused_seq_num(cdma_tl.bearer_reply_seq_num,
                                                        &cdma_tl.bearer_reply_seq_num) )
        {
           cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        }

        if (FALSE == wms_msg_convert_plus_address(&cdma_tl.address))
        {
          MSG_ERROR_0("wms_msg_convert_plus_address Failed!");
        }

        /* Encode the message with new info
        */
        cdma_tl.is_mo = TRUE;
        if( wms_ts_encode_CDMA_OTA( & cdma_tl, & ruim_sms.ota )
              != WMS_OK_S )
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }
        /* Update the contents of cmd_ptr w/new message_id */
        else if( (WMS_CDMA_USER_DATA_MAX >= cdma_tl.cl_bd.user_data.number_of_digits) &&
                  encoding_required &&
                 (wms_ts_encode_bearer_data(&cdma_tl.cl_bd,
                                          &cmd_ptr->cmd.msg_send.message.u.cdma_message.raw_ts) != WMS_OK_S) )
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }
        else
        {
          /* Change the tag to NOT_SENT since we are going to send/resend it
          */
          ruim_sms.status          = (uint8)WMS_TAG_MO_NOT_SENT;
          if( wms_ts_cdma_pack_ruim_sms( & ruim_sms, ruim_data ) == FALSE )
          {
            cmd_err = WMS_CMD_ERR_MSG_ENCODE;
          }
        }
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        if (wms_sim_mmgsdi_write_data( appInfo_ptr,
                                       appInfo_ptr->sms_file_enum,
                                       (mmgsdi_rec_num_type)i,
                                       ruim_data,
                                       (uint32)appInfo_ptr->sms_rec_len) != MMGSDI_SUCCESS)
        {
          cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
        }
        else
        {
          /* write was successful, update tag */
          appInfo_ptr->sms_rec_tags[i] = (wms_message_tag_e_type)ruim_sms.status;

          cmd_err = wms_msg_cdma_send_mo_channel(
                            TRUE,
                            cmd_ptr,
                            cmd_ptr->cmd.msg_send.send_mode,
                            & cmd_ptr->cmd.msg_send.message.msg_hdr,
                            & cdma_tl,
                            & ruim_sms.ota);
        }
      }

      // Populate and Dispatch Command Callback
      wms_client_cmd_status( cmd_ptr, cmd_err );

      // Populate and Dispatch Send Event Callback
      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
        msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
        msg_event_info.status_info.send_mode  = cmd_ptr->cmd.msg_send.send_mode;
        msg_event_info.status_info.message    = cmd_ptr->cmd.msg_send.message;
        msg_event_info.status_info.alpha_id.len = 0;
        msg_event_info.status_info.alpha_id.data = NULL;
        msg_event_info.status_info.as_id = as_id;
        msg_event_info.status_info.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
        (void)wms_msg_event_notify( WMS_MSG_EVENT_SEND, &msg_event_info );
      }
    }
    break; //WMS_MEMORY_STORE_RUIM

    case WMS_MEMORY_STORE_NV_CDMA:
      if( i >= cfg_s_ptr->nv_cdma_max_slots )
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
      }
      else if( ! WMS_IS_MO( cfg_s_ptr->nv_cdma_tags[i]) )
      {
        cmd_err = WMS_CMD_ERR_MSG_TAG;
      }
      else if( wms_nv_read_cdma_sms( i, ruim_data ) != TRUE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_READ;
      }
      else if( ruim_data[0] != (uint8) WMS_FORMAT_CDMA )
      {
        cmd_err = WMS_CMD_ERR_MSG_FORMAT;
      }
      else
      {
        /* Layout of SMS message in NV. */
        /* 0: format                    */
        /* 1: tag/status                */
        /* 2: len                       */
        /* 3+: data                     */
        ruim_sms.ota.format      = WMS_FORMAT_CDMA;
        ruim_sms.status          = (uint8) WMS_TAG_MO_NOT_SENT;
        ruim_sms.ota.data_len    = MIN (ruim_data[2], WMS_MAX_LEN);
        (void)memscpy( (void *)ruim_sms.ota.data,
                       ruim_sms.ota.data_len,
                       ruim_data + 3,
                       ruim_sms.ota.data_len );
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        if( wms_ts_decode_CDMA_OTA( & ruim_sms.ota, & cdma_tl ) != WMS_OK_S )
        {
          cmd_err = WMS_CMD_ERR_MSG_DECODE;
        }
        else if( ! WMS_IS_MO_CDMA_MSG_TYPE( cdma_tl.cl_bd.message_id.type ) )
        {
          cmd_err = WMS_CMD_ERR_MSG_TPDU_TYPE;
        }
        else
        {
          boolean encoding_required = TRUE;

          if (WMS_BD_TYPE_SUBMIT == cdma_tl.cl_bd.message_id.type &&
              TRUE == appInfo_ptr->in_roaming &&
              wms_cm_CT_features_supported( appInfo_ptr->asid ))
          {
            MSG_HIGH_0("Removing Priority and Reply Option");
            cdma_tl.cl_bd.mask &= ~ WMS_MASK_BD_PRIORITY;
            cdma_tl.cl_bd.mask &= ~ WMS_MASK_BD_REPLY_OPTION;
          }

          /* Don't change msg id if it is CANCELLATION or USER_ACK:
          */
          if( cdma_tl.cl_bd.message_id.type == WMS_BD_TYPE_SUBMIT &&
              cdma_tl.teleservice           != WMS_TELESERVICE_WAP )
          {
            if (cmd_ptr->cmd.msg_send.resend_msg)
            {
              /* If resending message, use client-provided message id if valid
              */
              if ( wms_msg_cdma_message_id_is_valid(cmd_ptr->cmd.msg_send.client_msg_id) )
              {
                /* Valid message id from client, use it and reencode
                */
                // encoding_required = TRUE;
                cdma_tl.cl_bd.message_id.id_number = cmd_ptr->cmd.msg_send.client_msg_id;
              }
              else
              {
                /* Invalid message id from client, use the one already encoded in raw data
                */
                encoding_required = FALSE;
              }
            }
            else
            {
              /* use the next message number
              */
              cdma_tl.cl_bd.message_id.id_number = wms_cfg_get_ruim_next_message_number();
            }
          }

          /* use the next sequence number if TL Ack is requested
          */
          if( cdma_tl.mask & WMS_MASK_TL_BEARER_REPLY_OPTION )
          {
            cdma_tl.bearer_reply_seq_num = (uint8) wms_msg_cdma_next_seq_num();
          }
          else
          {
            cdma_tl.bearer_reply_seq_num = wms_msg_cdma_next_non_tl_seq_num();
          }

          if (WMS_OK_S != wms_msg_get_next_unused_seq_num(cdma_tl.bearer_reply_seq_num,
                                                          &cdma_tl.bearer_reply_seq_num))
          {
             cmd_err = WMS_CMD_ERR_NO_RESOURCE;
          }

          if (FALSE == wms_msg_convert_plus_address(&cdma_tl.address))
          {
            MSG_ERROR_0("wms_msg_convert_plus_address Failed!");
          }

          /* Encode the message with new info
          */
          cdma_tl.is_mo = TRUE;
          if( wms_ts_encode_CDMA_OTA( & cdma_tl, & ruim_sms.ota )
                != WMS_OK_S )
          {
            cmd_err = WMS_CMD_ERR_MSG_ENCODE;
          }
          /* Update the contents of cmd_ptr w/new message_id */
          else if( (WMS_CDMA_USER_DATA_MAX >= cdma_tl.cl_bd.user_data.number_of_digits) &&
                    encoding_required &&
                    (wms_ts_encode_bearer_data(&cdma_tl.cl_bd,
                                          &cmd_ptr->cmd.msg_send.message.u.cdma_message.raw_ts) != WMS_OK_S) )
          {
            cmd_err = WMS_CMD_ERR_MSG_ENCODE;
          }
          else
          {
            (void)memscpy( ruim_data + 3,
                           ruim_sms.ota.data_len,
                           (void *)ruim_sms.ota.data,
                           ruim_sms.ota.data_len );
          }
        }
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        if( wms_nv_write_cdma_sms( i, ruim_data ) != TRUE )
        {
          cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
        }
        else
        {
          cmd_err = wms_msg_cdma_send_mo_channel(
                            TRUE,
                            cmd_ptr,
                            cmd_ptr->cmd.msg_send.send_mode,
                            & cmd_ptr->cmd.msg_send.message.msg_hdr,
                            & cdma_tl,
                            & ruim_sms.ota);
        }
      }

      // Populate and Dispatch Command Callback
      wms_client_cmd_status( cmd_ptr, cmd_err );

      // Populate and Dispatch Send Event Callback
      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
        msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
        msg_event_info.status_info.send_mode  = cmd_ptr->cmd.msg_send.send_mode;
        msg_event_info.status_info.message    = cmd_ptr->cmd.msg_send.message;
        msg_event_info.status_info.alpha_id.len = 0;
        msg_event_info.status_info.alpha_id.data = NULL;
        msg_event_info.status_info.as_id = as_id;
        msg_event_info.status_info.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
        (void)wms_msg_event_notify( WMS_MSG_EVENT_SEND, &msg_event_info );
      }
      break; //WMS_MEMORY_STORE_NV_CDMA
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS

    case WMS_MEMORY_STORE_RAM_GW:
      /* check for a valid message index and status
      */
      cmd_err = wms_msg_map_gw_ram_index(as_id, i, &i);
      if (WMS_CMD_ERR_NONE != cmd_err)
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
      }
      else if(NULL == cfg_s_ptr->ram_gw_sms[i])
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", i);

      }
      else
      {
        wms_message_tag_e_type ram_gw_sms_status = (wms_message_tag_e_type)cfg_s_ptr->ram_gw_sms[i]->status;
        if(!WMS_IS_MO(ram_gw_sms_status))
        {
          cmd_err = WMS_CMD_ERR_MSG_TAG;
        }
      }

      if(cmd_err == WMS_CMD_ERR_NONE)
      {
        /* Fill in preliminary info in the mo record, but don't set it as being
        ** used yet.
        */
        msg_s_ptr->gw_mo_rec[rec_index]->send_mode = WMS_SEND_MODE_MEMORY_STORE;
        msg_s_ptr->gw_mo_rec[rec_index]->user_data = cmd_ptr->hdr.user_data;
        msg_s_ptr->gw_mo_rec[rec_index]->msg_hdr   = cmd_ptr->cmd.msg_send.message.msg_hdr;
        msg_s_ptr->gw_mo_rec[rec_index]->client_id = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
        msg_s_ptr->gw_mo_rec[rec_index]->as_id = as_id;
        msg_s_ptr->gw_mo_rec[rec_index]->appInfo_ptr = appInfo_ptr;
        msg_s_ptr->gw_mo_rec[rec_index]->resend_msg = cmd_ptr->cmd.msg_send.resend_msg;
        msg_s_ptr->gw_mo_rec[rec_index]->client_msg_id = cmd_ptr->cmd.msg_send.client_msg_id;
        msg_s_ptr->gw_mo_rec[rec_index]->transport_type = cmd_ptr->cmd.msg_send.transport_type;

        /* try to send the message */
        cmd_err = wms_msg_send_gw(rec_index, NULL, FALSE);
        if(cmd_err != WMS_CMD_ERR_NONE)
        {
          wms_msg_gw_clear_mo_msg_rec(rec_index);
        }
      }

      // Populate and Dispatch Command Callback
      wms_client_cmd_status( cmd_ptr, cmd_err );

      // Populate and Dispatch Send Event Callback
      if( cmd_err == WMS_CMD_ERR_NONE )
      {
#ifdef FEATURE_GWSMS_MO_CONTROL
        if( msg_s_ptr->gw_mo_rec[rec_index]->mo_control_pending == TRUE )
        {
          /* Don't send event yet; will do it once we received response from card
          */
        }
        else
#endif /* FEATURE_GWSMS_MO_CONTROL */
        {
          msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
          msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
          msg_event_info.status_info.send_mode  = cmd_ptr->cmd.msg_send.send_mode;
          msg_event_info.status_info.message.msg_hdr = cmd_ptr->cmd.msg_send.message.msg_hdr;
          (void)memscpy( &msg_event_info.status_info.message.u.gw_message,
                         sizeof(wms_gw_message_s_type),
                         msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg,
                         sizeof(wms_gw_message_s_type));
          msg_event_info.status_info.alpha_id.len = 0;
          msg_event_info.status_info.alpha_id.data = NULL;
          msg_event_info.status_info.as_id = as_id;
          msg_event_info.status_info.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
          (void)wms_msg_event_notify( WMS_MSG_EVENT_SEND, &msg_event_info );
        }
      }
      break;

    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_SIM:
      if( i >= (uint32) appInfo_ptr->sms_rec_count )
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
      }
      else if(appInfo_ptr->sms_rec_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", i);

      }
      else if( ! WMS_IS_MO ( appInfo_ptr->sms_rec_tags[i] ) )
      {
        cmd_err = WMS_CMD_ERR_MSG_TAG;
      }

      if(cmd_err == WMS_CMD_ERR_NONE)
      {
        /* Fill in preliminary info in the mo record, but don't set it as being
        ** used yet.
        */
        msg_s_ptr->gw_mo_rec[rec_index]->send_mode = WMS_SEND_MODE_MEMORY_STORE;
        msg_s_ptr->gw_mo_rec[rec_index]->user_data = cmd_ptr->hdr.user_data;
        msg_s_ptr->gw_mo_rec[rec_index]->msg_hdr   = cmd_ptr->cmd.msg_send.message.msg_hdr;
        msg_s_ptr->gw_mo_rec[rec_index]->client_id = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
        msg_s_ptr->gw_mo_rec[rec_index]->as_id = as_id;
        msg_s_ptr->gw_mo_rec[rec_index]->appInfo_ptr = appInfo_ptr;
        msg_s_ptr->gw_mo_rec[rec_index]->resend_msg = cmd_ptr->cmd.msg_send.resend_msg;
        msg_s_ptr->gw_mo_rec[rec_index]->client_msg_id = cmd_ptr->cmd.msg_send.client_msg_id;
        msg_s_ptr->gw_mo_rec[rec_index]->transport_type = cmd_ptr->cmd.msg_send.transport_type;

        /* try to send the message */
        cmd_err = wms_msg_send_gw(rec_index, NULL, FALSE);
        if(cmd_err != WMS_CMD_ERR_NONE)
        {
          wms_msg_gw_clear_mo_msg_rec(rec_index);
        }
      }

      // Populate and Dispatch Command Callback
      wms_client_cmd_status( cmd_ptr, cmd_err );

      // Populate and Dispatch Send Event Callback
      if( cmd_err == WMS_CMD_ERR_NONE )
      {
#ifdef FEATURE_GWSMS_MO_CONTROL
        if( msg_s_ptr->gw_mo_rec[rec_index]->mo_control_pending == TRUE )
        {
          /* Don't send event yet; will do it once we received response from card
          */
        }
        else
#endif /* FEATURE_GWSMS_MO_CONTROL */
        {
          msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
          msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
          msg_event_info.status_info.send_mode  = cmd_ptr->cmd.msg_send.send_mode;
          msg_event_info.status_info.message.msg_hdr = cmd_ptr->cmd.msg_send.message.msg_hdr;
          (void)memscpy( &msg_event_info.status_info.message.u.gw_message,
                         sizeof(wms_gw_message_s_type),
                         msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg,
                         sizeof(wms_gw_message_s_type));
          msg_event_info.status_info.alpha_id.len = 0;
          msg_event_info.status_info.alpha_id.data = NULL;
          msg_event_info.status_info.as_id = as_id;
          msg_event_info.status_info.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
          (void)wms_msg_event_notify( WMS_MSG_EVENT_SEND, &msg_event_info );
        }
      }

      break;


    case WMS_MEMORY_STORE_NV_GW:
      /* check for a valid message index and status
      */
      if (wms_msg_map_gw_nv_index(as_id, i, &i) != WMS_CMD_ERR_NONE)
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
      }
      else if(cfg_s_ptr->nv_gw_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", i);

      }
      else if( ! WMS_IS_MO( cfg_s_ptr->nv_gw_tags[i] ) )
      {
        cmd_err = WMS_CMD_ERR_MSG_TAG;
      }

      if(cmd_err == WMS_CMD_ERR_NONE)
      {
        /* Fill in preliminary info in the mo record, but don't set it as being
        ** used yet.
        */
        msg_s_ptr->gw_mo_rec[rec_index]->send_mode = WMS_SEND_MODE_MEMORY_STORE;
        msg_s_ptr->gw_mo_rec[rec_index]->user_data = cmd_ptr->hdr.user_data;
        msg_s_ptr->gw_mo_rec[rec_index]->msg_hdr   = cmd_ptr->cmd.msg_send.message.msg_hdr;
        msg_s_ptr->gw_mo_rec[rec_index]->client_id = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
        msg_s_ptr->gw_mo_rec[rec_index]->as_id = as_id;
        msg_s_ptr->gw_mo_rec[rec_index]->appInfo_ptr = appInfo_ptr;
        msg_s_ptr->gw_mo_rec[rec_index]->resend_msg = cmd_ptr->cmd.msg_send.resend_msg;
        msg_s_ptr->gw_mo_rec[rec_index]->client_msg_id = cmd_ptr->cmd.msg_send.client_msg_id;
        msg_s_ptr->gw_mo_rec[rec_index]->transport_type = cmd_ptr->cmd.msg_send.transport_type;

        /* try to send the message */
        cmd_err = wms_msg_send_gw(rec_index, NULL, FALSE);
        if(cmd_err != WMS_CMD_ERR_NONE)
        {
          wms_msg_gw_clear_mo_msg_rec(rec_index);
        }
      }

      // Populate and Dispatch Command Callback
      wms_client_cmd_status( cmd_ptr, cmd_err );

      // Populate and Dispatch Send Event Callback
      if( cmd_err == WMS_CMD_ERR_NONE )
      {
#ifdef FEATURE_GWSMS_MO_CONTROL
        if( msg_s_ptr->gw_mo_rec[rec_index]->mo_control_pending == TRUE )
        {
          /* Don't send event yet; will do it once we received response from card
          */
        }
        else
#endif /* FEATURE_GWSMS_MO_CONTROL */
        {
          msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
          msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
          msg_event_info.status_info.send_mode  = cmd_ptr->cmd.msg_send.send_mode;
          msg_event_info.status_info.message.msg_hdr = cmd_ptr->cmd.msg_send.message.msg_hdr;
          (void)memscpy( &msg_event_info.status_info.message.u.gw_message,
                         sizeof(wms_gw_message_s_type),
                         msg_s_ptr->gw_mo_rec[rec_index]->sent_gw_msg,
                         sizeof(wms_gw_message_s_type));
          msg_event_info.status_info.alpha_id.len = 0;
          msg_event_info.status_info.alpha_id.data = NULL;
          msg_event_info.status_info.as_id = as_id;
          msg_event_info.status_info.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
          (void)wms_msg_event_notify( WMS_MSG_EVENT_SEND, &msg_event_info );
        }
      }
      break;

#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR_1("Invalid memory store %d", cmd_ptr->cmd.msg_write.message.msg_hdr.mem_store);
#ifdef FEATURE_GWSMS
      wms_msg_gw_clear_mo_msg_rec(rec_index);
#endif /* FEATURE_GWSMS */

      // Populate and Dispatch Command Callback
      wms_client_cmd_status( cmd_ptr, cmd_err );
      break;
  } // switch

  return cmd_err;

} /* wms_msg_send_from_memory_store() */



#ifdef FEATURE_CDSMS
boolean wms_is_card_supports_esn_tracking(wms_client_bd_s_type *cl_bd)
{

  if(cl_bd != NULL)
  {
    if(cl_bd->user_data.data[0]  == 'E' &&
        cl_bd->user_data.data[1] == 'S' &&
        cl_bd->user_data.data[2] == 'N')
    {
      return TRUE;
    }
  }

  MSG_HIGH_0("Not ESN Tracking SMS.");
  return FALSE;
}
/*
*/
boolean wms_msg_cdma_message_id_is_valid(
   wms_message_number_type message_id
)
{
   if (message_id > 65535)
   {
      return FALSE;
   }
   return TRUE;
}

/*
*/
wms_cmd_err_e_type wms_msg_cdma_send_client_message
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type   cmd_err = WMS_CMD_ERR_NONE;
  wms_cdma_message_s_type   * cl_ptr;
  wms_prov_app_info_s_type    *appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    cmd_err =  WMS_CMD_ERR_AS_ID;
    wms_client_cmd_status( cmd_ptr, cmd_err );
    return cmd_err;
  }

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* convert client msg to internal TL format
  */
  cl_ptr = & cmd_ptr->cmd.msg_send.message.u.cdma_message;
  MSG_HIGH_3( "Received CDMA digit_mode = %d, number_type = %d, number_plan = %d",
              cl_ptr->address.digit_mode,
              cl_ptr->address.number_type,
              cl_ptr->address.number_plan );

  wms_ts_convert_cl2tl( cl_ptr, & cdma_tl );

  if( wms_ts_decode_CDMA_bd( & cl_ptr->raw_ts, FALSE, FALSE, & cdma_tl.cl_bd ) != WMS_OK_S )
  {
    cmd_err = WMS_CMD_ERR_MSG_DECODE;
  }
  else if( ! WMS_IS_MO_CDMA_MSG_TYPE( cdma_tl.cl_bd.message_id.type ) )
  {
    cmd_err = WMS_CMD_ERR_MSG_TPDU_TYPE;
  }
  else if( wms_ts_validate_bd_subparms( & cdma_tl.cl_bd, cdma_tl.teleservice ) != WMS_OK_S )
  {
    cmd_err = WMS_CMD_ERR_MSG_DECODE;
  }
  else
  {
    boolean encoding_required = TRUE;

    if (WMS_BD_TYPE_SUBMIT == cdma_tl.cl_bd.message_id.type &&
        TRUE == appInfo_ptr->in_roaming &&
        wms_cm_CT_features_supported( appInfo_ptr->asid ))
    {
      MSG_HIGH_0("Removing Priority and Reply Option");
      cdma_tl.cl_bd.mask &= ~ WMS_MASK_BD_PRIORITY;
      cdma_tl.cl_bd.mask &= ~ WMS_MASK_BD_REPLY_OPTION;
    }

    /* Don't change msg id if it is CANCELLATION or USER_ACK:
    */
    if( cdma_tl.cl_bd.message_id.type == WMS_BD_TYPE_SUBMIT &&
        cdma_tl.teleservice           != WMS_TELESERVICE_WAP )
    {
      if (cmd_ptr->cmd.msg_send.resend_msg)
      {
        /* If resending message, use client-provided message id if valid
        */
        if ( wms_msg_cdma_message_id_is_valid(cmd_ptr->cmd.msg_send.client_msg_id) )
        {
          /* Valid message id from client, use it and reencode
          */
          // encoding_required = TRUE;
          cdma_tl.cl_bd.message_id.id_number = cmd_ptr->cmd.msg_send.client_msg_id;
        }
        else
        {
          /* Invalid message id from client, use the one already encoded in raw data
          */
          encoding_required = FALSE;
        }
      }
      else
      {
        /* use the next message number
        */
        cdma_tl.cl_bd.message_id.id_number = wms_cfg_get_ruim_next_message_number();
      }
    }

    /* use the next sequence number if TL Ack is requested
    */
    if( cdma_tl.mask & WMS_MASK_TL_BEARER_REPLY_OPTION )
    {
      cdma_tl.bearer_reply_seq_num = wms_msg_cdma_next_seq_num();
    }
    else
    {
      cdma_tl.bearer_reply_seq_num = wms_msg_cdma_next_non_tl_seq_num();
    }

    if( WMS_OK_S != wms_msg_get_next_unused_seq_num(cdma_tl.bearer_reply_seq_num,
                                                    &cdma_tl.bearer_reply_seq_num) )
    {
       cmd_err = WMS_CMD_ERR_NO_RESOURCE;
    }

    if (FALSE == wms_msg_convert_plus_address(&cdma_tl.address))
    {
       MSG_ERROR_0("wms_msg_convert_plus_address Failed!");
    }

    /* Encode the message with new info
    */
    cdma_tl.is_mo = TRUE;
    if( wms_ts_encode_CDMA_OTA( & cdma_tl,  & ruim_sms.ota ) != WMS_OK_S )
    {
      cmd_err = WMS_CMD_ERR_MSG_ENCODE;
    }
    /* Update the contents of cmd_ptr w/new message_id */
    else if( (WMS_CDMA_USER_DATA_MAX >= cdma_tl.cl_bd.user_data.number_of_digits) &&
              encoding_required &&
              (wms_ts_encode_bearer_data(&cdma_tl.cl_bd,
                                         &cmd_ptr->cmd.msg_send.message.u.cdma_message.raw_ts) != WMS_OK_S) )
    {
      cmd_err = WMS_CMD_ERR_MSG_ENCODE;
    }
    else
    {
      if (wms_cm_CT_features_supported(wms_get_appinfo_1x_asid())&&(cmd_ptr->hdr.client_id ==  WMS_CLIENT_TYPE_CAT))
      {
        if (wms_is_card_supports_esn_tracking(&cdma_tl.cl_bd))
        {
          cmd_err = WMS_CMD_ERR_NONE;
          MSG_HIGH_0("UTK ESN Tracking detected, Ignoring it.");

          /*Notify Cat client for stk reg successfully to avoid long waiting in STK module,
                          in fact, we did't send the short message to network*/
          msg_event_info.submit_report_info.report_status = WMS_RPT_OK;

          msg_event_info.submit_report_info.as_id         = cmd_ptr->hdr.as_id;
          msg_event_info.submit_report_info.client_id     = WMS_CLIENT_TYPE_CAT;
          msg_event_info.submit_report_info.message_mode  = WMS_MESSAGE_MODE_CDMA;
          msg_event_info.submit_report_info.alpha_id.len = 0;
          msg_event_info.submit_report_info.alpha_id.data = NULL;

          (void)wms_msg_event_notify( WMS_MSG_EVENT_SUBMIT_REPORT, &msg_event_info );

          MSG_HIGH_0("Send response, Ignoring it.");
          return cmd_err;
        }
      }

      cmd_err = wms_msg_cdma_send_mo_channel(
                        TRUE,
                        cmd_ptr,
                        cmd_ptr->cmd.msg_send.send_mode,
                        & cmd_ptr->cmd.msg_send.message.msg_hdr,
                        & cdma_tl,
                        & ruim_sms.ota);
    }
  }

  return cmd_err;

} /* wms_msg_cdma_send_client_message() */
#endif /* FEATURE_CDSMS */

/*
*/
wms_cmd_err_e_type wms_msg_send_client_message
(
  wms_cmd_type    *cmd_ptr,
  uint32          rec_index
)
{
  wms_cmd_err_e_type                cmd_err = WMS_CMD_ERR_MSG_SEND_MODE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_CDSMS
  if( cmd_ptr->cmd.msg_send.message.msg_hdr.message_mode
                   == WMS_MESSAGE_MODE_CDMA )
  {
    cmd_err = wms_msg_cdma_send_client_message( cmd_ptr );
  }
#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
  if( cmd_ptr->cmd.msg_send.message.msg_hdr.message_mode
                   == WMS_MESSAGE_MODE_GW )
  {
    cmd_err = wms_msg_check_tpdu_type(&cmd_ptr->cmd.msg_send.message, FALSE);

    if (cmd_err != WMS_CMD_ERR_NONE)
      return cmd_err;

    /* Fill in preliminary info in the mo record, but don't set it as being
    ** used yet.
    */

    msg_s_ptr->gw_mo_rec[rec_index]->as_id = cmd_ptr->hdr.as_id;
    msg_s_ptr->gw_mo_rec[rec_index]->appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;
    msg_s_ptr->gw_mo_rec[rec_index]->send_mode = WMS_SEND_MODE_CLIENT_MESSAGE;
    msg_s_ptr->gw_mo_rec[rec_index]->user_data = cmd_ptr->hdr.user_data;
    msg_s_ptr->gw_mo_rec[rec_index]->msg_hdr   = cmd_ptr->cmd.msg_send.message.msg_hdr;
    msg_s_ptr->gw_mo_rec[rec_index]->client_id = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
    msg_s_ptr->gw_mo_rec[rec_index]->resend_msg = cmd_ptr->cmd.msg_send.resend_msg;
    msg_s_ptr->gw_mo_rec[rec_index]->client_msg_id = cmd_ptr->cmd.msg_send.client_msg_id;
    msg_s_ptr->gw_mo_rec[rec_index]->transport_type = cmd_ptr->cmd.msg_send.transport_type;

    cmd_err = wms_msg_send_gw(rec_index,
                              &cmd_ptr->cmd.msg_send.message,
                              FALSE);
    if(cmd_err != WMS_CMD_ERR_NONE)
    {
      wms_msg_gw_clear_mo_msg_rec(rec_index);
    }
  }
#endif /* FEATURE_GWSMS */

  return cmd_err;

} /* wms_msg_send_client_message() */




#ifdef FEATURE_GWSMS

boolean wms_msg_get_free_mo_record
(
  uint32   *index_ptr
)
{
  uint32  i;

  if(NULL == index_ptr)
  {
     return FALSE;
  }

  for( i=0; i<WMS_MAX_MO_MESSAGES; i++ )
  {
    if( NULL == msg_s_ptr->gw_mo_rec[i] )
    {
      msg_s_ptr->gw_mo_rec[i] = (wms_msg_gw_mo_record_type *)
                                 wms_mem_malloc(sizeof(wms_msg_gw_mo_record_type));

      if(NULL == msg_s_ptr->gw_mo_rec[i])
      {
         return FALSE;
      }

      *index_ptr = i;
      msg_s_ptr->gw_mo_rec[i]->smi = WMS_MSG_DUMMY_SMI_ID;
#ifdef FEATURE_GWSMS_MO_CONTROL
      msg_s_ptr->gw_mo_rec[i]->mo_control_pending = FALSE;
 #endif
      msg_s_ptr->gw_mo_rec[i]->retry_wait_time = 0;
      msg_s_ptr->gw_mo_rec[i]->retry_count     = 0;

      // If retry logic is disabled, set the retry_time_left to the default value 
      // to prevent WMS from waiting forever if report indication is not received.
      if (0 == msg_s_ptr->mo_retry_period)
      { 
        MSG_HIGH("Retry logic is not enabled. Set MO status wait time to default value!",0,0,0);
        msg_s_ptr->gw_mo_rec[i]->retry_time_left = WMS_MAX_MO_STATUS_WAIT_TIME;
      }
      else 
      {
        msg_s_ptr->gw_mo_rec[i]->retry_time_left = (sint31)msg_s_ptr->mo_retry_period;
      }

      MSG_HIGH("index = %d, retry_time_left = %d,  retry_wait_time = %d", 
                i,
                msg_s_ptr->gw_mo_rec[i]->retry_time_left,
                msg_s_ptr->gw_mo_rec[i]->retry_wait_time );
      return TRUE;
    }
  }

  return FALSE;
}



boolean wms_msg_map_mo_record
(
  sys_modem_as_id_e_type as_id,
  word    smi,
  uint32  *index_ptr
)
{
  uint32  i;

  if(NULL == index_ptr)
  {
     return FALSE;
  }

  for( i=0; i<WMS_MAX_MO_MESSAGES; i++ )
  {
    if( (NULL != msg_s_ptr->gw_mo_rec[i]) &&
        (smi == msg_s_ptr->gw_mo_rec[i]->smi)
        && msg_s_ptr->gw_mo_rec[i]->as_id == as_id
      )
    {
      *index_ptr = i;
      return TRUE;
    }
  }

  return FALSE;

}


boolean wms_msg_get_free_mt_record
(
  uint32   *index_ptr
)
{
  uint32  i;

  if(NULL == index_ptr)
  {
     return FALSE;
  }

  for( i=0; i<WMS_MAX_MT_MESSAGES; i++ )
  {
    if(NULL == msg_s_ptr->gw_mt_rec[i])
    {
       msg_s_ptr->gw_mt_rec[i] = (wms_msg_gw_mt_record_type *)
                                  wms_mem_malloc (sizeof (wms_msg_gw_mt_record_type));

       if(NULL == msg_s_ptr->gw_mt_rec[i])
       {
          return FALSE;
       }

       msg_s_ptr->gw_mt_rec[i]->ack_client_id = WMS_CLIENT_TYPE_MAX;
       *index_ptr = i;
       return TRUE;
    }
  }

  return FALSE;
}

boolean wms_msg_map_mt_record
(
  sys_modem_as_id_e_type        as_id,
  wms_transaction_id_type       tid,
  uint32                        * index_ptr
)
{
  uint32  i;

  if(NULL == index_ptr)
  {
     return FALSE;
  }

  for( i=0; i<WMS_MAX_MT_MESSAGES; i++ )
  {
    if ( (NULL != msg_s_ptr->gw_mt_rec[i]) &&
         (tid == msg_s_ptr->gw_mt_rec[i]->tid)
        && msg_s_ptr->gw_mt_rec[i]->as_id == as_id
        )
    {
      *index_ptr = i;
      return TRUE;
    }
  }

  return FALSE;

}

static boolean wms_msg_map_mt_record_from_smi
(
  sys_modem_as_id_e_type          as_id,
  wms_cm_conn_id_type             smi,
  uint32                        * index_ptr
)
{
  uint32  i;

  if(NULL == index_ptr)
  {
     return FALSE;
  }

  for( i=0; i<WMS_MAX_MT_MESSAGES; i++ )
  {
    if ( (NULL != msg_s_ptr->gw_mt_rec[i]) &&
         (smi == msg_s_ptr->gw_mt_rec[i]->smi)
        && msg_s_ptr->gw_mt_rec[i]->as_id == as_id
      )
    {
      *index_ptr = i;
      return TRUE;
    }
  }

  return FALSE;

}

boolean wms_msg_wait_for_ack_report_ind(
  sys_modem_as_id_e_type as_id
)
{
  uint32  i;

  for (i = 0; i < WMS_MAX_MT_MESSAGES; i++)
  {
    if ((NULL != msg_s_ptr->gw_mt_rec[i]) &&
        (as_id == msg_s_ptr->gw_mt_rec[i]->as_id) &&
        (WMS_CLIENT_TYPE_MAX != msg_s_ptr->gw_mt_rec[i]->ack_client_id))
    {
      MSG_HIGH_0("Found MT record waiting for ack report indication from lower layer.");
      return TRUE;
    }
  }

  MSG_HIGH_0("wms_msg_wait_for_ack_report_ind() returns FALSE.");

  return FALSE;
}

#endif /* FEATURE_GWSMS */

/* This function checks if the Client has the appropriate privileges to Ack the message
   Rules:
   client_id ==  WMS_CLIENT_TYPE_INTERNAL  --> Go Ahead and Ack it
   mt_record_client_id ==  WMS_CLIENT_TYPE_MAX
     If (Primary Client Present) && (client_id = Primary_Client) --> Ack it
     else Ack it.
   client_id ==  mt_record_client_id  --> Ack it
*/
boolean wms_msg_check_valid_client_ack(
  wms_prov_app_info_s_type *appInfo_ptr,
  wms_client_id_type        client_id,
  wms_client_id_type        mt_record_client_id
)
{
  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return TRUE;
  }

  /* Internal Client */
  if (client_id == (wms_client_id_type)WMS_CLIENT_TYPE_INTERNAL)
  {
    return TRUE;
  }

  if (mt_record_client_id != (wms_client_id_type)WMS_CLIENT_TYPE_MAX)
  {
    /* Exclusive Message */
    if (mt_record_client_id == client_id)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    wms_client_id_type primary_client = appInfo_ptr->primary_client;

    /* Common Message */
    if (primary_client != (wms_client_id_type)WMS_CLIENT_TYPE_MAX)
    {
      /* Primary Client Initialized */
      /* Only primary Client can ACK it */
      if (primary_client == client_id)
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      /* Primary Client Not Initialized...Anyone can ACK it */
      return TRUE;
    }
  }
} /* wms_msg_check_valid_client_ack() */

/*
*/
void wms_msg_send_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  uint32                      i = 0;

  if (!wms_check_appinfo_is_valid(cmd_ptr->hdr.appInfo_ptr))
  {
     cmd_err = WMS_CMD_ERR_AS_ID;
     wms_client_cmd_status( cmd_ptr, cmd_err );
     return;
  }

  if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.msg_send.message.msg_hdr.message_mode)
  {
    if (WMS_MSG_TRANSPORT_TYPE_INTERNAL == cmd_ptr->cmd.msg_send.transport_type)
    {
      wms_msg_transport_s_type* transport_ptr = wms_msg_choose_registered_transport( cmd_ptr->hdr.as_id );
      if (NULL != transport_ptr && transport_ptr->is_mn_registered)
      {
        cmd_ptr->cmd.msg_send.transport_type = WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS;
      }
      else
      {
        cmd_ptr->cmd.msg_send.transport_type = WMS_MSG_TRANSPORT_TYPE_CS;
      }
    }
    
    // Allow MO SMS over IMS even in LPM mode.
    if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS != cmd_ptr->cmd.msg_send.transport_type &&
        cfg_s_ptr->lpm)
    {
      cmd_err = WMS_CMD_ERR_MSG_LPM;
      wms_client_cmd_status( cmd_ptr, cmd_err );
      return;
    }
  }

  /*
  if (!cfg_s_ptr->srv_available)
  {
    MSG_HIGH_0("Service Not Available");
    cmd_err = WMS_CMD_ERR_NO_NETWORK;
    wms_client_cmd_status( cmd_ptr, cmd_err );
    return;
  }
  */

#ifdef FEATURE_GWSMS
  if( (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.msg_send.message.msg_hdr.message_mode) &&
       (FALSE == wms_msg_get_free_mo_record( & i ))  )
  {
    MSG_ERROR_0("No Free MO Records Available");
    cmd_err = WMS_CMD_ERR_NO_RESOURCE;
    wms_client_cmd_status( cmd_ptr, cmd_err );
    return;
  }
#endif /* FEATURE_GWSMS */

   /* If the SMS is over IMS and retry period is Zero, set the retry_time_left to Timerf value */
     
#ifdef FEATURE_GWSMS
  if (0 == msg_s_ptr->mo_retry_period)
  { 
     if((WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == cmd_ptr->cmd.msg_send.transport_type)&&
         (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.msg_send.message.msg_hdr.message_mode))
     {
        if(msg_s_ptr->gw_mo_rec[i] != NULL)
        {         
          msg_s_ptr->gw_mo_rec[i]->retry_time_left = ((msg_s_ptr->timerf_value)/1000)+ 1;
          MSG_HIGH_1("Retry logic is not enabled. Set MO status wait time to timerf value= %d as the SMS is over IMS!",msg_s_ptr->gw_mo_rec[i]->retry_time_left );
        }
     }
  }
#endif /* FEATURE_GWSMS */

  if( cmd_ptr->cmd.msg_send.send_mode == WMS_SEND_MODE_MEMORY_STORE )
  {
    if (wmsi_msg_check_for_validity(cmd_ptr->cmd.msg_send.message.msg_hdr.message_mode,
                                    cmd_ptr->cmd.msg_send.message.msg_hdr.mem_store))
    {
       cmd_err = wms_msg_send_from_memory_store( cmd_ptr, i );
    }
    else
    {
       MSG_ERROR_2( "Invalid mem store %d for message_mode %d",
                    cmd_ptr->cmd.msg_send.message.msg_hdr.message_mode,
                    cmd_ptr->cmd.msg_send.send_mode);

       cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
    }
    wms_client_cmd_status( cmd_ptr, cmd_err );
  }
  else if( cmd_ptr->cmd.msg_send.send_mode == WMS_SEND_MODE_CLIENT_MESSAGE )
  {
    cmd_err = wms_msg_send_client_message( cmd_ptr, i );
    wms_client_cmd_status( cmd_ptr, cmd_err );
    if( cmd_err == WMS_CMD_ERR_NONE )
    {
#ifdef FEATURE_GWSMS_MO_CONTROL
      if( msg_s_ptr->gw_mo_rec[i] != NULL &&
          msg_s_ptr->gw_mo_rec[i]->mo_control_pending == TRUE )
      {
        /* Don't send event yet; will do it once we received response from card
        */
      }
      else
#endif /* FEATURE_GWSMS_MO_CONTROL */
      {
        msg_event_info.status_info.as_id = cmd_ptr->hdr.as_id;
        msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
        msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
        msg_event_info.status_info.send_mode  = cmd_ptr->cmd.msg_send.send_mode;
        msg_event_info.status_info.message    = cmd_ptr->cmd.msg_send.message;
        msg_event_info.status_info.alpha_id.len = 0;
        msg_event_info.status_info.alpha_id.data = NULL;
        msg_event_info.status_info.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
        (void)wms_msg_event_notify( WMS_MSG_EVENT_SEND, &msg_event_info );
      }
    }
  }
  else
  {
    cmd_err = WMS_CMD_ERR_MSG_SEND_MODE;
    wms_client_cmd_status( cmd_ptr, cmd_err );
  }

  return;
} /* wms_msg_send_proc() */



#ifdef FEATURE_CDSMS

/*===========================================================================
FUNCTION   wms_msg_awi_ack

DESCRIPTION
  Send an AWI SMS ack message.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void wms_msg_awi_ack
(
  boolean                   final_ack,
  boolean                   bf_bit,
  wms_error_class_e_type    error_class,
  wms_cdma_tl_status_e_type      tl_status
)
{
#ifdef FEATURE_ACP
  if( error_class == WMS_ERROR_NONE )
  {
    tl_status = 0x00;
  }

  wms_cm_awi_ack( msg_s_ptr->awi_info.seq_no,
                  bf_bit,
                  error_class,
                  tl_status );

  if( final_ack )
  {
    /* Reset the count after msg is acknowledged as final ack
    */
    msg_s_ptr->awi_info.cnt    = 0;
    msg_s_ptr->awi_info.seq_no = 0;
  }
#else /* FEATURE_ACP */
  (void)final_ack;
  (void)bf_bit;
  (void)error_class;
  (void)tl_status;
#endif /* FEATURE_ACP */

  return;
} /* wms_msg_awi_ack() */



wms_cmd_err_e_type wms_msg_cdma_ack_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type    cmd_err = WMS_CMD_ERR_NONE;
  boolean               record_found;
  uint32                i;
  wms_ack_info_s_type   *ack_ptr = & cmd_ptr->cmd.msg_ack.ack_info;
  wms_msg_cdma_message_record_type  *rec_ptr = NULL;
  wms_status_e_type     st = WMS_OK_S;

  MSG_HIGH_2( "wms_msg_cdma_ack_proc(): client_id = %d,  tid = %d",
              cmd_ptr->hdr.client_id,
              ack_ptr->transaction_id );

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_ACP
  /* AWI SMS ack processing
  */
  if( msg_s_ptr->awi_info.is_valid &&
      msg_s_ptr->awi_info.user_data == (void*) ack_ptr->transaction_id )
  {
    /* Got a valid Ack; Reset the awi info
    */
    (void) rex_clr_timer( & msg_s_ptr->awi_info.timer );
    (void) rex_clr_sigs( rex_self(), WMS_AWISMS_ACK_TIMER_SIG );
    msg_s_ptr->awi_info.is_valid = FALSE;

    /* send awi ack order
    */
    wms_msg_awi_ack( TRUE,
                     TRUE,
                     ack_ptr->u.cdma.error_class,
                     ack_ptr->u.cdma.tl_status );

    /* done with AWISMS ack processing
    */
    return cmd_err;  /* SHORT-BREAK */
  }
#endif /* FEATURE_ACP */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* CDMA Ack processing
  */

  /* search the mt_q for the matching tid
  */
  record_found = FALSE;

  for( i=0; i < WMS_MAX_MT_MESSAGES; i++ )
  {
    if( NULL != msg_s_ptr->cdma_mt_rec[i]  &&
        msg_s_ptr->cdma_mt_rec[i]->user_data == (void*) (ack_ptr->transaction_id) )
    {
      record_found = TRUE;
      rec_ptr = msg_s_ptr->cdma_mt_rec[i];
      break;
    }
  }
#ifdef FEATURE_FUZZ_TEST

  if( record_found == FALSE || ack_ptr->transaction_id == WMS_DBG_TRANSACTION_ID )
#else /* FEATURE_FUZZ_TEST */
  if( record_found == FALSE )
#endif /* FEATURE_FUZZ_TEST */
  {
    MSG_HIGH_1( "Invalid TID 0x%lx!", ack_ptr->transaction_id);
    cmd_err = WMS_CMD_ERR_MSG_TRANSACTION_ID;
  }
  else if (!wms_msg_check_valid_client_ack(cmd_ptr->hdr.appInfo_ptr,
                                           cmd_ptr->hdr.client_id, (wms_client_id_type)rec_ptr->client_id ))
  {
    MSG_HIGH_1("Unprivileged Client %d Acking the Message", cmd_ptr->hdr.client_id);
    cmd_err = WMS_CMD_ERR_CLIENT_ID;
  }
  else if( rec_ptr->state == WMS_MSG_STATE_WAIT_FOR_ACK )
  {
    rec_ptr->state = WMS_MSG_STATE_ACK_IN_PROGRESS;
    /* add the user data to the mt rec ptr */
    rec_ptr->mt_rec_user_data = cmd_ptr->hdr.user_data;

    if ( cfg_s_ptr->active_bearers[WMS_BEARER_LTE] && cfg_s_ptr->sms_over_s102 )
    {
      rec_ptr->transport_type   = WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102;
    }
    else 
    {
      rec_ptr->transport_type   = cmd_ptr->cmd.msg_ack.transport_type;
    }

    MSG_HIGH_3( "ack msg: found tid=%d, seq_num=%d, chosen transport %d",
                ack_ptr->transaction_id,
                rec_ptr->seq_num,
                rec_ptr->transport_type);

    // Allow Ack over IMS even in LPM Mode.	
    if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS != rec_ptr->transport_type &&
        cfg_s_ptr->lpm )
    {
      return WMS_CMD_ERR_MSG_LPM;
    }

    if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS != rec_ptr->transport_type &&
        FALSE == wms_check_appinfo_is_valid(cmd_ptr->hdr.appInfo_ptr))
    {
      return WMS_CMD_ERR_AS_ID;
    }

    /* clear the mt ack timer
    */
    (void) rex_clr_timer( & rec_ptr->timer );

    /* Copy Ack Data to Record */
    rec_ptr->error_class     = ack_ptr->u.cdma.error_class;
    rec_ptr->tl_status       = ack_ptr->u.cdma.tl_status;

    /* Set the retry_time_left  - Irrespective of Retry Period = 0 */
    if (rec_ptr->esn_present)
    {
      rec_ptr->retry_time_left = WMS_MAX_ACK_RETRY_PERIOD_JCDMA;
    }
    else
    {
      rec_ptr->retry_time_left = WMS_MAX_ACK_RETRY_PERIOD;
    }
    rec_ptr->retry_wait_time = 0;

    /* Assign Client ID of Acking Client */
    rec_ptr->client_id  = (wms_client_type_e_type)cmd_ptr->hdr.client_id;

    /* Set channel pref for the acking client */
    rec_ptr->ch_pref = cmd_ptr->cmd.msg_ack.ch_pref;

    /* Set the Global Retry Timer */
    wms_set_retry_timer();

    st = wms_msg_cdma_ack_msg_i( rec_ptr,
                                 ack_ptr->u.cdma.error_class,
                                 ack_ptr->u.cdma.tl_status );

    if( st == WMS_OK_S )
    {
      MSG_HIGH_0("ack msg: L2 timer set");
      rec_ptr->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
    }
    else if( st == WMS_NETWORK_NOT_READY_S )
    {
      rec_ptr->retry_wait_time = 1; /* Retry after 1 second */
      rec_ptr->state           = WMS_MSG_STATE_QUEUED;
    }
    else if( st == WMS_1X_AND_VOICE_NOT_ALLOWED_S )
    {
      MSG_HIGH_0("cannot ack cilent msg while voice call is active");
      cmd_err = WMS_CMD_ERR_1X_AND_VOICE_NOT_ALLOWED;
    }
    else if( st == WMS_TRANSPORT_NOT_READY_S )
    {
      cmd_err = WMS_CMD_ERR_TRANSPORT_NOT_READY;
    }
    else if( st == WMS_TRANSPORT_NOT_ALLOWED_S )
    {
      cmd_err = WMS_CMD_ERR_TRANSPORT_NOT_ALLOWED;
    }
    else if ( WMS_MO_PREF_CHN_UNSUPPORTED_S == st )
    {
      cmd_err = WMS_CMD_ERR_PREF_CHN_UNSUPPORTED_S;
    }
    else if ( WMS_IMS_NOT_ALLOWED_IN_SRLTE_S == st )
    {
      cmd_err = WMS_CMD_ERR_IMS_NOT_ALLOWED_IN_SRLTE; 
    }
    else /* other error */
    {
      MSG_HIGH_1("ack msg error: st=%d", st);
      cmd_err = WMS_CMD_ERR_BUSY;
    } /* if st - else */

    if ( WMS_CMD_ERR_NONE != cmd_err )
    {
      wms_msg_cdma_free_mt_msg_rec( rec_ptr );
    }
  }
  else
  {
    MSG_ERROR_1("Unexpected msg state %d", rec_ptr->state);
    cmd_err = WMS_CMD_ERR_MSG_TRANSACTION_ID;
  }

  /* done */
  return cmd_err;
} /* wms_msg_cdma_ack_proc() */

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS

/*===========================================================================
FUNCTION wms_msg_sim_memory_full

DESCRIPTION
  Function to check if SIM Memory Store is full.

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_msg_sim_memory_full(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  uint32 i;
  boolean mem_is_full = TRUE;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return FALSE;
  }

  /* Check for an empty SMS record on SIM:
     1. Returns error: SIM storage is full
     2. Else, SIM storage is not full. */
  if (WMS_CMD_ERR_NONE == wms_msg_find_free_sms_rec_index(appInfo_ptr, &i) )
  {
    mem_is_full = FALSE;
  }

  MSG_HIGH_1("wms_msg_sim_memory_full return=%d", mem_is_full);
  return mem_is_full;
}

/*===========================================================================
FUNCTION wms_msg_nv_gw_memory_full

DESCRIPTION
  Function to check if NV GW Memory Store is full.

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_msg_nv_gw_memory_full(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  boolean mem_is_full = TRUE;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return FALSE;
  }

  /* First check for client's memory, otherwise return storage capacity status of NV */
  if ( (appInfo_ptr->primary_client != (wms_client_id_type)WMS_CLIENT_TYPE_MAX) &&
        appInfo_ptr->use_client_memory )
  {
    mem_is_full = appInfo_ptr->client_memory_full;
  }
  else if ( WMS_CMD_ERR_NONE == wms_msg_find_free_gw_nv_index(appInfo_ptr->asid, NULL, NULL) )
  {
    mem_is_full = FALSE;
  }

  MSG_HIGH_1("wms_msg_nv_gw_memory_full return=%d", mem_is_full);

  return mem_is_full;
} /* wms_msg_nv_gw_memory_full() */
#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION wms_msg_total_memory_full

DESCRIPTION
  Function to check if all the Memory Stores are full.

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_msg_total_memory_full(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
   boolean mem_is_full = FALSE;

   if (!wms_check_appinfo_is_valid(appInfo_ptr))
   {
     return FALSE;
   }

   /* Memory is considered full when:
      1. Primary client is set and uses its memory:
          Both SIM and client memory are full.
      2. Else:
          Both SIM and NV storage are full. */
#ifdef FEATURE_GWSMS

   if ( wms_msg_sim_memory_full(appInfo_ptr) )
   {
     mem_is_full = wms_msg_nv_gw_memory_full(appInfo_ptr);
   }

#else /* FEATURE_GWSMS */
   mem_is_full = TRUE;
#endif /* FEATURE_GWSMS */
   return mem_is_full;
} /* wms_msg_total_memory_full() */

#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_msg_gw_deliver_report_ack

DESCRIPTION
  Send a successful acknowledgement to a message.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static wms_cmd_err_e_type wms_msg_gw_deliver_report_ack
(
   wms_ack_info_s_type       *ack_ptr,       /* ack information */
   uint32                    rec_index,      /* identifies the message */
   wms_client_type_e_type    ack_client_id,  /* acking client id */
   void                      *user_data      /* user data to save. */
)
{
   wms_cmd_err_e_type  cmd_err = WMS_CMD_ERR_NONE;

   if ( WMS_MSG_STATE_WAIT_FOR_ACK == msg_s_ptr->gw_mt_rec[rec_index]->state )
   {
      /* Send Deliver-Report-Ack
      */
      msg_s_ptr->gw_mt_rec[rec_index]->state = WMS_MSG_STATE_ACK_IN_PROGRESS;
      if (ack_ptr->u.gw.tpdu_type == WMS_TPDU_NONE)
      {
        raw_ts.len =0;
      }
      else if ( wms_ts_encode_deliver_report_ack( & ack_ptr->u.gw.u.ack,
                                                  & raw_ts ) != WMS_OK_S )
      {
         MSG_ERROR_0("wms_ts_encode_deliver_report_ack() failed");
         cmd_err = WMS_CMD_ERR_MSG_ENCODE;
      }

      if ( WMS_CMD_ERR_NONE == cmd_err )
      {
         msg_s_ptr->gw_mt_rec[rec_index]->ack_client_id = ack_client_id;
         msg_s_ptr->gw_mt_rec[rec_index]->user_data = user_data;
         cmd_err = wms_cm_gw_send_rl_report(msg_s_ptr->gw_mt_rec[rec_index]->as_id,
                                            msg_s_ptr->gw_mt_rec[rec_index]->smi,
                                            TRUE,
                                            0, /* rp_cause_value ignored on Success */
                                            raw_ts.data,
                                            (uint8)raw_ts.len);
         /* MT record will be freed if the ack report indication from N/W arrived before
            wms_cm_gw_send_rl_report() returns.  So check for MT record NULL is needed.
         */
         if ( (NULL != msg_s_ptr->gw_mt_rec[rec_index]) && (WMS_CMD_ERR_NONE == cmd_err) )
         {
            msg_s_ptr->gw_mt_rec[rec_index]->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
         }
      }
   }
   else
   {
      MSG_ERROR_1("Unexpected msg state %d", msg_s_ptr->gw_mt_rec[rec_index]->state);
      cmd_err = WMS_CMD_ERR_MSG_TRANSACTION_ID;
   }

   return cmd_err;
}

/*===========================================================================
FUNCTION wms_msg_gw_deliver_report_error

DESCRIPTION
  Send a negative acknowledgement to a message.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static wms_cmd_err_e_type wms_msg_gw_deliver_report_error(
   wms_ack_info_s_type       *ack_ptr,       /* ack information */
   uint32                     rec_index,     /* identifies the message */
   wms_client_type_e_type     ack_client_id, /* acking client id */
   void                      *user_data      /* user data to save. */
)
{
   uint8                     rp_cause_value;
   sys_modem_as_id_e_type    as_id;
   wms_prov_app_info_s_type *appInfo_ptr;
   wms_cmd_err_e_type        cmd_err = WMS_CMD_ERR_NONE;

   if (rec_index < WMS_MAX_MT_MESSAGES)
   {
      as_id       = msg_s_ptr->gw_mt_rec[rec_index]->as_id;
      appInfo_ptr = msg_s_ptr->gw_mt_rec[rec_index]->appInfo_ptr;
   }
   else
   {
      MSG_ERROR_1("Invalid rec_index", rec_index);
      return WMS_CMD_ERR_MSG_INDEX;
   }

   if ( WMS_MSG_STATE_WAIT_FOR_ACK == msg_s_ptr->gw_mt_rec[rec_index]->state )
   {
      /* Send Deliver-Report-Error
      */
      msg_s_ptr->gw_mt_rec[rec_index]->state = WMS_MSG_STATE_ACK_IN_PROGRESS;
      if (ack_ptr->u.gw.tpdu_type == WMS_TPDU_NONE)
      {
        raw_ts.len =0;
      }
      else if ( wms_ts_encode_deliver_report_error( & ack_ptr->u.gw.u.error,
                                                    & raw_ts ) != WMS_OK_S )
      {
         MSG_ERROR_0("wms_ts_encode_deliver_report_error() failed");
         cmd_err = WMS_CMD_ERR_MSG_ENCODE;
      }

      if ( WMS_CMD_ERR_NONE == cmd_err )
      {
         msg_s_ptr->gw_mt_rec[rec_index]->ack_client_id = ack_client_id;
         msg_s_ptr->gw_mt_rec[rec_index]->user_data = user_data;

         rp_cause_value = (uint8)ack_ptr->u.gw.rp_cause;

         /* Special handling for a negative Ack from a client with Memory Full cause:
            1. If any external client acks with Memory Full cause and the memory is
               full (according to following definition), send Memory Full to network:
               a. Primary client is set and uses its memory:
                     Both client and SIM memory should be full.
               b. Else
                     Both SIM and NV storage should be full.
            2. Else memory is available and external client gave incorrect information.
               Send temporary Unspecified Error to network. */
         if ((ack_client_id != WMS_CLIENT_TYPE_INTERNAL) &&
             (rp_cause_value == (uint8)WMS_RP_CAUSE_MEMORY_EXCEEDED))
         {
            wms_tp_cause_e_type   tp_cause;

            if (wms_msg_total_memory_full(appInfo_ptr))
            {
               /* Send Memory Full to the Network */
               rp_cause_value = (uint8)WMS_RP_CAUSE_MEMORY_EXCEEDED;
               tp_cause = WMS_TP_CAUSE_MEMORY_FULL;

               // If the MS notifies the network that the MS has been unable to accept a 
               // short message because its memory capacity has been exceeded, 
               // then the ME shall set the memory capacity Exceeded Notification flag if present
               appInfo_ptr->smss_mem_full_flag &= (uint8)WMS_SIM_MEM_FULL_SET;
              (void)wms_cfg_get_sim_next_message_number( appInfo_ptr );
            }
            else
            {
               /* Send Temporary unspecified Error to the Network */
               rp_cause_value = (uint8)WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED;
               tp_cause = WMS_TP_CAUSE_UNSPECIFIED_ERROR;
            }

            if (raw_ts.len == 0)
            {
               /* No TPDU present; hence add the Deliver Report TPDU specifying the TP Cause */
               raw_ts.len = 3;
               raw_ts.data[0] = 0; /* Deliver-Report */
               raw_ts.data[1] = (uint8) tp_cause;
               raw_ts.data[2] = 0;
            }
         }

         cmd_err = wms_cm_gw_send_rl_report(as_id,
                                             msg_s_ptr->gw_mt_rec[rec_index]->smi,
                                             FALSE,
                                             rp_cause_value,
                                             raw_ts.data,
                                             (uint8)raw_ts.len);
         /* MT record will be freed if the ack report indication from N/W arrived before
            wms_cm_gw_send_rl_report() returns.  So check for the MT record NULL is needed.
         */
         if ( (NULL != msg_s_ptr->gw_mt_rec[rec_index]) && (WMS_CMD_ERR_NONE == cmd_err) )
         {
            msg_s_ptr->gw_mt_rec[rec_index]->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
         }
      }
   }
   else
   {
      MSG_ERROR_1("Unexpected msg state %d", msg_s_ptr->gw_mt_rec[rec_index]->state);
      cmd_err = WMS_CMD_ERR_MSG_TRANSACTION_ID;
   }
   return cmd_err;
}
#endif /* FEATURE_GWSMS */

static wms_cmd_err_e_type wms_msg_do_client_ack
(
  wms_cmd_type          * cmd_ptr
)
{
  wms_cmd_err_e_type        cmd_err = WMS_CMD_ERR_MSG_SEND_MODE;
  wms_ack_info_s_type       *ack_ptr = NULL;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (NULL == cmd_ptr)
  {
    MSG_ERROR_0("Null cmd pointer");
    cmd_err = WMS_CMD_ERR_NULL_PTR;
    return cmd_err;
  }

  ack_ptr = &cmd_ptr->cmd.msg_ack.ack_info;

#ifdef FEATURE_CDSMS

  if( ack_ptr->message_mode == WMS_MESSAGE_MODE_CDMA )
  {
    cmd_err = wms_msg_cdma_ack_proc( cmd_ptr );
  }

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS

  if( ack_ptr->message_mode == WMS_MESSAGE_MODE_GW )
  {
    uint32 i;

    /* Process ack from the client
    */
    if (FALSE == wms_check_appinfo_is_valid(cmd_ptr->hdr.appInfo_ptr))
    {
      cmd_err = WMS_CMD_ERR_AS_ID;
    }
    else if (wms_msg_map_mt_record( cmd_ptr->hdr.as_id,
                                    ack_ptr->transaction_id, & i ) == FALSE)
    {
      MSG_HIGH_1( "Invalid TID 0x%lx!", ack_ptr->transaction_id);
      cmd_err = WMS_CMD_ERR_MSG_TRANSACTION_ID;
    }
    else if (!wms_msg_check_valid_client_ack(cmd_ptr->hdr.appInfo_ptr,
                                             cmd_ptr->hdr.client_id, msg_s_ptr->gw_mt_rec[i]->client_id))
    {
      MSG_HIGH_1("Unprivileged Client %d Acking the Message", cmd_ptr->hdr.client_id);
      cmd_err = WMS_CMD_ERR_CLIENT_ID;
    }
    else if (ack_ptr->u.gw.success == TRUE)
    {
       msg_s_ptr->gw_mt_rec[i]->transport_type = cmd_ptr->cmd.msg_ack.transport_type;
       cmd_err = wms_msg_gw_deliver_report_ack(ack_ptr,
                                               i,
                                               (wms_client_type_e_type)cmd_ptr->hdr.client_id,
                                               cmd_ptr->hdr.user_data );
    }
    else
    {
       msg_s_ptr->gw_mt_rec[i]->transport_type = cmd_ptr->cmd.msg_ack.transport_type;
       cmd_err = wms_msg_gw_deliver_report_error(ack_ptr,
                                                 i,
                                                 (wms_client_type_e_type)cmd_ptr->hdr.client_id,
                                                 cmd_ptr->hdr.user_data);
    }
  }

#endif /* FEATURE_GWSMS */

  return cmd_err;
} /* wms_msg_do_client_ack() */


/*
*/
void wms_msg_ack_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type    cmd_err = WMS_CMD_ERR_NONE;

  if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.msg_ack.ack_info.message_mode)
  {
    // Allow Ack over IMS even in LPM Mode.
    if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS != cmd_ptr->cmd.msg_ack.transport_type &&
        cfg_s_ptr->lpm )
    {
      cmd_err = WMS_CMD_ERR_MSG_LPM;
    }
  }
  /*
  else if(FALSE == cfg_s_ptr->srv_available)
  {
    MSG_HIGH_0("Service Not Available");
    cmd_err = WMS_CMD_ERR_NO_NETWORK;
  }
  */
  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    cmd_err = wms_msg_do_client_ack( cmd_ptr );
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  /* If the client is INTERNAL, don't send any events to clients */
  if( cmd_err == WMS_CMD_ERR_NONE &&
      cmd_ptr->hdr.client_id != (uint8) WMS_CLIENT_TYPE_INTERNAL )
  {
    msg_event_info.ack_info = cmd_ptr->cmd.msg_ack.ack_info;
    msg_event_info.ack_info.as_id = cmd_ptr->hdr.as_id;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_ACK, &msg_event_info );
  }

  return;
} /* wms_msg_ack_proc() */


/*
*/
void wms_msg_read_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  uint32                      i;
  wms_memory_store_e_type     mem_store;
  wms_prov_app_info_s_type   *appInfo_ptr;
  sys_modem_as_id_e_type      as_id;

  as_id       = cmd_ptr->hdr.as_id;
  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     cmd_err =  WMS_CMD_ERR_AS_ID;
     wms_client_cmd_status( cmd_ptr, cmd_err );
     return;
  }

  i = cmd_ptr->cmd.msg_read.index;

  mem_store = cmd_ptr->cmd.msg_read.mem_store;

  switch (mem_store)
  {

#ifdef FEATURE_CDSMS
    case WMS_MEMORY_STORE_CSIM:
    case WMS_MEMORY_STORE_RUIM:
      if (!wms_sim_mmgsdi_mem_store_supported(appInfo_ptr->mmgsdi_appInfo.app_type, mem_store))
      {
         cmd_err =  WMS_CMD_ERR_UNSUPPORTED;
         break;
      }
      else if ( i >= (uint32) appInfo_ptr->sms_rec_count )
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(cmd_ptr->cmd.msg_read.index);
      }
      else if(appInfo_ptr->sms_rec_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", cmd_ptr->cmd.msg_read.index);
      }
      else if( wms_sim_mmgsdi_read_data( appInfo_ptr,
                                         appInfo_ptr->sms_file_enum,
                                         (mmgsdi_rec_num_type)i,
                                         appInfo_ptr->sms_rec_len,
                                         ruim_data,
                                         sizeof(ruim_data)) != MMGSDI_SUCCESS)
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_READ;
      }
      else if( wms_ts_cdma_unpack_ruim_sms( ruim_data , & ruim_sms ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_DECODE;
      }
      else if( wms_ts_decode_CDMA_tl( & ruim_sms.ota, & cdma_tl, & cdma_raw_bd )
               != WMS_OK_S )
      {
        cmd_err = WMS_CMD_ERR_MSG_DECODE;
      }
      else
      {
        wms_ts_convert_tl2cl( & cdma_tl, & cdma_cl );
        cdma_cl.raw_ts = cdma_raw_bd;

        msg_event_info.status_info.message.msg_hdr.message_mode =
                                                         WMS_MESSAGE_MODE_CDMA;
        msg_event_info.status_info.message.msg_hdr.index     = i;
        msg_event_info.status_info.message.msg_hdr.mem_store = mem_store;
        msg_event_info.status_info.message.msg_hdr.tag       = (wms_message_tag_e_type)ruim_sms.status;
        msg_event_info.status_info.message.u.cdma_message    = cdma_cl;
      }
      break;

    case WMS_MEMORY_STORE_NV_CDMA:
      if  (i >= cfg_s_ptr->nv_cdma_max_slots)
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(cmd_ptr->cmd.msg_read.index);
        break;
      }
      if(cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", cmd_ptr->cmd.msg_read.index);
        break;
      }

      if( wms_nv_read_cdma_sms( i, ruim_data ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_READ;
      }
      else
      {
        /* Layout of SMS message in NV. */
        /* 0: format                    */
        /* 1: tag/status                */
        /* 2: len                       */
        /* 3+: data                     */
        ruim_sms.ota.format   = (wms_format_e_type) ruim_data[0];
        ruim_sms.status       = ruim_data[1];
        ruim_sms.ota.data_len = ruim_data[2];
        (void)memscpy( (void *)ruim_sms.ota.data,
                       ruim_sms.ota.data_len,
                       ruim_data + 3,
                       ruim_sms.ota.data_len );

        if( wms_ts_decode_CDMA_tl( & ruim_sms.ota, & cdma_tl, & cdma_raw_bd )
            == WMS_OK_S )
        {
          wms_ts_convert_tl2cl( & cdma_tl, & cdma_cl );
          cdma_cl.raw_ts = cdma_raw_bd;

          msg_event_info.status_info.message.msg_hdr.message_mode =
                                                        WMS_MESSAGE_MODE_CDMA;
          msg_event_info.status_info.message.msg_hdr.index      = i;
          msg_event_info.status_info.message.msg_hdr.mem_store  = mem_store;
          msg_event_info.status_info.message.msg_hdr.tag = (wms_message_tag_e_type) (ruim_sms.status & 0x7);
          msg_event_info.status_info.message.u.cdma_message     = cdma_cl;
        }
        else
        {
          cmd_err = WMS_CMD_ERR_MSG_DECODE;
        }
      }
      break;

#endif /* FEATURE_CDSMS */



#ifdef FEATURE_GWSMS

    case WMS_MEMORY_STORE_RAM_GW:
      cmd_err = wms_msg_map_gw_ram_index(as_id, i, &i);

      if (WMS_CMD_ERR_NONE != cmd_err)
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(cmd_ptr->cmd.msg_read.index);
        break;
      }
      if (NULL == cfg_s_ptr->ram_gw_sms[i])
      {
        cmd_err = WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", cmd_ptr->cmd.msg_read.index);
        break;
      }
      (void)wms_ts_unpack_sim_sms(cfg_s_ptr->ram_gw_sms[i],
                               & sim_sms_unpacked );

      msg_event_info.status_info.message.msg_hdr.message_mode =
                                                         WMS_MESSAGE_MODE_GW;
      msg_event_info.status_info.message.msg_hdr.index = cmd_ptr->cmd.msg_read.index;
      msg_event_info.status_info.message.msg_hdr.mem_store = mem_store;
      msg_event_info.status_info.message.msg_hdr.tag =
                              (wms_message_tag_e_type) (cfg_s_ptr->ram_gw_sms[i]->status & 0x7);
      msg_event_info.status_info.message.u.gw_message.is_broadcast = FALSE;
      msg_event_info.status_info.message.u.gw_message.sc_address
                = sim_sms_unpacked.sc_address;
      msg_event_info.status_info.message.u.gw_message.raw_ts_data
                = sim_sms_unpacked.raw_ts_data;
      break;


    case WMS_MEMORY_STORE_SIM:
    case WMS_MEMORY_STORE_USIM:
      if (!wms_sim_mmgsdi_mem_store_supported(appInfo_ptr->mmgsdi_appInfo.app_type, mem_store))
      {
         cmd_err =  WMS_CMD_ERR_UNSUPPORTED;
         break;
      }
      if ( i >= (uint32) appInfo_ptr->sms_rec_count )
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(cmd_ptr->cmd.msg_read.index);
      }
      else if(appInfo_ptr->sms_rec_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", cmd_ptr->cmd.msg_read.index);
      }
      else if( wms_sim_mmgsdi_read_data( appInfo_ptr,
                                         appInfo_ptr->sms_file_enum,
                                         (mmgsdi_rec_num_type)i,
                                         appInfo_ptr->sms_rec_len,
                                         sim_data,
                                         sizeof(sim_data)) != MMGSDI_SUCCESS)
      {
        cmd_err = WMS_CMD_ERR_MSG_SIM_READ;
      }
      else
      {
        sim_sms.status = sim_data[0];
        sim_sms.len = (uint8) MIN(appInfo_ptr->sms_rec_len-1, WMS_SIM_SMS_MAX-1);

        (void)memscpy( (void *)sim_sms.data,
                       sim_sms.len,
                       sim_data+1,
                       sim_sms.len );

        (void)wms_ts_unpack_sim_sms( & sim_sms,
                                 & sim_sms_unpacked );

        msg_event_info.status_info.message.msg_hdr.message_mode =
                                                         WMS_MESSAGE_MODE_GW;
        msg_event_info.status_info.message.msg_hdr.index = i;
        msg_event_info.status_info.message.msg_hdr.mem_store = mem_store;
        msg_event_info.status_info.message.msg_hdr.tag = (wms_message_tag_e_type) (sim_data[0] & 0x7);
        msg_event_info.status_info.message.u.gw_message.is_broadcast = FALSE;
        msg_event_info.status_info.message.u.gw_message.sc_address
                = sim_sms_unpacked.sc_address;
        msg_event_info.status_info.message.u.gw_message.raw_ts_data
                = sim_sms_unpacked.raw_ts_data;
      }
      break;

    case WMS_MEMORY_STORE_NV_GW:
      cmd_err =  wms_msg_map_gw_nv_index(as_id, i, &i);
      if (WMS_CMD_ERR_NONE != cmd_err)
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(cmd_ptr->cmd.msg_read.index);
        break;
      }
      if(cfg_s_ptr->nv_gw_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", cmd_ptr->cmd.msg_read.index);
        break;
      }

      if( wms_nv_read_gw_sms( i, sim_data ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_READ;
      }
      else
      {
        sim_sms.status = sim_data[0];
        sim_sms.len = (uint8) MIN(appInfo_ptr->sms_rec_len-1, WMS_SIM_SMS_MAX-1);
        (void)memscpy( (void *)sim_sms.data,
                       sim_sms.len,
                       sim_data+1,
                       sim_sms.len );

        (void)wms_ts_unpack_sim_sms( & sim_sms,
                                 & sim_sms_unpacked );

        msg_event_info.status_info.message.msg_hdr.message_mode =
                                                         WMS_MESSAGE_MODE_GW;
        msg_event_info.status_info.message.msg_hdr.index = cmd_ptr->cmd.msg_read.index;
        msg_event_info.status_info.message.msg_hdr.mem_store = mem_store;
        msg_event_info.status_info.message.msg_hdr.tag = (wms_message_tag_e_type) (sim_data[0] & 0x7);
        msg_event_info.status_info.message.u.gw_message.is_broadcast = FALSE;
        msg_event_info.status_info.message.u.gw_message.sc_address
                = sim_sms_unpacked.sc_address;
        msg_event_info.status_info.message.u.gw_message.raw_ts_data
                = sim_sms_unpacked.raw_ts_data;
      }
      break;

#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR_1("Invalid memory store %d", cmd_ptr->cmd.msg_write.message.msg_hdr.mem_store);
      break;
  } /* switch mem_store */


  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
    msg_event_info.status_info.as_id = as_id;
    (void)wms_msg_event_notify( WMS_MSG_EVENT_READ, &msg_event_info );
  }

  // done
  return;

} /* wms_msg_read_proc() */

/*=========================================================================
FUNCTION
  wms_msg_is_mo_retry_allowed

DESCRIPTION
  Function to check if Retry is Allowed or not based on the Error.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_is_mo_retry_allowed
(
  uint8 retry_count,
  const wms_submit_report_info_s_type  *report_info_ptr
)
{
  boolean do_retry = FALSE;

  switch(report_info_ptr->report_status)
  {
    case WMS_RPT_SIP_PERM_ERROR:
      /* Terminate modem retry process when AP is the master. Else, fall back to 1X for retry (if 1X is available). */
      if (WMS_CLIENT_TYPE_QMI != (wms_client_id_type)report_info_ptr->client_id)
      {
         wms_client_s_type *client_ptr = wms_client_ptr_fetch((wms_client_id_type)report_info_ptr->client_id);                  
         if(client_ptr != NULL && client_ptr->active_bearers[WMS_BEARER_CDMA_1X])
         {
            do_retry = TRUE;
         }
         WMS_CLIENT_PTR_RELEASEIF(client_ptr);
      }

      MSG_HIGH_2("On SIP_PERM_ERROR, retry allowed? %d for client %d", do_retry, (wms_client_id_type)report_info_ptr->client_id);
      break;
      
    case WMS_RPT_SIP_TEMP_ERROR:
    case WMS_RPT_LL_ERROR:
    if (WMS_RPT_LL_ERROR == report_info_ptr->report_status &&
        WMS_RP_CAUSE_TEMP_REJ == report_info_ptr->cause_info.cause_value)
    {
      MSG_HIGH_0("Received WMS_RP_CAUSE_TEMP_REJ cause");
      if (0 == retry_count)
      {
        MSG_HIGH_0("Retrying the MO SMS with TEMP_REJ_CAUSE from MM");
        do_retry = TRUE;
      }
    }
      if (NULL == wms_msg_choose_registered_transport( report_info_ptr->as_id ) ||
          retry_count < cfg_s_ptr->sms_retry_limit)
      {
        do_retry = TRUE;
      }
      MSG_HIGH_1("On LL_ERROR or SIP_TEMP_ERROR, retry allowed? %d", do_retry);
      break;

    case WMS_RPT_GENERAL_ERROR:
    case WMS_RPT_OUT_OF_RESOURCES:
    case WMS_RPT_NETWORK_NOT_READY:
    case WMS_RPT_PHONE_NOT_READY:
    case WMS_RPT_NO_ACK:
    case WMS_RPT_NOT_ALLOWED_IN_AMPS:
    case WMS_RPT_ACCESS_BLOCK:
    case WMS_RPT_SMR_TR1M_TIMEOUT:
    case WMS_RPT_SMR_TR2M_TIMEOUT:
    case WMS_RPT_SMR_NO_RESOURCES:
    case WMS_RPT_ACCESS_TOO_LARGE: /* Set up SMS call */
    case WMS_RPT_RP_ERROR:
    case WMS_RPT_CP_ERROR:
    case WMS_RPT_NO_RESPONSE_FROM_NETWORK:
    case WMS_RPT_PREF_CHN_NOT_AVAIL_ERROR:
      do_retry = TRUE;
      MSG_HIGH_1("Retry allowed with report status: %d", report_info_ptr->report_status);
      break;

    case WMS_RPT_MO_CONTROL_DISALLOW:
    case WMS_RPT_MO_CONTROL_ERROR:
    case WMS_RPT_CDMA_TL_ERROR:
    case WMS_RPT_DC_TOO_LARGE:
    case WMS_RPT_CANNOT_SEND_BROADCAST:
    default:
      do_retry = FALSE;
      MSG_HIGH_1("Retry disallowed with report status: %d", report_info_ptr->report_status);
      break;
  }

  return do_retry;

} /* wms_msg_is_mo_retry_allowed() */

wms_cmd_err_e_type wms_msg_do_write(
  wms_prov_app_info_s_type     *appInfo_ptr,
  wms_write_mode_e_type         write_mode, /* IN */
  wms_client_message_s_type    *msg_ptr,    /* IN-OUT */
  wms_cmd_type                 *cmd_ptr
)
{
  uint32                    i       = 0;
  wms_cmd_err_e_type        cmd_err = WMS_CMD_ERR_NONE;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    cmd_err =  WMS_CMD_ERR_AS_ID;
    return cmd_err;
  }

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL == msg_ptr)
  {
    cmd_err = WMS_CMD_ERR_NULL_PTR;
    MSG_HIGH_0("wms_msg_do_write: NULL ptr");
    return cmd_err;
  }

  /* Check for Valid Message Tag */
  if ( msg_ptr->msg_hdr.tag != WMS_TAG_MT_READ &&
       msg_ptr->msg_hdr.tag != WMS_TAG_MT_NOT_READ &&
       msg_ptr->msg_hdr.tag != WMS_TAG_MO_NOT_SENT &&
       msg_ptr->msg_hdr.tag != WMS_TAG_MO_SENT &&
       msg_ptr->msg_hdr.tag != WMS_TAG_MO_SENT_ST_NOT_RECEIVED &&
       msg_ptr->msg_hdr.tag != WMS_TAG_MO_SENT_ST_NOT_STORED &&
       msg_ptr->msg_hdr.tag != WMS_TAG_MO_SENT_ST_STORED )
  {
    MSG_ERROR_1("Invalid Tag %d in wms_msg_do_write", msg_ptr->msg_hdr.tag);
    return WMS_CMD_ERR_MSG_TAG;
  }

#ifdef FEATURE_GWSMS
  if (WMS_MESSAGE_MODE_GW == msg_ptr->msg_hdr.message_mode)
  {
    cmd_err = wms_msg_check_tpdu_type(msg_ptr, TRUE);

    if (cmd_err != WMS_CMD_ERR_NONE)
    {
      return cmd_err;
    }

    if ( wms_ts_decode(&msg_ptr->u.gw_message.raw_ts_data, &wms_msg_Gclient_ts) != WMS_OK_S )
    {
      return WMS_CMD_ERR_MSG_DECODE;
    }
  }
#endif /* FEATURE_GWSMS */

  MSG_HIGH_1("Memory Store Type = %d", msg_ptr->msg_hdr.mem_store);

  switch( msg_ptr->msg_hdr.mem_store )
  {

#ifdef FEATURE_CDSMS

    case WMS_MEMORY_STORE_CSIM:
    case WMS_MEMORY_STORE_RUIM:
    {
        if (!wms_sim_mmgsdi_mem_store_supported(appInfo_ptr->mmgsdi_appInfo.app_type, msg_ptr->msg_hdr.mem_store))
        {
           cmd_err = WMS_CMD_ERR_UNSUPPORTED;
           break;
        }

        if( write_mode == WMS_WRITE_MODE_INSERT )
        {
           cmd_err = wms_msg_find_free_sms_rec_index(appInfo_ptr, &i);

           if( WMS_CMD_ERR_MSG_SIM_FULL == cmd_err )
           {
              cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
              wms_cfg_do_memory_full(appInfo_ptr->asid, msg_ptr->msg_hdr.mem_store);
           }
        } /* if INSERT */
        else if( write_mode == WMS_WRITE_MODE_REPLACE )
        {
          i = msg_ptr->msg_hdr.index;
          if( i >= (uint32) appInfo_ptr->sms_rec_count )
          {
            cmd_err = WMS_CMD_ERR_MSG_INDEX;
          }
        }
        else
        {
          cmd_err = WMS_CMD_ERR_MSG_WRITE_MODE;
        }

        if( cmd_err == WMS_CMD_ERR_NONE )
        {
          /* prepare for the message to be written to CSIM/RUIM
          */
          wms_ts_convert_cl2tl( & msg_ptr->u.cdma_message,
                                & cdma_tl );

          if( wms_ts_encode_CDMA_tl( & cdma_tl,
                                     & msg_ptr->u.cdma_message.raw_ts,
                                     & ruim_sms.ota ) == WMS_OK_S )
          {
            ruim_sms.status = (uint8) msg_ptr->msg_hdr.tag;
            if( wms_ts_cdma_pack_ruim_sms( & ruim_sms, ruim_data ) == FALSE )
            {
              cmd_err = WMS_CMD_ERR_MSG_ENCODE;
            }
  #if (defined(FEATURE_UIM_TOOLKIT_UTK) || defined(FEATURE_CCAT))
            /* Check if this is a prl update, if so another read must be invoked
            ** to get the correct user data from the card.
            */
            else if (wms_ts_decode(&msg_ptr->u.cdma_message.raw_ts,
                                  &wms_msg_Gclient_ts) != WMS_OK_S)
            {
              cmd_err = WMS_CMD_ERR_MSG_DECODE;
            }
            else
            {
              if ( (wms_msg_Gclient_ts.u.cdma.mask & WMS_MASK_BD_DISPLAY_MODE) &&
                   (wms_msg_Gclient_ts.u.cdma.display_mode == WMS_DISPLAY_MODE_RESERVED) &&
                   (wms_msg_Gclient_ts.u.cdma.download_mode == WMS_DOWNLOAD_MODE_UPDATE_PRL))
              {
                /* This message contains a PRL Update, set the appropriate flag
                ** when doing the write.
                */
                if (wms_sim_mmgsdi_write_data( appInfo_ptr,
                                               appInfo_ptr->sms_file_enum,
                                               (mmgsdi_rec_num_type)i,
                                               ruim_data,
                                               (uint32)appInfo_ptr->sms_rec_len) != MMGSDI_SUCCESS)
                {
                  cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
                }
                else if (wms_sim_mmgsdi_read_data( appInfo_ptr,
                                                   appInfo_ptr->sms_file_enum,
                                                   (mmgsdi_rec_num_type)i,
                                                   appInfo_ptr->sms_rec_len,
                                                   ruim_data,
                                                   sizeof(ruim_data)) != MMGSDI_SUCCESS)
                {
                  cmd_err = WMS_CMD_ERR_MSG_RUIM_READ;
                }
                else
                {
                  uint16 j;
                  // Print out the raw data in the updated SMS record.
                  for(j = 0; j < ruim_data[1] + 2; j++)
                  {
                    MSG_HIGH_2( "sms raw data[%d] = 0x%x",
                                j,
                                ruim_data[j] );
                  }

                  /* Update client data with the new content */
                  MSG_HIGH_0("Update client data with new content");
                  if (wms_ts_cdma_unpack_ruim_sms( ruim_data , &ruim_sms ) == FALSE)
                  {
                    cmd_err = WMS_CMD_ERR_MSG_DECODE;
                  }
                  else if( wms_ts_decode_CDMA_tl( &ruim_sms.ota, &cdma_tl, &cdma_raw_bd )
                           != WMS_OK_S )
                  {
                    cmd_err = WMS_CMD_ERR_MSG_DECODE;
                  }
                  else
                  {
                    wms_ts_convert_tl2cl( &cdma_tl, &cdma_cl );
                    cdma_cl.raw_ts = cdma_raw_bd;
                    msg_event_info.mt_message_info.message.u.cdma_message = cdma_cl;
                  }
                }
              }
              else
              {
                /* not a PRL update, do a regular write */
                if (wms_sim_mmgsdi_write_data( appInfo_ptr,
                                               appInfo_ptr->sms_file_enum,
                                               (mmgsdi_rec_num_type)i,
                                               ruim_data,
                                               (uint32)appInfo_ptr->sms_rec_len) != MMGSDI_SUCCESS)
                {
                  cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
                }
              }
            }

            if(cmd_err == WMS_CMD_ERR_NONE)
  #else /* defined(FEATURE_UIM_TOOLKIT_UTK) || defined(FEATURE_CCAT) */
            else if ( wms_sim_mmgsdi_write_data( appInfo_ptr,
                                                 appInfo_ptr->sms_file_enum,
                                                 (mmgsdi_rec_num_type)i,
                                                 ruim_data,
                                                 appInfo_ptr->sms_rec_len) != MMGSDI_SUCCESS)
            {
              cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
            }
            else
  #endif /* defined(FEATURE_UIM_TOOLKIT_UTK) || defined(FEATURE_CCAT) */
            {
              /* RUIM write was successful, update message list
              */
              appInfo_ptr->sms_rec_tags[i] = (wms_message_tag_e_type)ruim_sms.status;

              msg_ptr->msg_hdr.index  = i;

              if ( (WMS_WRITE_MODE_INSERT == write_mode) &&
                   (i+1 == (uint32) appInfo_ptr->sms_rec_count) )
              {
                MSG_ERROR_0("Last message stored in memory");
                wms_cfg_do_memory_full(appInfo_ptr->asid, msg_ptr->msg_hdr.mem_store);
                /* set memory Flag in RUIM/CSIM card */
                wms_cfg_update_cdma_memory_full_flag(appInfo_ptr, WMS_SIM_MEM_FULL_SET);
              }
              /* Update the duplicate detection info cache if this is a MT msg. */
              if(WMS_IS_MT(appInfo_ptr->sms_rec_tags[i]))
              {
                if(wms_ts_decode_bearer_data(&msg_ptr->u.cdma_message.raw_ts,
                                             &cdma_tl.cl_bd) != WMS_OK_S)
                {
                  MSG_ERROR_0("Decode CDMA_bd failed");
                  cmd_err = WMS_CMD_ERR_MSG_DECODE;
                }
                /* Check if this message is already in the cache */
                else if(wms_msg_cdma_check_dups(&cdma_tl) == FALSE)
                {
                  /* Update the duplicate detection cache */
                  wms_cfg_update_dup_info_cache(msg_ptr->msg_hdr.mem_store,
                                                i,
                                                &cdma_tl);
                }
              }
            }
          }
        }
      }
      break;

    case WMS_MEMORY_STORE_NV_CDMA:
      if( write_mode == WMS_WRITE_MODE_INSERT )
      {
        for( i=0; i<cfg_s_ptr->nv_cdma_max_slots; i++ )
        {
          if( cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_NONE )
          {
            /* found a free slot */
            break;
          }
        }

        if( i == cfg_s_ptr->nv_cdma_max_slots )
        {
          cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
          wms_cfg_do_memory_full(appInfo_ptr->asid, WMS_MEMORY_STORE_NV_CDMA );

        }
      } /* if INSERT */
      else if( write_mode == WMS_WRITE_MODE_REPLACE )
      {
        i = msg_ptr->msg_hdr.index;
        if( i >= cfg_s_ptr->nv_cdma_max_slots )
        {
          cmd_err = WMS_CMD_ERR_MSG_INDEX;
        }
      }
      else
      {
        cmd_err = WMS_CMD_ERR_MSG_WRITE_MODE;
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        /* prepare for the message to be written to NV
        */
        wms_ts_convert_cl2tl( & msg_ptr->u.cdma_message,
                              & cdma_tl );

        if( wms_ts_encode_CDMA_tl( & cdma_tl,
                                   & msg_ptr->u.cdma_message.raw_ts,
                                   & ruim_sms.ota ) == WMS_OK_S )
        {
          /* Layout of SMS message in NV. */
          /* 0: format                    */
          /* 1: tag/status                */
          /* 2: len                       */
          /* 3+: data                     */
          ruim_data[0] = (uint8) msg_ptr->u.cdma_message.raw_ts.format;
          ruim_data[1] = (uint8)msg_ptr->msg_hdr.tag;
          ruim_data[2] = (uint8)ruim_sms.ota.data_len;
          (void)memscpy( ruim_data + 3,
                         ruim_sms.ota.data_len,
                         (void *)ruim_sms.ota.data,
                         ruim_sms.ota.data_len );

          if(write_mode == WMS_WRITE_MODE_INSERT)
          {
            if( wms_nv_write_cdma_sms( i, ruim_data ) == FALSE )
            {
              cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
            }
            else
            {
              /* NV write was successful, update message list
              */
              cfg_s_ptr->nv_cdma_tags[i]  = msg_ptr->msg_hdr.tag;
              msg_ptr->msg_hdr.index      = i;

              /* Update the duplicate detection info cache if this is a MT msg. */
              if(WMS_IS_MT(cfg_s_ptr->nv_cdma_tags[i]))
              {
                if(wms_ts_decode_bearer_data(&msg_ptr->u.cdma_message.raw_ts,
                                             &cdma_tl.cl_bd) != WMS_OK_S)
                {
                  MSG_ERROR_0("Decode CDMA_bd failed");
                  cmd_err = WMS_CMD_ERR_MSG_DECODE;
                }
                else if( (msg_ptr->u.cdma_message.raw_ts.format != WMS_FORMAT_MWI) &&
                         (wms_ts_validate_bd_subparms( & cdma_tl.cl_bd, cdma_tl.teleservice ) != WMS_OK_S ) )
                {
                  cmd_err = WMS_CMD_ERR_MSG_DECODE;
                }
                /* Check if this message is already in the cache */
                else if(wms_msg_cdma_check_dups(&cdma_tl) == FALSE)
                {
                  /* Update the duplicate detection cache */
                  wms_cfg_update_dup_info_cache(msg_ptr->msg_hdr.mem_store,
                                                i,
                                                &cdma_tl);
                }
              }
            }
          }
          else if (write_mode == WMS_WRITE_MODE_REPLACE)
          {
            if( wms_nv_replace_cdma_sms( i, ruim_data ) == FALSE )
            {
               cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
            }
            else
            {
              /* NV write was successful, update message list
              */
              cfg_s_ptr->nv_cdma_tags[i]  = msg_ptr->msg_hdr.tag;
              msg_ptr->msg_hdr.index      = i;
            }
          }
        }
        else
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }

        wms_cfg_cdma_update_voice_mail_index( (wms_format_e_type) ruim_data[0],   /* format */
                                              (wms_message_tag_e_type) ruim_data[1],   /* tag */
                                              ruim_data[2],   /* len */
                                              ruim_data+3,    /* data */
                                              i );
      }
      break;

#endif /* FEATURE_CDSMS */



#ifdef FEATURE_GWSMS

    case WMS_MEMORY_STORE_RAM_GW:
      if( write_mode == WMS_WRITE_MODE_INSERT )
      {
        cmd_err = wms_msg_find_free_gw_ram_index(appInfo_ptr->asid, &msg_ptr->msg_hdr.index, &i);
        if( WMS_CMD_ERR_MSG_RAM_FULL == cmd_err )
        {
          cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
          /* Notify clients of memory full condition */
          wms_cfg_do_memory_full(appInfo_ptr->asid, WMS_MEMORY_STORE_RAM_GW);

        }
      } /* if INSERT */
      else if( write_mode == WMS_WRITE_MODE_REPLACE )
      {
        i = msg_ptr->msg_hdr.index;
        cmd_err = wms_msg_map_gw_ram_index(appInfo_ptr->asid, i, &i);
        if((WMS_CMD_ERR_NONE == cmd_err) && (NULL == cfg_s_ptr->ram_gw_sms[i]))
        {
          cmd_err = WMS_CMD_ERR_MSG_INDEX;
        }
      }
      else
      {
        cmd_err = WMS_CMD_ERR_MSG_WRITE_MODE;
      }

      if ((cmd_err == WMS_CMD_ERR_NONE) && (i < WMS_MAX_RAM_MESSAGES))
      {
        sim_sms_unpacked.sc_address = msg_ptr->u.gw_message.sc_address;
        sim_sms_unpacked.raw_ts_data =
                  msg_ptr->u.gw_message.raw_ts_data;

        /* Check for Valid SC Address */
        if ( msg_ptr->u.gw_message.sc_address.number_of_digits == 0 )
        {
           MSG_HIGH_0("No valid SC address found, get from template");
           cmd_err = wms_msg_fill_sc_address (appInfo_ptr, &sim_sms_unpacked.sc_address );
        }

        if ( cmd_err != WMS_CMD_ERR_NONE )
            break;

        if( wms_ts_pack_sim_sms( & sim_sms_unpacked,
                                 cfg_s_ptr->ram_gw_sms[i] )
            != TRUE )
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
          break;
        }
        else
        {
          /* msg was written to free slot, update status and exit
          */
          if (i<WMS_MAX_RAM_MESSAGES)
          {
            cfg_s_ptr->ram_gw_sms[i]->status = (uint8)msg_ptr->msg_hdr.tag;

          }
        }
      }
      break;

    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_SIM:
      if (!wms_sim_mmgsdi_mem_store_supported(appInfo_ptr->mmgsdi_appInfo.app_type, msg_ptr->msg_hdr.mem_store))
      {
         cmd_err =  WMS_CMD_ERR_UNSUPPORTED;
         break;
      }


      if (!appInfo_ptr->sim_file_init_is_done[SMS_FILE_INDEX])
      {
         MSG_ERROR_0("wms_msg_do_write failed - init incomplete");
         cmd_err = WMS_CMD_ERR_BUSY;
         break;
      }

      if( write_mode == WMS_WRITE_MODE_INSERT )
      {
        cmd_err = wms_msg_find_free_sms_rec_index(appInfo_ptr, &i);

        if( WMS_CMD_ERR_MSG_SIM_FULL == cmd_err )
        {
          if (wms_msg_total_memory_full(appInfo_ptr))
          {
            /*SIM and NV are full*/
            cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
            wms_cfg_do_memory_full(appInfo_ptr->asid, msg_ptr->msg_hdr.mem_store);
            wms_msg_notify_SMMA( appInfo_ptr, FALSE );
          }
          else
          {
            /*Only SIM Is full*/
            cmd_err = WMS_CMD_ERR_MSG_SIM_FULL;
            wms_cfg_do_memory_full(appInfo_ptr->asid, msg_ptr->msg_hdr.mem_store);
          }

        }
      } /* if INSERT */
      else if( write_mode == WMS_WRITE_MODE_REPLACE )
      {
        i = msg_ptr->msg_hdr.index;
        if( i >= (uint32)appInfo_ptr->sms_rec_count )
        {
          cmd_err = WMS_CMD_ERR_MSG_INDEX;
        }
      }
      else
      {
        cmd_err = WMS_CMD_ERR_MSG_WRITE_MODE;
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        /* prepare for the message to be written to SIM
        */
        sim_sms_unpacked.status = (uint8)msg_ptr->msg_hdr.tag;
        sim_sms_unpacked.sc_address = msg_ptr->u.gw_message.sc_address;
        sim_sms_unpacked.raw_ts_data =
                      msg_ptr->u.gw_message.raw_ts_data;

        /* Check for Valid SC Address */
        if ( msg_ptr->u.gw_message.sc_address.number_of_digits == 0 )
        {
           MSG_HIGH_0("No valid SC address found, get from template");
           cmd_err = wms_msg_fill_sc_address (appInfo_ptr, &sim_sms_unpacked.sc_address );
        }

        if ( cmd_err != WMS_CMD_ERR_NONE )
            break;

        if( wms_ts_pack_sim_sms( & sim_sms_unpacked, & sim_sms )  != TRUE )
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }
        else
        {
          memset(sim_data, 0xff, WMS_MAX_LEN);
          sim_data[0] = sim_sms.status;

          if (sim_sms.len > 0)
          {
            (void)memscpy( sim_data + 1,
                           MIN(WMS_SIM_SMS_MAX, sim_sms.len),
                           (void *)sim_sms.data,
                           MIN(WMS_SIM_SMS_MAX, sim_sms.len));
          }
          else
          {
            cmd_err = WMS_CMD_ERR_BAD_PARM;
            break;
          }

          if( wms_sim_mmgsdi_write_data( appInfo_ptr,
                                         appInfo_ptr->sms_file_enum,
                                         (mmgsdi_rec_num_type)i,
                                         sim_data,
                                         (uint32)appInfo_ptr->sms_rec_len) != MMGSDI_SUCCESS)
          {
            cmd_err = WMS_CMD_ERR_MSG_SIM_WRITE;
          }
          else
          {
            /* SIM write was successful, update message list
            */
            appInfo_ptr->sms_rec_tags[i] = (wms_message_tag_e_type) sim_sms.status;

            /*The sms reference is required for mapping it to the status reports
              if the status report is requested. Otherwise it is defaulted to WMS_DUMMY_MESSAGE_NUMBER. */
            if( wms_msg_Gclient_ts.u.gw_pp.u.submit.status_report_enabled )
            {
              appInfo_ptr->sms_reference[i] = wms_msg_Gclient_ts.u.gw_pp.u.submit.message_reference;
            }
            else
            {
              appInfo_ptr->sms_reference[i] = WMS_DUMMY_MESSAGE_NUMBER;
            }
            msg_ptr->msg_hdr.index = i;
            wms_cfg_update_voice_mail_index( appInfo_ptr, sim_data, i );
          }
        }
      }
      break;


    case WMS_MEMORY_STORE_NV_GW:
      if( write_mode == WMS_WRITE_MODE_INSERT )
      {
        cmd_err = wms_msg_find_free_gw_nv_index(appInfo_ptr->asid, &msg_ptr->msg_hdr.index, &i);

        if( WMS_CMD_ERR_MSG_NV_FULL == cmd_err )
        {
          if (wms_msg_total_memory_full(appInfo_ptr))
          {
            /*SIM and NV are full*/
            cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
            wms_cfg_do_memory_full(appInfo_ptr->asid, WMS_MEMORY_STORE_NV_GW);
            wms_msg_notify_SMMA( appInfo_ptr, FALSE );
          }
          else
          {
            /*Only GW NV Is full*/
            cmd_err = WMS_CMD_ERR_MSG_NV_FULL;
            wms_cfg_do_memory_full(appInfo_ptr->asid, WMS_MEMORY_STORE_NV_GW);
          }
        }
      } /* if INSERT */
      else if( write_mode == WMS_WRITE_MODE_REPLACE )
      {
        i = msg_ptr->msg_hdr.index;
        cmd_err = wms_msg_map_gw_nv_index(appInfo_ptr->asid, i, &i);
      }
      else
      {
        cmd_err = WMS_CMD_ERR_MSG_WRITE_MODE;
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        /* prepare for the message to be written to NV
        */
        sim_sms_unpacked.status = (uint8)msg_ptr->msg_hdr.tag;
        sim_sms_unpacked.sc_address = msg_ptr->u.gw_message.sc_address;
        sim_sms_unpacked.raw_ts_data =
                      msg_ptr->u.gw_message.raw_ts_data;

        /* Check for Valid SC Address */
        if ( msg_ptr->u.gw_message.sc_address.number_of_digits == 0 )
        {
           MSG_ERROR_0("No valid SC address found, get from template");
           cmd_err = wms_msg_fill_sc_address (appInfo_ptr, &sim_sms_unpacked.sc_address );
        }

        if ( cmd_err != WMS_CMD_ERR_NONE )
            break;

        if( wms_ts_pack_sim_sms( & sim_sms_unpacked,
                                   & sim_sms )
                != TRUE )
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }
        else
        {
          sim_data[0] = sim_sms.status;

          if(sim_sms.len > 0)
          {
            (void)memscpy( sim_data + 1,
                           MIN(WMS_SIM_SMS_MAX, sim_sms.len),
                           (void *)sim_sms.data,
                           MIN(WMS_SIM_SMS_MAX, sim_sms.len));
          }
          else
          {
            cmd_err = WMS_CMD_ERR_BAD_PARM;
            break;
          }

          if(write_mode == WMS_WRITE_MODE_INSERT)
          {
           if( wms_nv_write_gw_sms( i, sim_data ) == FALSE )
           {
             cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
           }
           else
           {
            /* NV write was successful, update message list
            */
            cfg_s_ptr->nv_gw_tags[i] = (wms_message_tag_e_type) sim_sms.status;
           }
          }
          else if (write_mode == WMS_WRITE_MODE_REPLACE)
          {
            if( wms_nv_replace_gw_sms( i, sim_data ) == FALSE )
            {
               cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
            }
            else
            {
            /* NV write was successful, update message list
            */
              cfg_s_ptr->nv_gw_tags[i] = (wms_message_tag_e_type) sim_sms.status;
            }
          }

        }
      }
      break;

#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR_1("Invalid memory store %d", msg_ptr->msg_hdr.mem_store);
      break;
  } /* switch mem_store */

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    /* send out message list event */
    wms_cfg_do_message_list(appInfo_ptr, msg_ptr->msg_hdr.mem_store, WMS_TAG_NONE, cmd_ptr);

    if( write_mode == WMS_WRITE_MODE_INSERT )
    {
      wms_cfg_do_memory_status(appInfo_ptr, msg_ptr->msg_hdr.mem_store, WMS_TAG_NONE, cmd_ptr);
    }
  }

  return cmd_err;
} /* wms_msg_do_write() */


/*
*/
void wms_msg_write_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type                cmd_err = WMS_CMD_ERR_NONE;

  if( cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MT_READ &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MT_NOT_READ &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MO_NOT_SENT &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MO_SENT &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MO_SENT_ST_NOT_RECEIVED &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MO_SENT_ST_NOT_STORED &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MO_SENT_ST_STORED
    )
  {
    cmd_err = WMS_CMD_ERR_MSG_TAG;
    wms_client_cmd_status( cmd_ptr, cmd_err );
    MSG_ERROR_1("Invalid Tag %d in wms_msg_write_proc", cmd_ptr->cmd.msg_write.message.msg_hdr.tag);
    return;
  }

  cmd_err = wms_msg_do_write( cmd_ptr->hdr.appInfo_ptr,
                              cmd_ptr->cmd.msg_write.write_mode,
                              &cmd_ptr->cmd.msg_write.message,
                              cmd_ptr );

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id  =  (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.write_mode = cmd_ptr->cmd.msg_write.write_mode;
    msg_event_info.status_info.message    = cmd_ptr->cmd.msg_write.message;
    msg_event_info.status_info.as_id      = cmd_ptr->hdr.as_id;
    (void)wms_msg_event_notify( WMS_MSG_EVENT_WRITE, &msg_event_info );
  }

  /* done */
  return;

} /* wms_msg_write_proc() */


#ifdef FEATURE_GWSMS
void wms_msg_del_sts_rpt_for_msg(
  wms_prov_app_info_s_type *appInfo_ptr,
  uint8                     sms_index,
  wms_cmd_type             *cmd_ptr
)
{
  wms_message_index_type   smsr_index;
  wms_memory_store_e_type  mem_store  = WMS_MEMORY_STORE_SIM;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return;
  }

  mem_store = wms_sim_mmgsdi_get_supproted_mem_store(appInfo_ptr->mmgsdi_appInfo.app_type);

  if (wms_cfg_ms_get_sts_rpt_index(appInfo_ptr->asid, sms_index, &smsr_index))
  {
    mmgsdi_return_enum_type mmret;

    mmret = wms_sim_mmgsdi_delete_data( appInfo_ptr,
                                        appInfo_ptr->smsr_file_enum,
                                        (mmgsdi_rec_num_type)smsr_index,
                                        (uint32)appInfo_ptr->smsr_rec_len );
    if (MMGSDI_SUCCESS == mmret)
    {
      appInfo_ptr->smsr_status_rpts[smsr_index] = FALSE;

      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      msg_event_info.status_info.message.msg_hdr.index        = smsr_index;
      msg_event_info.status_info.message.msg_hdr.mem_store    = mem_store;
      msg_event_info.status_info.message.msg_hdr.tag          = WMS_TAG_STATUS_RPT;
      msg_event_info.status_info.as_id                        = appInfo_ptr->asid;
      msg_event_info.status_info.client_id                    = cmd_ptr->hdr.client_id;

      (void)wms_msg_event_notify( WMS_MSG_EVENT_DELETE_STS_REPORT, &msg_event_info );

      wms_cfg_do_sts_report_list(appInfo_ptr, mem_store, cmd_ptr);
      wms_cfg_do_memory_status(appInfo_ptr, WMS_MEMORY_STORE_SIM, WMS_TAG_STATUS_RPT, cmd_ptr);
    }
  }

  return;
} /* wms_msg_del_sts_rpt_for_msg() */
#endif /* FEATURE_GWSMS */

/*
*/
void wms_msg_delete_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_prov_app_info_s_type   *appInfo_ptr;
  wms_cmd_err_e_type          cmd_err     = WMS_CMD_ERR_NONE;
  uint32                      i           = cmd_ptr->cmd.msg_delete.index;
  wms_memory_store_e_type     mem_store   = cmd_ptr->cmd.msg_delete.mem_store;

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     cmd_err =  WMS_CMD_ERR_AS_ID;
     wms_client_cmd_status( cmd_ptr, cmd_err );
     return;
  }

  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

    case WMS_MEMORY_STORE_CSIM:
    case WMS_MEMORY_STORE_RUIM:
      if (!wms_sim_mmgsdi_mem_store_supported(appInfo_ptr->mmgsdi_appInfo.app_type, mem_store))
      {
         cmd_err =  WMS_CMD_ERR_UNSUPPORTED;
         break;
      }
      else if ( i >= (uint32) appInfo_ptr->sms_rec_count )
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(cmd_ptr->cmd.msg_delete.index);
      }
      else if(appInfo_ptr->sms_rec_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", cmd_ptr->cmd.msg_delete.index);
      }
      else if( wms_sim_mmgsdi_delete_data( appInfo_ptr,
                                           appInfo_ptr->sms_file_enum,
                                           (mmgsdi_rec_num_type)i,
                                           (uint32)appInfo_ptr->sms_rec_len) != MMGSDI_SUCCESS)
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_DELETE;
      }
      else
      {
        if(0 == (appInfo_ptr->smss_mem_full_flag & (uint8)WMS_SIM_MEM_FULL_MASK))
        {
          MSG_HIGH_0("Memory was full");
          /* Clear the flag */
          wms_cfg_update_cdma_memory_full_flag(appInfo_ptr, WMS_SIM_MEM_FULL_CLEAR);
        }
        /* Update duplicate detection list */
        if(WMS_IS_MT(appInfo_ptr->sms_rec_tags[i]))
        {
          /* Update the duplicate detection info cache. Mark the location as
          ** empty by simply setting the mask to 0.
          */
          wms_cfg_delete_dup_info_cache(mem_store, i);
        }

        /* Update Delete Message Event with Correct Tag */
        msg_event_info.status_info.message.msg_hdr.tag = appInfo_ptr->sms_rec_tags[i];

        /* RUIM delete was successful, update message list
        */
        appInfo_ptr->sms_rec_tags[i] = WMS_TAG_NONE;
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
      break;

    case WMS_MEMORY_STORE_NV_CDMA:
      /* Clear out the record
      */
      if  (i >= cfg_s_ptr->nv_cdma_max_slots)
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(cmd_ptr->cmd.msg_delete.index);
        break;
      }
      if(cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", cmd_ptr->cmd.msg_delete.index);
        break;
      }

          /* Layout of SMS message in NV. */
          /* 0: format                    */
          /* 1: tag/status                */
          /* 2: len                       */
          /* 3+: data                     */

      ruim_data[1] =  (uint8)WMS_TAG_NONE;

      if( wms_nv_delete_cdma_sms( i, ruim_data ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_DELETE;
      }
      else
      {
        /* Update duplicate detection list */
        if(WMS_IS_MT(cfg_s_ptr->nv_cdma_tags[i]))
        {
          /* Update the duplicate detection info cache. Mark the location as
          ** empty by simply setting the mask to 0.
          */
          wms_cfg_delete_dup_info_cache(mem_store, i);
        }

        /* Update Delete Message Event with Correct Tag */
        msg_event_info.status_info.message.msg_hdr.tag = cfg_s_ptr->nv_cdma_tags[i];

        /* NV write was successful, update message list
        */
        cfg_s_ptr->nv_cdma_tags[i] = WMS_TAG_NONE;
        if( cfg_s_ptr->cdma_voice_mail_index == i )
        {
          cfg_s_ptr->cdma_voice_mail_index   = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
          cfg_s_ptr->cdma_voice_mail_active  = FALSE;
        }
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
      break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS

    case WMS_MEMORY_STORE_RAM_GW:
      cmd_err = wms_msg_map_gw_ram_index(appInfo_ptr->asid, i, &i);

      if (WMS_CMD_ERR_NONE != cmd_err)
       {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(cmd_ptr->cmd.msg_delete.index);
        break;
      }
      if(NULL == cfg_s_ptr->ram_gw_sms[i])
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", cmd_ptr->cmd.msg_delete.index);
        break;
      }

      /* Update Delete Message Event with Correct Tag */
      msg_event_info.status_info.message.msg_hdr.tag = (wms_message_tag_e_type) cfg_s_ptr->ram_gw_sms[i]->status;
      msg_event_info.status_info.message.msg_hdr.message_mode =  WMS_MESSAGE_MODE_GW;
      WMS_MEM_FREEIF(cfg_s_ptr->ram_gw_sms[i]);
      break;

    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_SIM:
      if (!wms_sim_mmgsdi_mem_store_supported(appInfo_ptr->mmgsdi_appInfo.app_type, mem_store))
      {
         cmd_err =  WMS_CMD_ERR_UNSUPPORTED;
         break;
      }
      if ( i >= (uint32) appInfo_ptr->sms_rec_count )
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(cmd_ptr->cmd.msg_delete.index);
      }
      else if(appInfo_ptr->sms_rec_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", cmd_ptr->cmd.msg_delete.index);
      }
      else if( wms_sim_mmgsdi_delete_data( appInfo_ptr,
                                           appInfo_ptr->sms_file_enum,
                                           (mmgsdi_rec_num_type)i,
                                           (uint32)appInfo_ptr->sms_rec_len) != MMGSDI_SUCCESS)
      {
        cmd_err = WMS_CMD_ERR_MSG_SIM_DELETE;
      }
      else
      {
        /* SIM write was successful, update message list
        */
        appInfo_ptr->sms_rec_tags[i] = WMS_TAG_NONE;

        /* Update Delete Message Event with Correct Tag */
        msg_event_info.status_info.message.msg_hdr.tag = appInfo_ptr->sms_rec_tags[i];

        if( appInfo_ptr->voice_mail_index == i )
        {
          appInfo_ptr->voice_mail_index   = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
          appInfo_ptr->voice_mail_active  = FALSE;
        }
        wms_msg_del_sts_rpt_for_msg(appInfo_ptr, (uint8)i, cmd_ptr);
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      break;

    case WMS_MEMORY_STORE_NV_GW:
      /* Clear out the record
      */
      cmd_err =  wms_msg_map_gw_nv_index(appInfo_ptr->asid, i, &i);
      if (WMS_CMD_ERR_NONE != cmd_err)
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(cmd_ptr->cmd.msg_delete.index);
        break;
      }
      if(cfg_s_ptr->nv_gw_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", cmd_ptr->cmd.msg_delete.index);
        break;
      }
      sim_data[0] = (uint8)WMS_TAG_NONE;

      {
        uint32 sms_rec_len = (uint32)MIN(WMS_SIM_SMS_MAX-1, appInfo_ptr->sms_rec_len-1);
        memset( sim_data+1, 0xFF, sms_rec_len );
      }

      if( wms_nv_delete_gw_sms( i, sim_data ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_DELETE;
      }
      else
      {
        /* Update Delete Message Event with Correct Tag */
        msg_event_info.status_info.message.msg_hdr.tag = cfg_s_ptr->nv_gw_tags[i];

        /* NV write was successful, update message list
        */
        cfg_s_ptr->nv_gw_tags[i] = WMS_TAG_NONE;
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      break;

#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR_1("Invalid memory store %d", mem_store);
      break;
  } /* switch mem_store */


  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (cmd_err == WMS_CMD_ERR_NONE)
  {
    msg_event_info.status_info.user_data                 = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id                 = (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.message.msg_hdr.index     = cmd_ptr->cmd.msg_delete.index;
    msg_event_info.status_info.message.msg_hdr.mem_store = mem_store;
    msg_event_info.status_info.as_id                     = appInfo_ptr->asid;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_DELETE, &msg_event_info );

    // send out CFG message list event
    wms_cfg_do_message_list(appInfo_ptr, mem_store, WMS_TAG_NONE, cmd_ptr);
    wms_cfg_do_memory_status(appInfo_ptr, mem_store, WMS_TAG_NONE, cmd_ptr );

#ifdef FEATURE_GWSMS
    wms_msg_notify_SMMA(appInfo_ptr, FALSE);
#endif /* FEATURE_GWSMS */
  }

  return;
} /* wms_msg_delete_proc() */

/*
*/
void wms_msg_delete_all_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_prov_app_info_s_type   *appInfo_ptr;
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  uint32                      i;
  wms_memory_store_e_type     mem_store = cmd_ptr->cmd.msg_delete_all.mem_store;
  wms_message_tag_e_type      tag = cmd_ptr->cmd.msg_delete_all.tag;

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     cmd_err =  WMS_CMD_ERR_AS_ID;
     wms_client_cmd_status( cmd_ptr, cmd_err );
     return;
  }

  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

    case WMS_MEMORY_STORE_CSIM:
    case WMS_MEMORY_STORE_RUIM:
      if (!wms_sim_mmgsdi_mem_store_supported(appInfo_ptr->mmgsdi_appInfo.app_type, mem_store))
      {
         cmd_err =  WMS_CMD_ERR_UNSUPPORTED;
         break;
      }
      for( i=0; i < (uint32) appInfo_ptr->sms_rec_count; i++ )
      {
        if( tag == WMS_TAG_NONE ||
            ( tag == appInfo_ptr->sms_rec_tags[i] ) )
        {
          if( appInfo_ptr->sms_rec_tags[i] == WMS_TAG_NONE )
          {
            continue; /* skip empty slots */
          }
          else if(wms_sim_mmgsdi_delete_data( appInfo_ptr,
                                              appInfo_ptr->sms_file_enum,
                                              (mmgsdi_rec_num_type)i,
                                              (uint32)appInfo_ptr->sms_rec_len) != MMGSDI_SUCCESS)
          {
            cmd_err = WMS_CMD_ERR_MSG_RUIM_DELETE;
          }
          else
          {
            if(0 == (appInfo_ptr->smss_mem_full_flag & (uint8)WMS_SIM_MEM_FULL_MASK))
            {
              MSG_HIGH_0("Memory was full");
              /* Clear the flag */
              wms_cfg_update_cdma_memory_full_flag(appInfo_ptr, WMS_SIM_MEM_FULL_CLEAR);
            }
            /* Update duplicate detection list */
            if(WMS_IS_MT(appInfo_ptr->sms_rec_tags[i]))
            {
              /* Update the duplicate detection info cache. Mark the location as
              ** empty by simply setting the mask to 0.
              */
              wms_cfg_delete_dup_info_cache(mem_store, i);
            }

            /* RUIM delete was successful, update message list
            */
            appInfo_ptr->sms_rec_tags[i] = WMS_TAG_NONE;
          }
        }
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
      break;

    case WMS_MEMORY_STORE_NV_CDMA:
      for( i=0; i < cfg_s_ptr->nv_cdma_max_slots; i++ )
      {
        if( tag == WMS_TAG_NONE ||
            ( tag == cfg_s_ptr->nv_cdma_tags[i] ) )
        {
          ruim_data[1] =  (uint8)WMS_TAG_NONE;

          if( cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_NONE )
          {
            continue; /* skip empty slots */
          }
          else if( wms_nv_delete_cdma_sms( i, ruim_data ) == FALSE )
          {
            cmd_err = WMS_CMD_ERR_MSG_NV_DELETE;
          }
          else
          {
            /* Update duplicate detection list */
            if(WMS_IS_MT(cfg_s_ptr->nv_cdma_tags[i]))
            {
              /* Update the duplicate detection info cache. Mark the location as
              ** empty by simply setting the mask to 0.
              */
              wms_cfg_delete_dup_info_cache(mem_store, i);
            }

            /* NV write was successful, update message list
            */
            cfg_s_ptr->nv_cdma_tags[i] = WMS_TAG_NONE;
            if( cfg_s_ptr->cdma_voice_mail_index == i )
            {
              cfg_s_ptr->cdma_voice_mail_index=(wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
              cfg_s_ptr->cdma_voice_mail_active  = FALSE;
            }
          }
        }
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
      break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS

    case WMS_MEMORY_STORE_RAM_GW:
      for( i=0; i < WMS_MAX_RAM_MESSAGES; i++ )
      {
        if( ( NULL != cfg_s_ptr->ram_gw_sms[i] ) &&
            ( tag == WMS_TAG_NONE ||
              tag == (wms_message_tag_e_type)cfg_s_ptr->ram_gw_sms[i]->status ) )
        {
          WMS_MEM_FREEIF(cfg_s_ptr->ram_gw_sms[i]);
        }
      }

      /*Delete all the status reports as well*/
      for( i=0; i < WMS_MAX_RAM_STATUS_REPORTS; i++ )
      {
        cfg_s_ptr->ram_status_rpts[i] = FALSE;
      }

      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      break;

    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_SIM:
      for( i=0; i < (uint32)appInfo_ptr->sms_rec_count; i++ )
      {
        if( tag == WMS_TAG_NONE ||
            ( tag == appInfo_ptr->sms_rec_tags[i] ) )
        {
          if(appInfo_ptr->sms_rec_tags[i]== WMS_TAG_NONE)
          {
             continue;
          }
          if( wms_sim_mmgsdi_delete_data( appInfo_ptr,
                                          appInfo_ptr->sms_file_enum,
                                          (mmgsdi_rec_num_type)i,
                                          (uint32)appInfo_ptr->sms_rec_len) != MMGSDI_SUCCESS )
          {
            cmd_err = WMS_CMD_ERR_MSG_SIM_DELETE;
          }
          else
          {
            /* SIM write was successful, update message list
            */
            appInfo_ptr->sms_rec_tags[i] = WMS_TAG_NONE;
            if( appInfo_ptr->voice_mail_index == i )
            {
              appInfo_ptr->voice_mail_index   = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
              appInfo_ptr->voice_mail_active  = FALSE;
            }
          }
        }
      }

      /*Delete all the status reports as well*/
      for( i=0; i <(uint32)appInfo_ptr->smsr_rec_count; i++ )
      {
        if( appInfo_ptr->smsr_status_rpts[i] == FALSE )
        {
          continue;
        }

        if( wms_sim_mmgsdi_delete_data( appInfo_ptr,
                                        appInfo_ptr->smsr_file_enum,
                                        (mmgsdi_rec_num_type)i,
                                        (uint32)appInfo_ptr->smsr_rec_len) != MMGSDI_SUCCESS )
        {
          cmd_err = WMS_CMD_ERR_MSG_SIM_DELETE;
        }
        else
        {
          appInfo_ptr->smsr_status_rpts[i] = FALSE;
        }
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      break;


    case WMS_MEMORY_STORE_NV_GW:
      for( i=0; i < cfg_s_ptr->nv_gw_max_slots; i++ )
      {
        if( tag == WMS_TAG_NONE ||
            ( tag == cfg_s_ptr->nv_gw_tags[i] ) )
        {

          if( cfg_s_ptr->nv_gw_tags[i] == WMS_TAG_NONE )
          {
            continue; /* skip empty slots */
          }

          sim_data[0] = (uint8)WMS_TAG_NONE;

          {
            uint32 sms_rec_len = (uint32)MIN(WMS_SIM_SMS_MAX-1, appInfo_ptr->sms_rec_len-1);
            memset( sim_data+1, 0xFF, sms_rec_len );
          }
          if( wms_nv_delete_gw_sms( i, sim_data ) == FALSE )
          {
            cmd_err = WMS_CMD_ERR_MSG_NV_DELETE;
          }
          else
          {
            /* NV write was successful, update message list
            */
            cfg_s_ptr->nv_gw_tags[i] = WMS_TAG_NONE;
          }
        }
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      break;

#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR_1("Invalid memory store %d", mem_store);
      break;
  } /* switch mem_store */


  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (cmd_err == WMS_CMD_ERR_NONE)
  {
    msg_event_info.status_info.user_data                 = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id                 = (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.message.msg_hdr.index     = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
    msg_event_info.status_info.message.msg_hdr.mem_store = mem_store;
    msg_event_info.status_info.message.msg_hdr.tag       = WMS_TAG_NONE;
    msg_event_info.status_info.as_id                     = appInfo_ptr->asid;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_DELETE_ALL, &msg_event_info );

    // send out CFG message list event
    wms_cfg_do_message_list(appInfo_ptr, mem_store, WMS_TAG_NONE, cmd_ptr);
    wms_cfg_do_memory_status(appInfo_ptr, mem_store, WMS_TAG_NONE, cmd_ptr);

#ifdef FEATURE_GWSMS
    wms_msg_notify_SMMA(appInfo_ptr, FALSE);
#endif /* FEATURE_GWSMS */
  }

  return;
} /* wms_msg_delete_all_proc() */


wms_cmd_err_e_type wms_msg_do_modify_tag(
  wms_prov_app_info_s_type   *appInfo_ptr,
  wms_memory_store_e_type     mem_store,
  wms_message_index_type      index,
  wms_message_tag_e_type      tag,
  wms_cmd_type       	     *cmd_ptr
)
{
  wms_cmd_err_e_type    cmd_err = WMS_CMD_ERR_NONE;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return WMS_CMD_ERR_AS_ID;
  }

  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

    case WMS_MEMORY_STORE_CSIM:
    case WMS_MEMORY_STORE_RUIM:
      if(index >= (wms_message_index_type)appInfo_ptr->sms_rec_count)
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(index);
      }
      else if(appInfo_ptr->sms_rec_tags[index] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", index);
      }
      else if(wms_sim_mmgsdi_read_data( appInfo_ptr,
                                        appInfo_ptr->sms_file_enum,
                                        (mmgsdi_rec_num_type)index,
                                        appInfo_ptr->sms_rec_len,
                                        ruim_data,
                                        sizeof(ruim_data)) != MMGSDI_SUCCESS)
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_READ;
      }
      else
      {
        ruim_data[0] = (uint8)tag;
        if( wms_sim_mmgsdi_write_data( appInfo_ptr,
                                       appInfo_ptr->sms_file_enum,
                                       (mmgsdi_rec_num_type)index,
                                       ruim_data,
                                       (uint32)appInfo_ptr->sms_rec_len) != MMGSDI_SUCCESS)
        {
          cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
        }
        else
        {
          /* RUIM/CSIM write was successful, update message list
          */
          appInfo_ptr->sms_rec_tags[index] = tag;
        }
      }

      break;

    case WMS_MEMORY_STORE_NV_CDMA:
      if(index >= cfg_s_ptr->nv_cdma_max_slots)
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(index);
      }
      else if(cfg_s_ptr->nv_cdma_tags[index] == WMS_TAG_NONE)
      {
        cmd_err = WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", index);
      }
      else if(wms_nv_read_cdma_sms(index, ruim_data) == FALSE)
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_READ;
      }
      else
      {
        ruim_data[1] = (uint8)tag;
        if( wms_nv_replace_cdma_sms( index, ruim_data ) == FALSE )
        {
          cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
        }
        else
        {
          /* SIM write was successful, update message list
          */
          cfg_s_ptr->nv_cdma_tags[index] = tag;
        }
      }
      break;

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS

    case WMS_MEMORY_STORE_RAM_GW:
      cmd_err = wms_msg_map_gw_ram_index(appInfo_ptr->asid, index, &index);

      if (WMS_CMD_ERR_NONE != cmd_err)
      {
         break;
      }

      if (index < WMS_MAX_RAM_MESSAGES)
      {
         if (NULL == cfg_s_ptr->ram_gw_sms[index])
         {
            cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
            MSG_ERROR_1("Empty message %d", index);
         }
         else
         {
            cfg_s_ptr->ram_gw_sms[index]->status =(uint8)tag;
         }
      }
      break;

    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_SIM:
      if (index >= (wms_message_index_type)appInfo_ptr->sms_rec_count)
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(index);
      }
      else if (WMS_TAG_NONE == appInfo_ptr->sms_rec_tags[index])
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", index);
      }
      else if (wms_sim_mmgsdi_read_data( appInfo_ptr,
                                         appInfo_ptr->sms_file_enum,
                                         (mmgsdi_rec_num_type)index,
                                         appInfo_ptr->sms_rec_len,
                                         sim_data,
                                         sizeof(sim_data)) != MMGSDI_SUCCESS)
      {
        cmd_err = WMS_CMD_ERR_MSG_SIM_READ;
      }
      else
      {
        sim_data[0] = (uint8)tag;
        if (wms_sim_mmgsdi_write_data( appInfo_ptr,
                                       appInfo_ptr->sms_file_enum,
                                       (mmgsdi_rec_num_type)index,
                                       sim_data,
                                       (uint32)appInfo_ptr->sms_rec_len) != MMGSDI_SUCCESS)
        {
          cmd_err = WMS_CMD_ERR_MSG_SIM_WRITE;
        }
        else
        {
          /* SIM write was successful, update message list
          */
          appInfo_ptr->sms_rec_tags[index] = tag;
        }
      }
      break;

    case WMS_MEMORY_STORE_NV_GW:
      cmd_err = wms_msg_map_gw_nv_index(appInfo_ptr->asid, index, &index);

      if (WMS_CMD_ERR_NONE != cmd_err)
      {
        break;
      }
      if (index >= WMS_MAX_NV_GW_MESSAGES ||
          wms_nv_read_gw_sms( index, sim_data ) == FALSE)
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_READ;
      }
      else
      {
        sim_data[0] = (uint8)tag;
        if (wms_nv_replace_gw_sms( index, sim_data ) == FALSE )
        {
          cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
        }
        else
        {
          /* SIM write was successful, update message list
          */
          cfg_s_ptr->nv_gw_tags[index] = tag;
        }
      }
      break;

#endif /* FEATURE_GWSMS */

    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR_1("Invalid memory store %d", mem_store);
      break;
  }

  /* send out message list event */
  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    wms_cfg_do_message_list(appInfo_ptr, mem_store, WMS_TAG_NONE, cmd_ptr);
//    wms_cfg_do_memory_status( mem_store, WMS_TAG_NONE );
  }

  return cmd_err;
} /* wms_msg_do_modify_tag() */

/*
*/
void wms_msg_modify_tag_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err   = WMS_CMD_ERR_NONE;

  cmd_err = wms_msg_do_modify_tag( cmd_ptr->hdr.appInfo_ptr,
                                   cmd_ptr->cmd.msg_modify_tag.mem_store,
                                   cmd_ptr->cmd.msg_modify_tag.index,
                                   cmd_ptr->cmd.msg_modify_tag.tag,
                                   cmd_ptr );

  wms_client_cmd_status(cmd_ptr, cmd_err);

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    msg_event_info.status_info.user_data                 = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id                 = (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.message.msg_hdr.index     = cmd_ptr->cmd.msg_modify_tag.index;
    msg_event_info.status_info.message.msg_hdr.mem_store = cmd_ptr->cmd.msg_modify_tag.mem_store;
    msg_event_info.status_info.message.msg_hdr.tag       = cmd_ptr->cmd.msg_modify_tag.tag;
    msg_event_info.status_info.as_id                     = cmd_ptr->hdr.as_id;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_MODIFY_TAG, &msg_event_info );
  }

  return;
} /* wms_msg_modify_tag_proc() */

/*
*/
void wms_msg_read_template_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  wms_prov_app_info_s_type   *appInfo_ptr;
  uint32                      i;
  wms_memory_store_e_type     mem_store;
#ifdef FEATURE_GWSMS
  wms_gw_template_s_type gw_template;
#endif /* FEATURE_GWSMS */
#ifdef FEATURE_CDSMS
  wms_cdma_template_s_type   *cdma_template_ptr = &msg_event_info.status_info.message.u.cdma_template;
#endif /* FEATURE_CDSMS */

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    cmd_err =  WMS_CMD_ERR_AS_ID;
    wms_client_cmd_status( cmd_ptr, cmd_err );
    return;
  }

  i             = cmd_ptr->cmd.msg_read_template.index;
  mem_store     = cmd_ptr->cmd.msg_read_template.mem_store;
#ifdef FEATURE_GWSMS
  memset( &gw_template, 0x00, sizeof(wms_gw_template_s_type) );
#endif /* FEATURE_GWSMS */


  switch( mem_store )
  {
#ifdef FEATURE_CDSMS

    case WMS_MEMORY_STORE_CSIM:
    case WMS_MEMORY_STORE_RUIM:
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;

      if (!wms_sim_mmgsdi_mem_store_supported(appInfo_ptr->mmgsdi_appInfo.app_type, mem_store))
      {
         cmd_err =  WMS_CMD_ERR_UNSUPPORTED;
         break;
      }
      else if ( i >= (uint32) appInfo_ptr->smsp_rec_count )
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
        MSG_ERROR_2( "Invalid template index %d (smsp_rec_count %d)",
                     i,
                     appInfo_ptr->smsp_rec_count );
      }
      else if (NULL == appInfo_ptr->smsp_raw_rec[i].data_ptr)
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_READ;
      }
      else if (wms_ts_cdma_decode_smsp( appInfo_ptr->smsp_raw_rec[i].data_ptr,
                                        (uint8)appInfo_ptr->smsp_raw_rec[i].data_len,
                                        cdma_template_ptr ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_DECODE;
      }
      break;

    case WMS_MEMORY_STORE_NV_CDMA:
       msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;

      //TBD: support multiple templates in the future
      if ( wms_nv_read_cdma_params( 0, ruim_data ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_READ;
      }
      else if (wms_ts_cdma_decode_smsp( ruim_data,
                                        255,
                                        cdma_template_ptr ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_DECODE;
      }
      break;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS

    case WMS_MEMORY_STORE_SIM:
    case WMS_MEMORY_STORE_USIM:
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;

      if ( i >= (uint32)appInfo_ptr->smsp_rec_count )
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
        MSG_ERROR_2( "Invalid template index %d (smsp_rec_count %d)",
                     i,
                     appInfo_ptr->smsp_rec_count );
      }
      else if (NULL == appInfo_ptr->smsp_raw_rec[i].data_ptr)
      {
        cmd_err = WMS_CMD_ERR_MSG_SIM_READ;
      }
      else
      {
         gw_template.alpha_id.data = (uint8 *) wms_mem_malloc((size_t) WMS_ALPHA_ID_MAX);

         if ( NULL == gw_template.alpha_id.data )
         {
           cmd_err = WMS_CMD_ERR_NO_RESOURCE;
           MSG_ERROR_0("malloc failed for alpha_id.data");
         }
         else if (wms_ts_decode_smsp( appInfo_ptr->smsp_raw_rec[i].data_ptr,
                                 (uint8)appInfo_ptr->smsp_raw_rec[i].data_len,
                                  &gw_template ) == FALSE )
         {
            cmd_err = WMS_CMD_ERR_MSG_DECODE;
         }
         else 
         {
           msg_event_info.status_info.message.u.gw_template = gw_template;  
         }
       }  
      break;

    case WMS_MEMORY_STORE_NV_GW:
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      gw_template.alpha_id.data = (uint8 *) wms_mem_malloc((size_t) WMS_ALPHA_ID_MAX);

      if ( NULL == gw_template.alpha_id.data )
      {
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        MSG_ERROR_0("malloc failed for alpha_id.data");
      }
      else if (wms_nv_read_gw_params( &gw_template, appInfo_ptr->asid ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_READ;
      }
      else 
      {
        msg_event_info.status_info.message.u.gw_template = gw_template;
      }
      break;

#endif /* FEATURE_GWSMS */

    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR_1("Invalid memory store %d", cmd_ptr->cmd.msg_write.message.msg_hdr.mem_store);
      break;
  } /* switch mem_store */

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    msg_event_info.status_info.user_data                  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id                  = (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.as_id                      = appInfo_ptr->asid;
    msg_event_info.status_info.message.msg_hdr.index      = i;
    msg_event_info.status_info.message.msg_hdr.mem_store  = mem_store;
    msg_event_info.status_info.message.msg_hdr.tag        = WMS_TAG_MO_TEMPLATE;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_READ_TEMPLATE, &msg_event_info );
  }

#ifdef FEATURE_GWSMS
    WMS_MEM_FREEIF(gw_template.alpha_id.data);
#endif /* FEATURE_GWSMS */

  return;
} /* wms_msg_read_template_proc() */

/*
*/
void wms_msg_write_template_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  wms_prov_app_info_s_type   *appInfo_ptr;
  wms_memory_store_e_type     mem_store;
  uint32                      msg_idx;
  wms_write_mode_e_type       write_mode;
  boolean                     sc_addr_is_changed = FALSE;
#ifdef FEATURE_GWSMS
  wms_gw_template_s_type     *template_ptr;
  uint8                      *local_handle = NULL;
#endif /* FEATURE_GWSMS */
#ifdef FEATURE_CDSMS
  wms_cdma_template_s_type   *cdma_template_ptr = &cmd_ptr->cmd.msg_write_template.message.u.cdma_template;
#endif /* FEATURE_CDSMS */

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     cmd_err =  WMS_CMD_ERR_AS_ID;
     wms_client_cmd_status( cmd_ptr, cmd_err );
     return;
  }

  msg_idx       = cmd_ptr->cmd.msg_write_template.message.msg_hdr.index;
  mem_store     = cmd_ptr->cmd.msg_write_template.message.msg_hdr.mem_store;
  write_mode    = cmd_ptr->cmd.msg_write_template.write_mode;

#ifdef FEATURE_GWSMS
  template_ptr  = &cmd_ptr->cmd.msg_write_template.message.u.gw_template;
#endif /* FEATURE_GWSMS */

  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

  case WMS_MEMORY_STORE_CSIM:
  case WMS_MEMORY_STORE_RUIM:
    if (!wms_sim_mmgsdi_mem_store_supported(appInfo_ptr->mmgsdi_appInfo.app_type, mem_store))
    {
      cmd_err =  WMS_CMD_ERR_UNSUPPORTED;
      break;
    }
    else if (write_mode == WMS_WRITE_MODE_INSERT)
    {
      for (msg_idx = 0; msg_idx < (uint32)appInfo_ptr->smsp_rec_count; msg_idx++)
      {
        if (appInfo_ptr->smsp_rec_templates[msg_idx] == FALSE)
        {
          break;
        }
      }

      if (msg_idx == (uint32)appInfo_ptr->smsp_rec_count)
      {
        cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
        wms_cfg_do_memory_full(appInfo_ptr->asid, mem_store);
      }
    }
    else if (write_mode == WMS_WRITE_MODE_REPLACE)
    {
      if (msg_idx >= (uint32)appInfo_ptr->smsp_rec_count)
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
      }
    }
    else
    {
      cmd_err = WMS_CMD_ERR_MSG_WRITE_MODE;
    }


    if (cmd_err == WMS_CMD_ERR_NONE)
    {
      if (wms_ts_cdma_encode_smsp(cdma_template_ptr,
                                  (uint8)appInfo_ptr->smsp_rec_len,
                                  ruim_data) == FALSE)
      {
        cmd_err = WMS_CMD_ERR_MSG_ENCODE;
      }
      else if (wms_sim_mmgsdi_write_data(appInfo_ptr,
                                         appInfo_ptr->smsp_file_enum,
                                         (mmgsdi_rec_num_type)msg_idx,
                                         ruim_data,
                                         (uint32)appInfo_ptr->smsp_rec_len) != MMGSDI_SUCCESS)
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
      }
      else if (appInfo_ptr->smsp_rec_templates[msg_idx] == FALSE)
      {
        appInfo_ptr->smsp_rec_templates[msg_idx] = TRUE;
      }

      if ( NULL == appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr )
      {
        appInfo_ptr->smsp_raw_rec[msg_idx].data_len = 0;
        appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr = (uint8 *)wms_mem_malloc((size_t)appInfo_ptr->smsp_rec_len);
      }

      if ( NULL != appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr )
      {
        (void)memscpy( (void *)appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr, appInfo_ptr->smsp_rec_len,
                       (void *)ruim_data, appInfo_ptr->smsp_rec_len);

        appInfo_ptr->smsp_raw_rec[msg_idx].data_len = appInfo_ptr->smsp_rec_len;
        MSG_HIGH_3( "smsp_raw_rec[%d] = 0x%08x, len = %d",
                    msg_idx,
                    appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr,
                    appInfo_ptr->smsp_rec_len );
      }
      else
      {
        MSG_ERROR_1("malloc failed for smsp_raw_rec[%d]", msg_idx);
      }

      wms_cfg_do_template_list(appInfo_ptr, mem_store, cmd_ptr);
    }
    break;

  case WMS_MEMORY_STORE_NV_CDMA:
    //TBD:  support multiple templates in the future
    {
      msg_idx = 0;
      if (wms_ts_cdma_encode_smsp( cdma_template_ptr,
                                   255,
                                   ruim_data ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_ENCODE;
      }
      else if (wms_nv_write_cdma_params( msg_idx, ruim_data ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
      }
    }
    break;

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS

  case WMS_MEMORY_STORE_USIM:
  case WMS_MEMORY_STORE_SIM:
    if (write_mode == WMS_WRITE_MODE_INSERT )
      {
        for( msg_idx = 0; msg_idx < (uint32)appInfo_ptr->smsp_rec_count; msg_idx++ )
        {
          if (appInfo_ptr->smsp_rec_templates[msg_idx] == FALSE )
          {
            break;
          }
        }

        if (msg_idx == (uint32) appInfo_ptr->smsp_rec_count )
        {
          cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
          wms_cfg_do_memory_full(appInfo_ptr->asid, mem_store);

        }
      }
      else if (write_mode == WMS_WRITE_MODE_REPLACE )
      {
        if (msg_idx >= (uint32) appInfo_ptr->smsp_rec_count )
        {
          cmd_err = WMS_CMD_ERR_MSG_INDEX;
        }
      }
      else
      {
        cmd_err = WMS_CMD_ERR_MSG_WRITE_MODE;
      }


      if (cmd_err == WMS_CMD_ERR_NONE )
      {
        if (wms_ts_encode_smsp( template_ptr,
                                (uint8) appInfo_ptr->smsp_rec_len,
                                 sim_data ) == FALSE )
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }
        else if (wms_sim_mmgsdi_write_data( appInfo_ptr,
                                            appInfo_ptr->smsp_file_enum,
                                            (mmgsdi_rec_num_type)msg_idx,
                                            sim_data,
                                            (uint32)appInfo_ptr->smsp_rec_len) != MMGSDI_SUCCESS)
        {
          cmd_err = WMS_CMD_ERR_MSG_SIM_WRITE;
        }
        else if (appInfo_ptr->smsp_rec_templates[msg_idx] == FALSE )
        {
          appInfo_ptr->smsp_rec_templates[msg_idx] = TRUE;
        }

        if ( NULL == appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr )
        {
          appInfo_ptr->smsp_raw_rec[msg_idx].data_len = 0;
          appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr = (uint8 *)wms_mem_malloc((size_t)appInfo_ptr->smsp_rec_len);
        }

        if ( NULL != appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr )
        {
          (void)memscpy( (void *)appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr, appInfo_ptr->smsp_rec_len,
                         (void *)sim_data, appInfo_ptr->smsp_rec_len);

          appInfo_ptr->smsp_raw_rec[msg_idx].data_len = appInfo_ptr->smsp_rec_len;
          MSG_HIGH_3( "smsp_raw_rec[%d] = 0x%08x, len = %d",
                      msg_idx,
                      appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr,
                      appInfo_ptr->smsp_rec_len );
        }
        else
        {
          MSG_ERROR_1("malloc failed for smsp_raw_rec[%d]", msg_idx);
        }

        wms_cfg_do_template_list(appInfo_ptr, mem_store, cmd_ptr);

        if (WMS_CMD_ERR_NONE == cmd_err )
        {
          MSG_HIGH_1( "wms_msg_write_template_proc(): deallocate sc address template buff - 0x%08x",
                      appInfo_ptr->gw_template.alpha_id.data );

          WMS_MEM_FREEIF(appInfo_ptr->gw_template.alpha_id.data);

          sc_addr_is_changed = wms_msg_check_sc_addr_is_changed(&template_ptr->sc_addr, &appInfo_ptr->gw_template.sc_addr);
		  
          appInfo_ptr->gw_template           = *template_ptr;
          appInfo_ptr->gw_template_index     = msg_idx;
          appInfo_ptr->gw_template_mem_store = mem_store;

          if (template_ptr->alpha_id.len > 0 && NULL != template_ptr->alpha_id.data)
          {
            appInfo_ptr->gw_template.alpha_id.data = (uint8 *)wms_mem_malloc((uint32)template_ptr->alpha_id.len);
          }
          else
          {
            appInfo_ptr->gw_template.alpha_id.data = NULL;
            appInfo_ptr->gw_template.alpha_id.len = 0;
          }

          if (NULL != appInfo_ptr->gw_template.alpha_id.data)
          {
            (void) memscpy( (void*)appInfo_ptr->gw_template.alpha_id.data,
                            template_ptr->alpha_id.len,
                            (void*)template_ptr->alpha_id.data,
                            template_ptr->alpha_id.len );
          }

        }
      }
      break;

    case WMS_MEMORY_STORE_NV_GW:
      if (wms_nv_write_gw_params( template_ptr, appInfo_ptr->asid ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
      }
      break;

#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if (cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
    msg_event_info.status_info.as_id      = appInfo_ptr->asid;
    msg_event_info.status_info.write_mode = write_mode;
    msg_event_info.status_info.message    = cmd_ptr->cmd.msg_write.message;

#ifdef FEATURE_GWSMS
    msg_event_info.status_info.message.u.gw_template.alpha_id.len = 0;
    msg_event_info.status_info.message.u.gw_template.alpha_id.data = NULL;

    if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.msg_write_template.message.msg_hdr.message_mode)
    {
      if ((NULL != template_ptr->alpha_id.data) && (template_ptr->alpha_id.len > 0))
      {
         // Alpha_ID Present; Hence Copy it to gw_template.alpha_id
         msg_event_info.status_info.message.u.gw_template.alpha_id.data =
           (uint8 *)wms_mem_malloc((size_t) template_ptr->alpha_id.len);

         if (NULL == msg_event_info.status_info.message.u.gw_template.alpha_id.data)
         {
           MSG_ERROR_0("Null ptr - msg_event_info - data");
           msg_event_info.status_info.message.u.gw_template.alpha_id.len = 0;
         }
         else
         {
           msg_event_info.status_info.message.u.gw_template.alpha_id.len = template_ptr->alpha_id.len;

           (void)memscpy( (void *)msg_event_info.status_info.message.u.gw_template.alpha_id.data,
                          template_ptr->alpha_id.len,
                          (void *)template_ptr->alpha_id.data,
                          template_ptr->alpha_id.len);
         }
      }
      local_handle = msg_event_info.status_info.message.u.gw_template.alpha_id.data; // Save copy
    }
#endif /* FEATURE_GWSMS */

    (void)wms_msg_event_notify( WMS_MSG_EVENT_WRITE_TEMPLATE, &msg_event_info );
    if (sc_addr_is_changed)
    {
      msg_event_info.status_info.client_id  = WMS_CLIENT_TYPE_INTERNAL;
      (void)wms_msg_event_notify_single_client( WMS_CLIENT_TYPE_QMI, WMS_MSG_EVENT_SMSC_ADDR_CHANGE, &msg_event_info );
    }
    // msg_event_info.status_info.message.u.gw_template.alpha_id.data may be tainted after return
  }

#ifdef FEATURE_GWSMS
  if (WMS_MESSAGE_MODE_GW == cmd_ptr->cmd.msg_write_template.message.msg_hdr.message_mode)
  {
    msg_event_info.status_info.message.u.gw_template.alpha_id.len = 0;
    msg_event_info.status_info.message.u.gw_template.alpha_id.data = NULL; // Squash untrustable pointer
    WMS_MEM_FREEIF(local_handle);                                          // Free using local copy

    template_ptr->alpha_id.len = 0;
    WMS_MEM_FREEIF(template_ptr->alpha_id.data);
  }
#endif /* FEATURE_GWSMS */

  return;
} /* wms_msg_write_template_proc() */


/*
*/
void wms_msg_delete_template_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type        cmd_err = WMS_CMD_ERR_NONE;
  wms_prov_app_info_s_type *appInfo_ptr;
  wms_memory_store_e_type   mem_store;
  wms_message_index_type    msg_idx;

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     cmd_err =  WMS_CMD_ERR_AS_ID;
     wms_client_cmd_status( cmd_ptr, cmd_err );
     return;
  }

  mem_store   = cmd_ptr->cmd.msg_delete_template.mem_store;
  msg_idx     = cmd_ptr->cmd.msg_delete_template.index;

  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

    case WMS_MEMORY_STORE_CSIM:
    case WMS_MEMORY_STORE_RUIM:
      if (!wms_sim_mmgsdi_mem_store_supported(appInfo_ptr->mmgsdi_appInfo.app_type, mem_store))
      {
        cmd_err = WMS_CMD_ERR_UNSUPPORTED;
        break;
      }
      else if (appInfo_ptr->smsp_rec_templates[msg_idx] == FALSE )
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty message %d", cmd_ptr->cmd.msg_delete_template.index);
      }
      else if (wms_sim_mmgsdi_delete_data( appInfo_ptr,
                                           appInfo_ptr->smsp_file_enum,
                                           (mmgsdi_rec_num_type)msg_idx,
                                           (uint32)appInfo_ptr->smsp_rec_len) != MMGSDI_SUCCESS )
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_DELETE;
      }
      else
      {
        appInfo_ptr->smsp_rec_templates[msg_idx] = FALSE;
        if ( NULL != appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr )
        {
          memset(appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr, 0xFF, appInfo_ptr->smsp_rec_len );
          appInfo_ptr->smsp_raw_rec[msg_idx].data_len = 0;
        }
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
      break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS

    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_SIM:
      if( appInfo_ptr->smsp_rec_templates[msg_idx] == FALSE )
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty message %d", cmd_ptr->cmd.msg_delete_template.index);
      }
      else if( wms_sim_mmgsdi_delete_data( appInfo_ptr,
                                           appInfo_ptr->smsp_file_enum,
                                           (mmgsdi_rec_num_type)msg_idx,
                                           (uint32)appInfo_ptr->smsp_rec_len) != MMGSDI_SUCCESS )
      {
        cmd_err = WMS_CMD_ERR_MSG_SIM_DELETE;
      }
      else
      {
        appInfo_ptr->smsp_rec_templates[msg_idx] = FALSE;
        if ( NULL != appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr )
        {
          memset(appInfo_ptr->smsp_raw_rec[msg_idx].data_ptr, 0xFF, appInfo_ptr->smsp_rec_len );
          appInfo_ptr->smsp_raw_rec[msg_idx].data_len = 0;
        }
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      break;

#endif /* FEATURE_GWSMS */

    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      break;
  }

  wms_client_cmd_status(cmd_ptr, cmd_err);

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data                  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id                  = (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.message.msg_hdr.index      = msg_idx;
    msg_event_info.status_info.message.msg_hdr.mem_store  = mem_store;
    msg_event_info.status_info.message.msg_hdr.tag        = WMS_TAG_MO_TEMPLATE;
    msg_event_info.status_info.as_id                      = appInfo_ptr->asid;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_DELETE_TEMPLATE, &msg_event_info );

    // send out CFG message list event
    wms_cfg_do_template_list(appInfo_ptr, mem_store, cmd_ptr);
  }

  return;
} /* wms_msg_delete_template_proc() */



/*
*/
void wms_msg_delete_template_all_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type        cmd_err = WMS_CMD_ERR_NONE;
  wms_prov_app_info_s_type* appInfo_ptr;
  wms_memory_store_e_type   mem_store;

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     cmd_err =  WMS_CMD_ERR_AS_ID;
     wms_client_cmd_status( cmd_ptr, cmd_err );
     return;
  }

  mem_store   = cmd_ptr->cmd.msg_delete_template.mem_store;

  MSG_HIGH_1("Deleting all templates: mem_store=%d", mem_store);

  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

    case WMS_MEMORY_STORE_CSIM:
    case WMS_MEMORY_STORE_RUIM:

      if (!wms_sim_mmgsdi_mem_store_supported(appInfo_ptr->mmgsdi_appInfo.app_type, mem_store))
      {
         cmd_err =  WMS_CMD_ERR_UNSUPPORTED;
         break;
      }

      {
        wms_message_index_type  idx;

        for (idx = 0; idx < (uint32) appInfo_ptr->smsp_rec_count; idx++ )
        {
          if( appInfo_ptr->smsp_rec_templates[idx] == FALSE )
          {
            continue;
          }

          if( wms_sim_mmgsdi_delete_data( appInfo_ptr,
                                          appInfo_ptr->smsp_file_enum,
                                          (mmgsdi_rec_num_type)idx,
                                          (uint32)appInfo_ptr->smsp_rec_len) != MMGSDI_SUCCESS )
          {
            cmd_err = WMS_CMD_ERR_MSG_RUIM_DELETE;
          }
          else
          {
            appInfo_ptr->smsp_rec_templates[idx] = FALSE;
            if ( NULL != appInfo_ptr->smsp_raw_rec[idx].data_ptr )
            {
              memset(appInfo_ptr->smsp_raw_rec[idx].data_ptr, 0xFF, appInfo_ptr->smsp_rec_len );
              appInfo_ptr->smsp_raw_rec[idx].data_len = 0;

              MSG_HIGH_3( "smsp_raw_rec[%d] = 0x%08x, len = %d",
                          idx,
                          appInfo_ptr->smsp_raw_rec[idx].data_ptr,
                          appInfo_ptr->smsp_rec_len );
            }
          }
        }
        msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
      }
      break;

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS

    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_SIM:
      {
        wms_message_index_type  idx;

        for (idx = 0; idx < (uint32) appInfo_ptr->smsp_rec_count; idx++ )
        {
          if (appInfo_ptr->smsp_rec_templates[idx] == FALSE )
          {
            continue;
          }

          if (wms_sim_mmgsdi_delete_data( appInfo_ptr,
                                          appInfo_ptr->smsp_file_enum,
                                          (mmgsdi_rec_num_type)idx,
                                          (uint32)appInfo_ptr->smsp_rec_len) != MMGSDI_SUCCESS )
          {
            cmd_err = WMS_CMD_ERR_MSG_SIM_DELETE;
          }
          else
          {
            appInfo_ptr->smsp_rec_templates[idx] = FALSE;
            if ( NULL != appInfo_ptr->smsp_raw_rec[idx].data_ptr )
            {
              memset(appInfo_ptr->smsp_raw_rec[idx].data_ptr, 0xFF, appInfo_ptr->smsp_rec_len );
              appInfo_ptr->smsp_raw_rec[idx].data_len = 0;

              MSG_HIGH_3( "smsp_raw_rec[%d] = 0x%08x, len = %d",
                          idx,
                          appInfo_ptr->smsp_raw_rec[idx].data_ptr,
                          appInfo_ptr->smsp_rec_len );
            }
          }
        }
        msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      }
      break;

#endif /* FEATURE_GWSMS */

    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      break;
  }

  wms_client_cmd_status(cmd_ptr, cmd_err);

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    msg_event_info.status_info.user_data                  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id                  = (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.message.msg_hdr.index      = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
    msg_event_info.status_info.message.msg_hdr.mem_store  = mem_store;
    msg_event_info.status_info.message.msg_hdr.tag        = WMS_TAG_MO_TEMPLATE;
    msg_event_info.status_info.as_id                      = appInfo_ptr->asid;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_DELETE_TEMPLATE_ALL, &msg_event_info );

    // send out CFG message list event
    wms_cfg_do_template_list(appInfo_ptr, mem_store, cmd_ptr);
  }

  return;
} /* wms_msg_delete_template_all_proc() */

/*===========================================================================
FUNCTION wms_msg_send_smsc_address_change_ind

DESCRIPTION
  Send request to QMI_WMS about SMSC address change

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_msg_send_smsc_address_change_ind(
  wms_prov_app_info_s_type   *appInfo_ptr
)
{
  // Notify SMSC address change event to QMI_WMS so that QMI_WMS global SMSC cache init happens.
  wms_msg_event_info_s_type         *msg_event_info_ptr = NULL;

  msg_event_info_ptr = (wms_msg_event_info_s_type *)wms_mem_malloc(sizeof(wms_msg_event_info_s_type));

  if (NULL == msg_event_info_ptr) 
  {
    MSG_ERROR_0( "msg_event_info_ptr is NULL" );
    return;
  }

  msg_event_info_ptr->status_info.user_data                       = NULL;
  msg_event_info_ptr->status_info.client_id                       = WMS_CLIENT_TYPE_INTERNAL;
  msg_event_info_ptr->status_info.as_id                           = appInfo_ptr->asid;
  msg_event_info_ptr->status_info.message.msg_hdr.message_mode    = WMS_MESSAGE_MODE_GW;
  msg_event_info_ptr->status_info.message.msg_hdr.tag             = WMS_TAG_MO_TEMPLATE;
  msg_event_info_ptr->status_info.message.msg_hdr.mem_store       = appInfo_ptr->gw_template_mem_store;
  msg_event_info_ptr->status_info.message.msg_hdr.index           = appInfo_ptr->gw_template_index;

  (void)memscpy( (void *)&(msg_event_info_ptr->status_info.message.u.gw_template),
                 sizeof(msg_event_info_ptr->status_info.message.u.gw_template),
                 (void *)&(appInfo_ptr->gw_template),
                 sizeof(appInfo_ptr->gw_template) );

  msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.len    = 0;
  msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.data   = NULL;

  if ((NULL != appInfo_ptr->gw_template.alpha_id.data) && (appInfo_ptr->gw_template.alpha_id.len > 0))
  {
    // Alpha_ID Present; Hence Copy it to gw_template.alpha_id
    msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.data =
      (uint8 *)wms_mem_malloc((size_t) appInfo_ptr->gw_template.alpha_id.len);

    if (NULL == msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.data)
    {
      MSG_ERROR_0( "Null ptr - msg_event_info - data" );
      msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.len = 0;
    }
    else
    {
      msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.len = appInfo_ptr->gw_template.alpha_id.len;

      (void)memscpy( (void *)msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.data,
                      appInfo_ptr->gw_template.alpha_id.len,
                      (void *)appInfo_ptr->gw_template.alpha_id.data,
                      appInfo_ptr->gw_template.alpha_id.len );
    }
  }

  wms_msg_event_notify_single_client( WMS_CLIENT_TYPE_QMI, WMS_MSG_EVENT_SMSC_ADDR_CHANGE, msg_event_info_ptr );

  WMS_MEM_FREEIF(msg_event_info_ptr->status_info.message.u.gw_template.alpha_id.data);
  WMS_MEM_FREEIF(msg_event_info_ptr);
} /* wms_msg_send_smsc_address_change_ind() */


#ifdef FEATURE_GWSMS
void wms_msg_read_sts_report_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  wms_prov_app_info_s_type   *appInfo_ptr;
  uint32                      idx;
  wms_memory_store_e_type     mem_store;

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    cmd_err =  WMS_CMD_ERR_AS_ID;
    wms_client_cmd_status( cmd_ptr, cmd_err );
    return;
  }

  idx           = cmd_ptr->cmd.msg_read_sts_report.index;
  mem_store     = cmd_ptr->cmd.msg_read_sts_report.mem_store;

  switch (mem_store)
  {
    case WMS_MEMORY_STORE_SIM:
    case WMS_MEMORY_STORE_USIM:
      if ( (idx >= (uint32)appInfo_ptr->smsr_rec_count) ||
           (appInfo_ptr->smsr_status_rpts[idx] == FALSE) )
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(idx);
        break;
      }

      if (wms_sim_mmgsdi_read_data( appInfo_ptr,
                                    appInfo_ptr->smsr_file_enum,
                                    (mmgsdi_rec_num_type)idx,
                                    appInfo_ptr->smsr_rec_len,
                                    sim_data,
                                    sizeof(sim_data)) != MMGSDI_SUCCESS)
      {
        cmd_err = WMS_CMD_ERR_MSG_SIM_READ;
      }
      else
      {
        sim_smsr.rec_id = sim_data[0];
        sim_smsr.len = (uint8) MIN(appInfo_ptr->smsr_rec_len-1, WMS_SIM_SMS_MAX-1);
        (void)memscpy( (void *)sim_smsr.data,
                       sim_smsr.len,
                       sim_data + 1,
                       sim_smsr.len);

        (void)wms_ts_unpack_sim_smsr(&sim_smsr, &raw_ts);

        msg_event_info.status_info.message.u.gw_message.is_broadcast = FALSE;
        msg_event_info.status_info.message.u.gw_message.raw_ts_data = raw_ts;
      }

      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      break;

    case WMS_MEMORY_STORE_RAM_GW:
      cmd_err =  wms_msg_map_gw_ram_status_rpt_index(appInfo_ptr->asid, idx, &idx);

      if ( (WMS_CMD_ERR_NONE != cmd_err) ||
           (FALSE == cfg_s_ptr->ram_status_rpts[idx]) )
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(cmd_ptr->cmd.msg_read.index);
        break;
      }

      (void)wms_ts_unpack_sim_smsr( &cfg_s_ptr->ram_gw_smsr[idx], &raw_ts);

      msg_event_info.status_info.message.u.gw_message.is_broadcast = FALSE;
      msg_event_info.status_info.message.u.gw_message.raw_ts_data  = raw_ts;
      msg_event_info.status_info.message.msg_hdr.message_mode      = WMS_MESSAGE_MODE_GW;
      break;

    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR_1("Invalid memory store %d", cmd_ptr->cmd.msg_write.message.msg_hdr.mem_store);
      break;
  } /* switch mem_store */

  wms_client_cmd_status(cmd_ptr, cmd_err);

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data                  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id                  = (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.as_id                      = appInfo_ptr->asid;
    msg_event_info.status_info.message.msg_hdr.index      = cmd_ptr->cmd.msg_read_sts_report.index;
    msg_event_info.status_info.message.msg_hdr.mem_store  = mem_store;
    msg_event_info.status_info.message.msg_hdr.tag        = WMS_TAG_STATUS_RPT;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_READ_STS_REPORT, &msg_event_info );
  }

  return;
} /* wms_msg_read_status_rpt_proc() */

wms_cmd_err_e_type wms_msg_do_write_sts_report(
  wms_prov_app_info_s_type     *appInfo_ptr,
  wms_write_mode_e_type         write_mode, /* IN */
  wms_message_index_type        rec_id,
  wms_client_message_s_type    *msg_ptr,   /* IN-OUT */
  wms_cmd_type                 *cmd_ptr  
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  uint32                      i;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return WMS_CMD_ERR_AS_ID;
  }

  if (NULL == msg_ptr)
  {
    MSG_HIGH_0("wms_msg_do_write_sts_report: NULL ptr");
    cmd_err = WMS_CMD_ERR_NULL_PTR;
    return cmd_err;
  }

  i = msg_ptr->msg_hdr.index;

  if ( (msg_ptr->u.gw_message.raw_ts_data.tpdu_type != WMS_TPDU_STATUS_REPORT) ||
      ((msg_ptr->u.gw_message.raw_ts_data.data[0] & 0x03) != WMS_MESSAGE_TYPE_STATUS_REPORT) )
  {
      return WMS_CMD_ERR_MSG_TPDU_TYPE;
  }

  if (wms_ts_decode(&msg_ptr->u.gw_message.raw_ts_data, &wms_msg_Gclient_ts) != WMS_OK_S)
  {
    return WMS_CMD_ERR_MSG_DECODE;
  }

  sim_smsr.rec_id = (uint8)rec_id + 1;

  switch(msg_ptr->msg_hdr.mem_store )
  {
    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_SIM:
      if (write_mode == WMS_WRITE_MODE_INSERT)
      {
        for (i = 0; i<(uint32)appInfo_ptr->smsr_rec_count; i++)
        {
          if (appInfo_ptr->smsr_status_rpts[i] == FALSE)
          {
            break;
          }
        }

        if (i == (uint32) appInfo_ptr->smsr_rec_count)
        {
          cmd_err = WMS_CMD_ERR_MSG_SIM_FULL;
        }
      } /* if INSERT */
      else if (write_mode == WMS_WRITE_MODE_REPLACE)
      {
        i = msg_ptr->msg_hdr.index;
        if (i >= (uint32)appInfo_ptr->smsr_rec_count)
        {
          cmd_err = WMS_CMD_ERR_MSG_INDEX;
        }
      }
      else
      {
        cmd_err = WMS_CMD_ERR_MSG_WRITE_MODE;
      }

      if (WMS_CMD_ERR_NONE == cmd_err)
      {
        /* prepare for the message to be written to SIM
        */
        sim_sms_unpacked.raw_ts_data =  msg_ptr->u.gw_message.raw_ts_data;

        if (wms_ts_pack_sim_smsr( & sim_sms_unpacked.raw_ts_data,
                                   & sim_smsr )
                != TRUE )
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }
        else
        {
          memset(sim_data, 0xFF, WMS_MAX_LEN);

          sim_data[0] = sim_smsr.rec_id;

          (void)memscpy( sim_data + 1,
                         sim_smsr.len,
                         (void *)sim_smsr.data,
                         sim_smsr.len );

          if (wms_sim_mmgsdi_write_data( appInfo_ptr,
                                         appInfo_ptr->smsr_file_enum,
                                         (mmgsdi_rec_num_type)i,
                                         sim_data,
                                         (uint32)appInfo_ptr->smsr_rec_len) != MMGSDI_SUCCESS)
          {
            cmd_err = WMS_CMD_ERR_MSG_SIM_WRITE;
          }
          else
          {
            /* SIM write was successful, update message list
            */
             msg_ptr->msg_hdr.index = i;
             appInfo_ptr->smsr_status_rpts[i] = TRUE;
          }
        }
      }
      break;

    case WMS_MEMORY_STORE_RAM_GW:
      if (write_mode == WMS_WRITE_MODE_INSERT)
      {
        cmd_err = wms_msg_find_free_gw_ram_status_rpt_index(appInfo_ptr->asid,
                                                            &msg_ptr->msg_hdr.index, &i);
      } /* if INSERT */
      else if (write_mode == WMS_WRITE_MODE_REPLACE)
      {
        i = msg_ptr->msg_hdr.index;
        cmd_err = wms_msg_map_gw_ram_status_rpt_index(appInfo_ptr->asid, i, &i);
      }
      else
      {
        cmd_err = WMS_CMD_ERR_MSG_WRITE_MODE;
      }

      if ( (WMS_CMD_ERR_NONE == cmd_err) && (i<WMS_MAX_RAM_STATUS_REPORTS) )
      {
        /* prepare for the message to be written to SIM
        */
        sim_sms_unpacked.raw_ts_data =  msg_ptr->u.gw_message.raw_ts_data;

        if (wms_ts_pack_sim_smsr( & sim_sms_unpacked.raw_ts_data,
                                   &cfg_s_ptr->ram_gw_smsr[i] )
                != TRUE )
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }
        else
        {
          /* RAM write was successful, update message list
          */
          cfg_s_ptr->ram_status_rpts[i] = TRUE;
        }
      }
      break;

    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
  }

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    wms_cfg_do_sts_report_list(appInfo_ptr, msg_ptr->msg_hdr.mem_store, cmd_ptr);
    if (WMS_WRITE_MODE_INSERT == write_mode)
    {
      wms_cfg_do_memory_status(appInfo_ptr, msg_ptr->msg_hdr.mem_store, WMS_TAG_STATUS_RPT, cmd_ptr);
    }
  }

  return cmd_err;
} /* wms_msg_do_write_sts_rpt() */

void wms_msg_write_sts_report_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type               cmd_err = WMS_CMD_ERR_NONE;
  wms_prov_app_info_s_type        *appInfo_ptr;
  wms_message_index_type           rec_id = 0;
  wms_gw_status_report_s_type     *status_report_ptr = NULL;
  wms_status_e_type                st;

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     cmd_err =  WMS_CMD_ERR_AS_ID;
     wms_client_cmd_status( cmd_ptr, cmd_err );
     return;
  }

  if (cmd_ptr->cmd.msg_write_sts_report.message.msg_hdr.tag != WMS_TAG_STATUS_RPT)
  {
    cmd_err = WMS_CMD_ERR_MSG_TAG;
    wms_client_cmd_status( cmd_ptr, cmd_err );
    return;
  }

  status_report_ptr = (wms_gw_status_report_s_type *)wms_mem_malloc(sizeof (wms_gw_status_report_s_type));

  if (NULL == status_report_ptr)
  {
     cmd_err = WMS_CMD_ERR_NO_RESOURCE;
     wms_client_cmd_status(cmd_ptr, cmd_err);
     return;
  }

  st = wms_ts_decode_status_report( &cmd_ptr->cmd.msg_write_sts_report.message.u.gw_message.raw_ts_data,
                                    status_report_ptr,
                                    NULL );

  if (st != WMS_OK_S)
  {
    MSG_ERROR_0("Status Report Decoding Failed ");
    cmd_err = WMS_CMD_ERR_MSG_DECODE;
    wms_client_cmd_status(cmd_ptr, cmd_err);
    wms_mem_free(status_report_ptr);
    return;
  }

  if (wms_sim_mmgsdi_mem_store_supported( appInfo_ptr->mmgsdi_appInfo.app_type,
                                          cmd_ptr->cmd.msg_write_sts_report.message.msg_hdr.mem_store) )
  {
    if (!wms_cfg_ms_locate_mo_msg_for_sts_report(appInfo_ptr->asid, &rec_id, status_report_ptr->message_reference ))
    {
      cmd_err = WMS_CMD_ERR_MSG_NO_MO_MSG;
      wms_client_cmd_status(cmd_ptr, cmd_err);
      wms_mem_free(status_report_ptr);
      return;
    }
  }

  /* rec_id has been assigned if cmd_err==NONE */
  cmd_err = wms_msg_do_write_sts_report( appInfo_ptr,
                                         cmd_ptr->cmd.msg_write_sts_report.write_mode,
                                         rec_id,
                                         &cmd_ptr->cmd.msg_write_sts_report.message,
                                         cmd_ptr);

  wms_client_cmd_status(cmd_ptr, cmd_err);

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
    msg_event_info.status_info.write_mode = cmd_ptr->cmd.msg_write_sts_report.write_mode;
    msg_event_info.status_info.message    = cmd_ptr->cmd.msg_write_sts_report.message;
    msg_event_info.status_info.as_id      = appInfo_ptr->asid;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_WRITE_STS_REPORT, &msg_event_info );
  }

  wms_mem_free(status_report_ptr);

  return;
} /* wms_msg_write_sts_report_proc() */

void wms_msg_del_sts_report_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type        cmd_err = WMS_CMD_ERR_NONE;
  wms_prov_app_info_s_type *appInfo_ptr;
  wms_memory_store_e_type   mem_store;
  wms_message_index_type    idx;

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     cmd_err =  WMS_CMD_ERR_AS_ID;
     wms_client_cmd_status( cmd_ptr, cmd_err );
     return;
  }

  mem_store   = cmd_ptr->cmd.msg_delete_sts_report.mem_store;
  idx         = cmd_ptr->cmd.msg_delete_sts_report.index;

  switch( mem_store )
  {
    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_SIM:
      if (idx >=(uint32)appInfo_ptr->smsr_rec_count)
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(idx);
        break;
      }

      if (appInfo_ptr->smsr_status_rpts[idx] == FALSE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", cmd_ptr->cmd.msg_delete_sts_report.index);
      }
      else if (wms_sim_mmgsdi_delete_data( appInfo_ptr,
                                           appInfo_ptr->smsr_file_enum,
                                           (mmgsdi_rec_num_type)idx,
                                           (uint32)appInfo_ptr->smsr_rec_len) != MMGSDI_SUCCESS )
      {
        cmd_err = WMS_CMD_ERR_MSG_SIM_DELETE;
      }
      else
      {
        appInfo_ptr->smsr_status_rpts[idx] = FALSE;
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      break;

    case WMS_MEMORY_STORE_RAM_GW:
      cmd_err = wms_msg_map_gw_ram_status_rpt_index(appInfo_ptr->asid, idx, &idx);

      if (WMS_CMD_ERR_NONE != cmd_err)
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        wms_check_valid_message_index(cmd_ptr->cmd.msg_delete.index);
        break;
      }

      if (cfg_s_ptr->ram_status_rpts[idx] == FALSE )
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR_1("Empty  message %d", cmd_ptr->cmd.msg_delete_sts_report.index);
      }
      else
      {
        cfg_s_ptr->ram_status_rpts[idx] = FALSE;
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      break;

    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      break;
  }

  wms_client_cmd_status(cmd_ptr, cmd_err);

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    msg_event_info.status_info.user_data                  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id                  = (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.message.msg_hdr.index      = idx;
    msg_event_info.status_info.message.msg_hdr.mem_store  = mem_store;
    msg_event_info.status_info.message.msg_hdr.tag        = WMS_TAG_STATUS_RPT;
    msg_event_info.status_info.as_id                      = appInfo_ptr->asid;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_DELETE_STS_REPORT, &msg_event_info );

    wms_cfg_do_sts_report_list(appInfo_ptr, mem_store, cmd_ptr);
    wms_cfg_do_memory_status(appInfo_ptr, mem_store, WMS_TAG_STATUS_RPT, cmd_ptr);
  }

  return;
} /* wms_msg_del_sts_report_proc() */

void wms_msg_del_sts_report_all_proc(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type        cmd_err = WMS_CMD_ERR_NONE;
  wms_prov_app_info_s_type *appInfo_ptr;
  wms_memory_store_e_type   mem_store;
  wms_message_index_type    idx;

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     cmd_err =  WMS_CMD_ERR_AS_ID;
     wms_client_cmd_status( cmd_ptr, cmd_err );
     return;
  }

  mem_store = cmd_ptr->cmd.msg_delete_sts_report.mem_store;

  MSG_HIGH_1("Deleting all Status Reports: mem_store=%d", mem_store);

  switch (mem_store)
  {
    case WMS_MEMORY_STORE_USIM:
    case WMS_MEMORY_STORE_SIM:
      for (idx = 0; idx < (uint32)appInfo_ptr->sms_rec_count; idx++)
      {
        if (appInfo_ptr->smsr_status_rpts[idx] == FALSE)
        {
          continue;
        }

        if (wms_sim_mmgsdi_delete_data( appInfo_ptr,
                                        appInfo_ptr->smsr_file_enum,
                                        (mmgsdi_rec_num_type)idx,
                                        (uint32)appInfo_ptr->smsr_rec_len) != MMGSDI_SUCCESS )
        {
          cmd_err = WMS_CMD_ERR_MSG_SIM_DELETE;
        }
        else
        {
          appInfo_ptr->smsr_status_rpts[idx] = FALSE;
        }
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      break;

    case WMS_MEMORY_STORE_RAM_GW:
      for (idx = 0; idx < WMS_MAX_RAM_STATUS_REPORTS; idx++)
      {
        cfg_s_ptr->ram_status_rpts[idx] = FALSE;
      }
      msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      break;

    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      break;
  }

  wms_client_cmd_status(cmd_ptr, cmd_err);

  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    msg_event_info.status_info.user_data                  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id                  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
    msg_event_info.status_info.message.msg_hdr.index      = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
    msg_event_info.status_info.message.msg_hdr.mem_store  = mem_store;
    msg_event_info.status_info.message.msg_hdr.tag        = WMS_TAG_STATUS_RPT;
    msg_event_info.status_info.as_id                      = appInfo_ptr->asid;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_DELETE_STS_REPORT_ALL, &msg_event_info );

    wms_cfg_do_sts_report_list(appInfo_ptr, mem_store, cmd_ptr);
    wms_cfg_do_memory_status(appInfo_ptr, mem_store, WMS_TAG_STATUS_RPT, cmd_ptr);
  }

  return;
} /* wms_msg_delete_sts_report_all_proc() */

#endif /* FEATURE_GWSMS */


#ifdef FEATURE_GSTK
/*
*/
void wms_msg_gstk_evt_proc(
  wms_cmd_type    *cmd_ptr
)
{
#ifdef FEATURE_GWSMS_MO_CONTROL
  uint32                      rec_index;
  gstk_mo_sms_ctrl_rsp_type  *mo_control_rsp_ptr;
  wms_gw_message_s_type      *gw_msg_ptr;
  wms_report_status_e_type    wms_report_status  = WMS_RPT_OK;
  wms_mo_ctrl_status_e_type   mo_ctrl_status     = WMS_MO_CTRL_STATUS_NO_MOD;
  wms_prov_app_info_s_type   *appInfo_ptr        = NULL;
  uint8                       index;

  if (GSTK_MO_SMS_CTRL_IND_RSP != cmd_ptr->cmd.msg_gstk_evt.gstk_evt.hdr_cmd.command_id)
  {
    /* IGNORE events not interested in */
    return;
  }

  mo_control_rsp_ptr = &cmd_ptr->cmd.msg_gstk_evt.gstk_evt.cmd.mo_sms_cc_rsp;

  /* --------- Processing GSTK_MO_SMS_CTRL_IND_RSP ---------- */

  rec_index = cmd_ptr->cmd.msg_gstk_evt.gstk_evt.hdr_cmd.user_data;

  if (NULL == msg_s_ptr->gw_mo_rec[rec_index])
  {
    MSG_ERROR_1("Inactive MO record at index %d", rec_index);
    return;
  }

  if (FALSE == msg_s_ptr->gw_mo_rec[rec_index]->mo_control_pending)
  {
    MSG_ERROR_1("MO control not pending for MO record at index %d", rec_index);
    return;
  }

  appInfo_ptr = msg_s_ptr->gw_mo_rec[rec_index]->appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }
  
  if (FALSE == appInfo_ptr->mo_waiting_for_gstk_resp)
  {
    MSG_ERROR_0("Invaild state, WMS is not expecting GSTK_MO_SMS_CTRL_IND_RSP ");
    return;
  }

  msg_s_ptr->gw_mo_rec[rec_index]->mo_control_pending = FALSE;
  appInfo_ptr->mo_waiting_for_gstk_resp               = FALSE;  // Unlock the processing of incoming MO control MO messages..

  gw_msg_ptr = &msg_s_ptr->gw_mo_rec[rec_index]->mo_control_cl_msg.u.gw_message;

  if (GSTK_ENVELOPE_CMD_SUCCESS == mo_control_rsp_ptr->general_resp)
  {
    if (WMS_OK_S != wms_ts_decode(&gw_msg_ptr->raw_ts_data, &wms_msg_Gclient_ts))
    {
      wms_report_status = WMS_RPT_MO_CONTROL_ERROR;
    }
    else
    {
      MSG_HIGH_1("MO SMS Call Control result from GSTK: %d", mo_control_rsp_ptr->mo_sms_ctrl_result);
      switch (mo_control_rsp_ptr->mo_sms_ctrl_result)
      {
        case GSTK_ALLOWED_NO_MOD:
          break;

        case GSTK_NOT_ALLOWED:
          mo_ctrl_status = WMS_MO_CTRL_STATUS_DISALLOWED;
          wms_report_status = WMS_RPT_MO_CONTROL_DISALLOW;
          break;

        case GSTK_ALLOWED_BUT_MOD:
          mo_ctrl_status = WMS_MO_CTRL_STATUS_MOD;

          if (FALSE == wms_ts_convert_from_gstk_address(&mo_control_rsp_ptr->rp_dest_address,
                                                        &gw_msg_ptr->sc_address))
          {
            wms_report_status = WMS_RPT_MO_CONTROL_ERROR;
          }
          else if (FALSE == wms_ts_convert_from_gstk_address(&mo_control_rsp_ptr->tp_dest_address,
                                                             &wms_msg_Gclient_ts.u.gw_pp.u.submit.address))
          {
            wms_report_status = WMS_RPT_MO_CONTROL_ERROR;
          }
          else if (WMS_OK_S != wms_ts_encode(& wms_msg_Gclient_ts, &gw_msg_ptr->raw_ts_data))
          {
            wms_report_status = WMS_RPT_MO_CONTROL_ERROR;
          }
          break;

        default:
          wms_report_status = WMS_RPT_MO_CONTROL_ERROR;
          break;
      } /* switch */
    }
  }
  else
  {
    wms_report_status = WMS_RPT_MO_CONTROL_ERROR;
  }
  if ( (WMS_RPT_OK == wms_report_status) &&
        wms_msg_suspend_ps_call_is_supported() )
  {
    cm_call_cmd_err_e_type suspend_status;

    suspend_status = wms_cm_suspend_pscall(msg_s_ptr->gw_mo_rec[rec_index]->as_id);

    if (CM_CALL_CMD_ERR_NOERR != suspend_status)
    {
      /* fall through to send submit report event. */
      wms_report_status = WMS_RPT_NETWORK_NOT_READY;
    }
  }

  if (WMS_RPT_OK == wms_report_status)
  {
    /* Save the MO SMS Call Control Information to submit report to be used later. */
    msg_s_ptr->gw_mo_rec[rec_index]->prev_report.alpha_id.len = mo_control_rsp_ptr->alpha.length;
    msg_s_ptr->gw_mo_rec[rec_index]->prev_report.mo_ctrl_status = mo_ctrl_status;

    if (mo_control_rsp_ptr->alpha.length > 0)
    {
       msg_s_ptr->gw_mo_rec[rec_index]->prev_report.alpha_id.data = (uint8 *)wms_mem_malloc((size_t) mo_control_rsp_ptr->alpha.length);

       if (NULL == msg_s_ptr->gw_mo_rec[rec_index]->prev_report.alpha_id.data)
       {
         MSG_ERROR_0("Could not allocate buffer for msg_s_ptr alpha");
         msg_s_ptr->gw_mo_rec[rec_index]->prev_report.alpha_id.len = 0;
       }
       else
       {
         (void)memscpy( (void *)msg_s_ptr->gw_mo_rec[rec_index]->prev_report.alpha_id.data,
                         mo_control_rsp_ptr->alpha.length,
                        (void *)mo_control_rsp_ptr->alpha.text,
                         mo_control_rsp_ptr->alpha.length);
       }
    }
    else
    {
      msg_s_ptr->gw_mo_rec[rec_index]->prev_report.alpha_id.data = NULL;
    }

    /* Send message to lower layers
    */
    if (WMS_CMD_ERR_NONE == wms_cm_gw_send_rl_data( msg_s_ptr->gw_mo_rec[rec_index],
                                                    &gw_msg_ptr->sc_address,
                                                    (uint8)wms_msg_Gclient_ts.u.gw_pp.u.submit.message_reference,
                                                    &gw_msg_ptr->raw_ts_data))
    {
      uint8 *local_handle = NULL;

      /* send event to clients with new msg addresses
      */
      msg_event_info.status_info.user_data            = msg_s_ptr->gw_mo_rec[rec_index]->user_data;
      msg_event_info.status_info.send_mode            = msg_s_ptr->gw_mo_rec[rec_index]->send_mode;
      msg_event_info.status_info.message              = msg_s_ptr->gw_mo_rec[rec_index]->mo_control_cl_msg;
      msg_event_info.status_info.message.u.gw_message = *gw_msg_ptr;
      msg_event_info.status_info.client_id            = msg_s_ptr->gw_mo_rec[rec_index]->client_id;
      msg_event_info.status_info.alpha_id.len         = mo_control_rsp_ptr->alpha.length;
      if (mo_control_rsp_ptr->alpha.length > 0)
      {
         msg_event_info.status_info.alpha_id.data = (uint8 *)wms_mem_malloc((size_t) mo_control_rsp_ptr->alpha.length);

         if (NULL == msg_event_info.status_info.alpha_id.data)
         {
           MSG_ERROR_0("null ptr - could not allocate buffer for msg_event_info data.");
           wms_report_status = WMS_RPT_OUT_OF_RESOURCES;
         }
         else
         {
            (void)memscpy( (void *)msg_event_info.status_info.alpha_id.data,
                           mo_control_rsp_ptr->alpha.length,
                           (void *)mo_control_rsp_ptr->alpha.text,
                           mo_control_rsp_ptr->alpha.length);
         }
      }
      else
      {
         msg_event_info.status_info.alpha_id.data = NULL;
      }

      local_handle = msg_event_info.status_info.alpha_id.data; // Save copy

      if (WMS_RPT_OK == wms_report_status)
      {
         msg_event_info.status_info.mo_ctrl_status = mo_ctrl_status;
         msg_event_info.status_info.as_id = msg_s_ptr->gw_mo_rec[rec_index]->as_id;
         (void)wms_msg_event_notify( WMS_MSG_EVENT_SEND, &msg_event_info );
      }

      msg_event_info.status_info.alpha_id.len = 0;
      msg_event_info.status_info.alpha_id.data = NULL; // Squash untrustable pointer
      WMS_MEM_FREEIF(local_handle);                    // Free using local trusworthy copy
    }
    else
    {
      /* fall through to send submit report event. */
      wms_report_status = WMS_RPT_GENERAL_ERROR;
    }
  }

  if (WMS_RPT_OK != wms_report_status)
  {
     uint8 *local_handle = NULL;

    /* send submit report event to clients
    */
    msg_event_info.submit_report_info.report_status   = wms_report_status;
    msg_event_info.submit_report_info.mo_ctrl_status  = mo_ctrl_status;
    msg_event_info.submit_report_info.user_data       = msg_s_ptr->gw_mo_rec[rec_index]->user_data;
    msg_event_info.submit_report_info.client_id       = msg_s_ptr->gw_mo_rec[rec_index]->client_id;
    msg_event_info.submit_report_info.message_mode    = WMS_MESSAGE_MODE_GW;
    msg_event_info.submit_report_info.alpha_id.len    = mo_control_rsp_ptr->alpha.length;

    if (mo_control_rsp_ptr->alpha.length > 0)
    {
       msg_event_info.submit_report_info.alpha_id.data = (uint8 *)wms_mem_malloc((size_t) mo_control_rsp_ptr->alpha.length);

       if (NULL == msg_event_info.submit_report_info.alpha_id.data)
       {
         MSG_ERROR_0("Null ptr - msg_event_info - data");
         msg_event_info.submit_report_info.alpha_id.len = 0;
       }
       else
       {
         (void)memscpy( (void *)msg_event_info.submit_report_info.alpha_id.data,
                        mo_control_rsp_ptr->alpha.length,
                        (void *)mo_control_rsp_ptr->alpha.text,
                        mo_control_rsp_ptr->alpha.length);
       }
    }
    else
    {
        msg_event_info.submit_report_info.alpha_id.data = NULL;
    }
    msg_event_info.submit_report_info.as_id                      = msg_s_ptr->gw_mo_rec[rec_index]->as_id;
    msg_event_info.submit_report_info.delivery_failure_permanent = TRUE;

    local_handle = msg_event_info.submit_report_info.alpha_id.data; // Save copy

    /* clear the mo msg record
    */
    wms_msg_gw_clear_mo_msg_rec(rec_index);

    /* No retry for MO CONTROL errors */
    (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT, &msg_event_info);

    msg_event_info.submit_report_info.alpha_id.len  = 0;
    msg_event_info.submit_report_info.alpha_id.data = NULL; // Squash untrustable pointer
    WMS_MEM_FREEIF(local_handle);                           // Free using trustworthy local copy
  }

  // Check for any MO message whose MO control processing is delayed.
  for (index = 0; index < WMS_MAX_MO_MESSAGES; index++)
  {
    if ( (NULL != msg_s_ptr->gw_mo_rec[index]) &&
         (TRUE == msg_s_ptr->gw_mo_rec[index]->mo_control_pending) )
    {      
      break;
    }	
  }

  if (WMS_MAX_MO_MESSAGES!= index)
  {
    msg_s_ptr->gw_mo_rec[index]->retry_wait_time = (sint31)1; // Retry after 1 second.
  } 
  
#endif /* FEATURE_GWSMS_MO_CONTROL */

  return;
} /* wms_msg_gstk_evt_proc() */

#endif /* FEATURE_GSTK */


#ifdef FEATURE_GWSMS

/*===========================================================================
FUNCTION wms_msg_notify_SMMA

DESCRIPTION
  Send request to lower layers in order to notify the network that memory
  is available to receive messages.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_msg_notify_SMMA(
  wms_prov_app_info_s_type   *appInfo_ptr,
  boolean                     retry
)
{
  wms_cmd_err_e_type                cmd_err = WMS_CMD_ERR_NONE;
  boolean                           mem_available;
  uint32                            rec_index;
  boolean                           new_req = TRUE;
  uint8                             mem_full_flag;

  if (NULL == appInfo_ptr) 
  {
    return;
  }

  /* Don't send SMMA when EF-SMSS is not present in the card. Spec TS 51.011 specifies
  ** that the provision of EF-SMSS is associated with EF-SMS. Both files shall be present
  ** together, or both absent from the SIM.
   
  ** SMMA is effectively disabled in this condition.
  */
  if (0 == appInfo_ptr->sms_rec_count)
  {
    MSG_HIGH("SMMA not sent: sms_rec_count = %d", appInfo_ptr->sms_rec_count, 0, 0);
    return;
  }

  mem_full_flag = appInfo_ptr->smss_mem_full_flag;

  if (appInfo_ptr->sim_init_complete == FALSE)
  {
    return;
  }

  if ((MMGSDI_APP_SIM  != appInfo_ptr->mmgsdi_appInfo.app_type) &&
      (MMGSDI_APP_USIM != appInfo_ptr->mmgsdi_appInfo.app_type))
  {
    MSG_ERROR_0("Wrong app type");
    return;
  }

  mem_available = FALSE;

  /**
  CASE 1 : If memory is available AND
           If Memory Full Flag is set THEN
              Send SMMA request, ABORT = FALSE


  CASE 2: If memory is not available AND
          If flag is set AND
          If SMMA request was sent earlier THEN
             Send SMMA Abort, ABORT = TRUE (Implying that the request was sent before the flag was cleared)

  */

  mem_available = !wms_msg_total_memory_full(appInfo_ptr);/*Will return TRUE if memory is full*/

  /*CASE-1*/
  /* Found a free record. Memory is not full any more.
  ** Send notification to the network.
  */

  if (mem_available)
  {
    if (0 == (mem_full_flag & (uint8)WMS_SIM_MEM_FULL_MASK))
    {

      if (wms_msg_map_mo_record(appInfo_ptr->asid, WMS_MSG_SMMA_REQUEST_ID, & rec_index) == FALSE )
      {
         /* if SMMA request has not been sent before, create a new record */
         if (wms_msg_get_free_mo_record( & rec_index ) == TRUE )
         {
            msg_s_ptr->gw_mo_rec[rec_index]->smi             = WMS_MSG_SMMA_REQUEST_ID;
            msg_s_ptr->gw_mo_rec[rec_index]->appInfo_ptr     = appInfo_ptr;
            MSG_HIGH_1("SMMA entry added: retry_time_left %d", msg_s_ptr->gw_mo_rec[rec_index]->retry_time_left);
            new_req = TRUE;
         }
         else
         {
           MSG_ERROR_0("No Free MO Records Available");
           cmd_err = WMS_CMD_ERR_NO_RESOURCE;
           return;
         }
      }
      else
      {
         new_req = FALSE;
      }

      if ((TRUE == retry) || (TRUE == new_req))
      {
        wms_msg_transport_s_type* transport_ptr = wms_msg_choose_registered_transport( appInfo_ptr->asid );
        wms_msg_transport_type_e_type transport_type = WMS_MSG_TRANSPORT_TYPE_CS;
        if (transport_ptr != NULL && transport_ptr->is_mn_registered)
        {
          transport_type = WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS;
        }

         /* Send SMMA Request */
         MSG_HIGH_0("Requesting SMMA");

         msg_s_ptr->gw_mo_rec[rec_index]->as_id  = appInfo_ptr->asid;

         cmd_err = wms_cm_gw_send_rl_memory_available( msg_s_ptr->gw_mo_rec[rec_index], FALSE, transport_type);

         if (WMS_CMD_ERR_NONE == cmd_err)
         {
            /* Set the Global Retry Timer if needed */
            if (msg_s_ptr->mo_retry_period > 0)
            {
               wms_set_retry_timer();
            }
         }
         else
         {
            /*Clear the Record*/
            wms_msg_gw_clear_mo_msg_rec(rec_index);
         }
      }
      else
      {
         MSG_HIGH_0("SMMA was sent before");
      }
    }
    else
    {
       MSG_HIGH_1("mem_full_flag = %d", mem_full_flag);
    }
  }
  /*CASE 2- Memory is not available*/
  else
  {
    /*CASE 2*/
    if (0 == (mem_full_flag & (uint8)WMS_SIM_MEM_FULL_MASK))
    {
      wms_msg_transport_s_type* transport_ptr = wms_msg_choose_registered_transport( appInfo_ptr->asid );
      wms_msg_transport_type_e_type transport_type = WMS_MSG_TRANSPORT_TYPE_CS;
      if (transport_ptr != NULL && transport_ptr->is_mn_registered)
      {
        transport_type = WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS;
      }
        
      if (wms_msg_map_mo_record(appInfo_ptr->asid, WMS_MSG_SMMA_REQUEST_ID, &rec_index) == FALSE )
      {
        /* The SMMA was not sent at all and memory is still not available.
        */
        return;
      }

      /*Memory is now not available again. Send Abort*/
      MSG_HIGH_0("Requesting SMMA abort");
      cmd_err = wms_cm_gw_send_rl_memory_available( msg_s_ptr->gw_mo_rec[rec_index], TRUE, transport_type);

      if (WMS_CMD_ERR_NONE == cmd_err)
      {
        /*Clear the Record*/
        wms_msg_gw_clear_mo_msg_rec(rec_index);
      }
    }
    else
    {
       MSG_HIGH_1("mem_full_flag = %d", mem_full_flag);
    }
  }

  MSG_HIGH_1("wms_msg_notify_SMMA() result: %d", cmd_err);

  return;
} /* wms_msg_notify_SMMA() */

/*===========================================================================
FUNCTION wms_msg_clear_sim_mem_full_flag

DESCRIPTION
  Reset SIM_MEM_FULL_FLAG.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_msg_clear_sim_mem_full_flag(
  wms_prov_app_info_s_type  *appInfo_ptr
)
{
  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return;
  }

  if (0 == (appInfo_ptr->smss_mem_full_flag & (uint8)WMS_SIM_MEM_FULL_MASK) )
  {
    appInfo_ptr->smss_mem_full_flag |= (uint8)WMS_SIM_MEM_FULL_CLEAR;
    (void)wms_cfg_get_sim_next_message_number(appInfo_ptr);
  }

  return;
} /* wms_msg_clear_sim_mem_full_flag() */

/*===========================================================================
FUNCTION wms_msg_SMMA_CONF_ind_proc

DESCRIPTION
  Process confirmation from lower layer about SMMA request.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_msg_SMMA_CONF_ind_proc(
  sys_modem_as_id_e_type    asid,          // The asid is from lower layer, OK as parameter
  wms_report_status_e_type  error_status,
  uint8                     cause_value
)
{
   uint32                    rec_idx;
   boolean                   clear_rec = TRUE;

   MSG_HIGH_2( "Got SMMA_CONF Status=%d, Cause=%d",
               error_status,
               cause_value );

   /* Find the matching record */
   if (FALSE == wms_msg_map_mo_record(asid, WMS_MSG_SMMA_REQUEST_ID, &rec_idx))
   {
      /* Unrecognized report, discard.*/
      MSG_ERROR_1("Unrecognized SMMA rpt ind: SMI %d not found", WMS_MSG_SMMA_REQUEST_ID);
      return;
   }

   /*RP Error - NO_ERROR_RP_ACK    -  Clear the Flag*/
   /*RP Error - Permanent          -  Clear the Flag*/
   /*RP Error - Temporary Error/LL Error -  Keep the Flag and retry */

   switch (error_status) {

      case WMS_RPT_RP_ERROR:
      /*if rp_cause is WMS_RP_CAUSE_TEMPORARY_FAILURE, do nothing, keep the flag
      ** else if Memory is still not available keep the flag else clear the flag */
         if (cause_value == (int)WMS_RP_CAUSE_TEMPORARY_FAILURE)
         {
            if ((msg_s_ptr->mo_retry_period > 0) &&
                (msg_s_ptr->gw_mo_rec[rec_idx]->retry_time_left > (sint31)msg_s_ptr->mo_retry_interval))
            {
               /* Set the retry_wait_time */
               msg_s_ptr->gw_mo_rec[rec_idx]->retry_wait_time = (sint31)msg_s_ptr->mo_retry_interval;
               MSG_HIGH_2( "SMMA retry_time_left=%d retry_wait_time=%d",
                           msg_s_ptr->gw_mo_rec[rec_idx]->retry_time_left,
                           msg_s_ptr->gw_mo_rec[rec_idx]->retry_wait_time );
               clear_rec = FALSE;
            }
         }

         break;

      case WMS_RPT_LL_ERROR:
      case WMS_RPT_NO_RESPONSE_FROM_NETWORK:
         if ((msg_s_ptr->mo_retry_period > 0) &&
             (msg_s_ptr->gw_mo_rec[rec_idx]->retry_time_left > (sint31)msg_s_ptr->mo_retry_interval))
         {
            /* Set the retry_wait_time */
            msg_s_ptr->gw_mo_rec[rec_idx]->retry_wait_time = (sint31)msg_s_ptr->mo_retry_interval;
            MSG_HIGH_2( "SMMA retry_time_left=%d retry_wait_time=%d",
                        msg_s_ptr->gw_mo_rec[rec_idx]->retry_time_left,
                        msg_s_ptr->gw_mo_rec[rec_idx]->retry_wait_time);

            clear_rec = FALSE;
         }
         break;

      /* Got Positive Ack, if Memory is still not available keep the flag else
       * clear the flag */
      case WMS_RPT_OK:
         break;

      default: /*Clear the flag for all the other cases*/
         break;
   }

   if (TRUE == clear_rec)
   {
      wms_prov_app_info_s_type *appInfo_ptr;

      appInfo_ptr = msg_s_ptr->gw_mo_rec[rec_idx]->appInfo_ptr;
      // If these repeat procedures fail, the mobile shall unset the memory capacity exceeded notification flag 
      // in the (U)SIM and exit this procedure.
      wms_msg_clear_sim_mem_full_flag( appInfo_ptr );
      /* Clear the msg record */
      wms_msg_gw_clear_mo_msg_rec( rec_idx );
   }

   return;
} /* wms_msg_SMMA_CONF_ind_proc() */

#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS

wms_report_status_e_type wms_msg_cdma_map_report_status
(
  wms_status_e_type  st
)
{
  switch( st )
  {
    case WMS_OK_S:
      return WMS_RPT_OK;

    case WMS_OUT_OF_RESOURCES_S:
      return WMS_RPT_OUT_OF_RESOURCES;


    case WMS_NETWORK_NOT_READY_S:
      return WMS_RPT_NETWORK_NOT_READY;

    case WMS_PHONE_NOT_READY_S:
      return WMS_RPT_PHONE_NOT_READY;

    case WMS_NOT_ALLOWED_IN_AMPS_S:
      return WMS_RPT_NOT_ALLOWED_IN_AMPS;

    case WMS_NETWORK_FAILURE_S:
      return WMS_RPT_LL_ERROR;

    case WMS_ACCESS_TOO_LARGE_S:
      return WMS_RPT_ACCESS_TOO_LARGE;

    case WMS_DTC_TOO_LARGE_S:
      return WMS_RPT_DC_TOO_LARGE;

    case WMS_ACCESS_BLOCK_S:
      return WMS_RPT_ACCESS_BLOCK;

    case WMS_SIP_PERM_ERROR_S:
      return WMS_RPT_SIP_PERM_ERROR;

    case WMS_SIP_TEMP_ERROR_S:
      return WMS_RPT_SIP_TEMP_ERROR;

    case WMS_MO_PREF_CHN_NOT_AVAIL_S:
      return WMS_RPT_PREF_CHN_NOT_AVAIL_ERROR;

    case WMS_1X_AND_VOICE_NOT_ALLOWED_S:
      return WMS_RPT_1X_AND_VOICE_NOT_ALLOWED;

    case WMS_TRANSPORT_NOT_READY_S:
      return WMS_RPT_TRANSPORT_NOT_READY;

    case WMS_TRANSPORT_NOT_ALLOWED_S:
      return WMS_RPT_TRANSPORT_NOT_ALLOWED;

    case WMS_IMS_NOT_ALLOWED_IN_SRLTE_S:
      return WMS_RPT_IMS_NOT_ALLOWED_IN_SRLTE;

    case WMS_INVALID_TPDU_TYPE_S:
    case WMS_INVALID_VALIDITY_FORMAT_S:
    case WMS_MT_MSG_FAILED_S: /* internal use */
    case WMS_INVALID_TRANSACTION_ID_S:
    case WMS_INVALID_FORMAT_S:
    case WMS_GENERAL_ERROR_S:
    case WMS_INVALID_PARM_SIZE_S:
    case WMS_INVALID_USER_DATA_SIZE_S:
    case WMS_INVALID_PARM_VALUE_S:
    case WMS_MISSING_PARM_S:
    default:
      return WMS_RPT_GENERAL_ERROR;
  }

} /* wms_msg_cdma_map_report_status() */

/*=========================================================================
FUNCTION
  wms_msg_auto_dc_send

DESCRIPTION
  Automatically send TCH_ONLY message when DC is connected.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_auto_dc_send(
  void
)
{
  wms_status_e_type     st;
  wms_msg_cdma_message_record_type *rec_ptr;
  boolean first_tch_sms_found = FALSE;

  rec_ptr = msg_s_ptr->cdma_mo_rec.head_ptr;
  while ( NULL != rec_ptr )
  {
    if ( (WMS_MSG_STATE_QUEUED == rec_ptr->state) && 
         (WMS_CLIENT_CH_PREF_TCH_ONLY == rec_ptr->ch_pref) )
    {
      if ( !first_tch_sms_found ) 
      {
        first_tch_sms_found = TRUE;
        MSG_HIGH_2( "wms_msg_auto_dc_send(): sending large msg rec 0x%x with seg_num %d",
                    rec_ptr,
                    rec_ptr->seq_num );

        /* Send the Large Message over DC */
        st = wms_msg_cdma_send_sms_OTA( FALSE,   /* not a TL ack */
                                        rec_ptr,
                                        rec_ptr->ota,
                                        &(rec_ptr->address) );

        // Send MO SMS Retry Attempt Event
        wms_msg_evt_mo_sms_retry_attempt( (uint32)rec_ptr->user_data,
                                          (uint32)rec_ptr,
                                          (uint8)rec_ptr->prev_report.report_status,
                                          (uint8)rec_ptr->prev_report.cause_info.tl_status,
                                          (uint32)rec_ptr->retry_time_left );
        if( WMS_OK_S == st )
        {
          rec_ptr->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
        }
        else if( (WMS_NETWORK_NOT_READY_S == st) || (WMS_PHONE_NOT_READY_S == st) )
        {
          /* retry again in 1 second */
          rec_ptr->retry_wait_time = (sint31)1;
        }
        else
        {
          if( rec_ptr->send_status_to_clients == TRUE )
          {
            msg_event_info.submit_report_info = rec_ptr->prev_report;
            msg_event_info.submit_report_info.report_status = wms_msg_cdma_map_report_status( st );
            msg_event_info.submit_report_info.delivery_failure_permanent = TRUE;
           (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT,&msg_event_info);
          }

          /* Clear the MO record */
          rec_ptr = wms_msg_cdma_get_next_mo_rec_before_delete( rec_ptr );
          continue;
        }
      } /* if first_tch_sms_found */
      else
      {
        /* Now since DC is connected,  allowed resent for all large messages in the queue. */
        rec_ptr->retry_wait_time = (sint31)1;
      }
    }   /* if found large msg */
    else if ( WMS_CLIENT_CH_PREF_ACH_ONLY == rec_ptr->ch_pref )
    {
      /* Wait untill DC connected before trying ACH messages. */
      rec_ptr->retry_wait_time = (sint31)0;
    }
    else if ( WMS_CLIENT_CH_PREF_ACH_OR_TCH == rec_ptr->ch_pref )
    {
      /* nothing to do here */
    }

    rec_ptr = rec_ptr->next;
  } /* while loop */

  if ( wms_msg_suspend_ps_call_is_supported() )
  {
    wms_cm_resume_1x_pscall(wms_get_appinfo_1x_asid());
  }

  if ( first_tch_sms_found ) 
  {
    wms_set_retry_timer();
  }

  return;
} /* wms_msg_auto_dc_send() */


/*=========================================================================
FUNCTION
  wms_msg_stop_dc_resend

DESCRIPTION
  Reschedule resending TCH_ONLY message over DC when it cannot be established.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_stop_dc_resend
(
  void
)
{
  wms_msg_cdma_message_record_type *rec_ptr;
  boolean enable_retry_timer = FALSE;

  MSG_HIGH_1("In wms_msg_stop_dc_resend(), retry period = %d", msg_s_ptr->mo_retry_period);

  rec_ptr = msg_s_ptr->cdma_mo_rec.head_ptr;
  while ( NULL != rec_ptr )
  {
    if ( (WMS_MSG_STATE_QUEUED == rec_ptr->state) && 
         (WMS_CLIENT_CH_PREF_TCH_ONLY == rec_ptr->ch_pref) ) 
    {
      /* Allow the record to be retried if retry logic is enabled. */
      if ( msg_s_ptr->mo_retry_period > 0 )
      {
        rec_ptr->retry_wait_time = msg_s_ptr->mo_retry_interval;
      }
      /* Otherwise defalt to 1 second per message queued */
      else
      {
        rec_ptr->retry_wait_time = (sint31)msg_s_ptr->cdma_mo_rec.count;
      }

      enable_retry_timer = TRUE;
    } /* if found large msg */

    rec_ptr = rec_ptr->next;
  }

  if (enable_retry_timer)
  {
    wms_set_retry_timer();
  }

  return;
} /* wms_msg_stop_dc_resend() */

/*=========================================================================
FUNCTION
  wms_msg_auto_send_ach

DESCRIPTION
  Automatically send the first ACH messages in the list when DC disconnected.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_auto_send_ach
(
  void
)
{
  wms_status_e_type     st;
  boolean first_ach_sms_found = FALSE;
  wms_msg_cdma_message_record_type *rec_ptr = msg_s_ptr->cdma_mo_rec.head_ptr;

  while( NULL != rec_ptr )
  {
    if ( (WMS_MSG_STATE_QUEUED == rec_ptr->state) &&
         (WMS_CLIENT_CH_PREF_TCH_ONLY != rec_ptr->ch_pref) )
    {
      if ( !first_ach_sms_found )
      {
        MSG_HIGH_2( "wms_msg_auto_send_ach(): Sending msg rec 0x%x with seg_num %d",
                    rec_ptr,
                    rec_ptr->seq_num );

        first_ach_sms_found = TRUE;

        /* Send ACH message*/
        st = wms_msg_cdma_send_sms_OTA( FALSE,   /* not a TL ack */
                                        rec_ptr,
                                        rec_ptr->ota,
                                        &(rec_ptr->address) );

        // Send MO SMS Retry Attempt Event
        wms_msg_evt_mo_sms_retry_attempt( (uint32)rec_ptr->user_data,
                                          (uint32)rec_ptr,
                                          (uint8)rec_ptr->prev_report.report_status,
                                          (uint8)rec_ptr->prev_report.cause_info.tl_status,
                                          (uint32)rec_ptr->retry_time_left );

        if( st == WMS_OK_S )
        {
          rec_ptr->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
        }
        else if( (WMS_NETWORK_NOT_READY_S == st) || (WMS_PHONE_NOT_READY_S == st) )
        {
          rec_ptr->retry_wait_time = (sint31)1;
        }
        else
        {
          if( rec_ptr->send_status_to_clients == TRUE )
          {
            msg_event_info.submit_report_info = rec_ptr->prev_report;
            msg_event_info.submit_report_info.report_status = wms_msg_cdma_map_report_status( st );
            msg_event_info.submit_report_info.delivery_failure_permanent = FALSE;
            (void)wms_msg_event_notify( WMS_MSG_EVENT_SUBMIT_REPORT, &msg_event_info );
          }

          /* Clear the MO record. */
          rec_ptr = wms_msg_cdma_get_next_mo_rec_before_delete( rec_ptr );
          continue;
        }
      }
      else
      {
        /* Now since DC is disconnected,  allowed resent for all none large messages. */
        rec_ptr->retry_wait_time = (sint31)1;
      }
    } /* if found ACH SMS */

    rec_ptr = rec_ptr->next;
  } /* while loop */

  if ( wms_msg_suspend_ps_call_is_supported() )
  {
    wms_cm_resume_1x_pscall(wms_get_appinfo_1x_asid());
  }

  if ( first_ach_sms_found )
  {
    wms_set_retry_timer();
  }

  return;
} /* wms_msg_auto_send_ach() */


/*=========================================================================
FUNCTION
  wms_msg_mc_mo_status_proc

DESCRIPTION
  This function processed the MO SMS Status WMS Command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_mc_mo_status_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_msg_cdma_message_record_type  *rec_ptr = NULL;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_0("WMS_CMD_MSG_MC_MO_STATUS_E");
  rec_ptr = msg_s_ptr->mc_msg.rec_ptr;

  if(NULL == rec_ptr)
  {
     MSG_HIGH_0("rec_ptr is NULL");
     return;
  }

  if( WMS_SIP_PERM_ERROR_S == cmd_ptr->cmd.mc_mo_status.status )
  {
    /* Make sure we won't send SMS over SIP again due to a permanent error
    */
    wms_client_s_type *client_ptr = wms_client_ptr_fetch((wms_client_id_type)rec_ptr->client_id);
    if (NULL != client_ptr) 
    {
       wms_bearer_e_type bearer = cmd_ptr->cmd.mc_mo_status.bearer;
       MSG_HIGH_1("Deactivate bearer %d", bearer);
       if ( (WMS_BEARER_CDMA_EVDO == bearer) || 
            (WMS_BEARER_CDMA_WLAN == bearer) ||
            (WMS_BEARER_LTE       == bearer) )
       {
          client_ptr->active_bearers[bearer] = FALSE;
       } 
       else 
       {
          client_ptr->active_bearers[WMS_BEARER_CDMA_EVDO] = FALSE;
          client_ptr->active_bearers[WMS_BEARER_CDMA_WLAN] = FALSE;
          client_ptr->active_bearers[WMS_BEARER_LTE]       = FALSE;
       } 
    }
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }


  if( msg_s_ptr->mc_msg.is_valid == FALSE )
  {
    /* there is no msg being sent; ignore this event
    */
    MSG_HIGH_0("no msg was being sent");
  }
  else if( msg_s_ptr->mc_msg.is_ack == TRUE )
  {

/* --------------------------------------------------------- */
/* -------------------- got ack msg status ----------------- */
/* --------------------------------------------------------- */

    MSG_HIGH_2("got ack st=0x%lx,seq=%d", 
                cmd_ptr->cmd.mc_mo_status.status, 
                rec_ptr->seq_num);

    /* an ack msg was sent; result is available */
    if( rec_ptr->state == WMS_MSG_STATE_LAYER2_IN_PROGRESS )
    {
      /* MT timer not expired */
      if( cmd_ptr->cmd.mc_mo_status.status == WMS_OK_S )
      {
        /* ack was successfully sent; clear the MT record */
        if(rec_ptr->client_id != WMS_CLIENT_TYPE_INTERNAL)
        {
          msg_event_info.ack_report_info.as_id  = cmd_ptr->hdr.as_id;
          msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
          msg_event_info.ack_report_info.client_id    = rec_ptr->client_id;
          msg_event_info.ack_report_info.user_data = rec_ptr->mt_rec_user_data;
          msg_event_info.ack_report_info.transaction_id =
            (wms_transaction_id_type)rec_ptr->user_data;
          msg_event_info.ack_report_info.is_success = TRUE;
          msg_event_info.ack_report_info.error_status =
             wms_msg_cdma_map_report_status(cmd_ptr->cmd.mc_mo_status.status);
          (void) wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
        }
        wms_msg_cdma_free_mt_msg_rec( rec_ptr );
      }
      else
      {
        rec_ptr->retry_wait_time = 1; /* Retry after 1 second */
        rec_ptr->state           = WMS_MSG_STATE_QUEUED;

        rec_ptr->error_class  = msg_s_ptr->mc_msg.error_class;
        rec_ptr->tl_status    = msg_s_ptr->mc_msg.tl_status;
        rec_ptr->retry_count++;
      }
    }
    else if( rec_ptr->state == WMS_MSG_STATE_IDLE )
    {
      /* the timer expired; ignore this event */
      MSG_HIGH_1("MT msg(%d) rec is idle", rec_ptr->seq_num);
    }
    else
    {
      MSG_ERROR_1("Unexpected msg state %d", rec_ptr->state);
      if(rec_ptr->client_id != WMS_CLIENT_TYPE_INTERNAL)
      {
        msg_event_info.ack_report_info.as_id  = cmd_ptr->hdr.as_id;
        msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
        msg_event_info.ack_report_info.client_id = rec_ptr->client_id;
        msg_event_info.ack_report_info.user_data = rec_ptr->mt_rec_user_data;
        msg_event_info.ack_report_info.transaction_id =
          (wms_transaction_id_type)rec_ptr->user_data;
        msg_event_info.ack_report_info.is_success = FALSE;
        msg_event_info.ack_report_info.error_status =
           wms_msg_cdma_map_report_status(cmd_ptr->cmd.mc_mo_status.status);
        (void) wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
      }
      wms_msg_cdma_free_mt_msg_rec( rec_ptr );
    }
  }
  else  /* a teleservice M.O. msg was sent */
  {

/* -------------------------------------------------------- */
/* -------------------- got MO msg status ----------------- */
/* -------------------------------------------------------- */

    MSG_HIGH_3("got MO status = %d ,seq=%d ,tid=0x%lx",
                cmd_ptr->cmd.mc_mo_status.status, 
                rec_ptr->seq_num,
                rec_ptr->user_data );

    if( WMS_MSG_STATE_LAYER2_IN_PROGRESS == rec_ptr->state )
    {
      if( cmd_ptr->cmd.mc_mo_status.status == WMS_OK_S &&
          !(rec_ptr->seq_num & WMS_DUMMY_SEQ_NUM) &&
            ((WMS_MSG_TRANSPORT_TYPE_CS == rec_ptr->transport_type) ||
             (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102 == rec_ptr->transport_type)) )
      {
        /* Msg requesting TL ack was sent successfully */
        /* Set up MO ack timer and wait for TL ack from base station */
        MSG_HIGH_0("MO msg State; Wait for Ack");
        rec_ptr->state = WMS_MSG_STATE_WAIT_FOR_ACK;

        /* Increase Retry Period by WMS_TIMER_MO_ACK */
        rec_ptr->retry_time_left = WMS_TIMER_MO_ACK/1000;
        /* Set the Global Retry Timer */
        wms_set_retry_timer();
      }
      else
      {
        if( cmd_ptr->cmd.mc_mo_status.status == WMS_ACCESS_TOO_LARGE_S )
        {
          MSG_HIGH_3( "Got WMS_ACCESS_TOO_LARGE MO status. rec_ptr = 0x%08x, seq_num, ch_pref = %d",
                      rec_ptr,
                      rec_ptr->seq_num,
                      rec_ptr->ch_pref );

          if ( WMS_CLIENT_CH_PREF_ACH_ONLY == rec_ptr->ch_pref )
          {
            /* Phone is not cofigured to send of Traffic channel
               Send Error code
            */
            cmd_ptr->cmd.mc_mo_status.status = WMS_UNSUPPORTED_S;

            /* No retry for this msg any more */
            rec_ptr->retry_time_left = -1;

            MSG_HIGH_0("Client channel pref is not configured for DC");
          }
          else
          {
            MSG_HIGH_0("Queuing for retry over DC");
            rec_ptr->state           = WMS_MSG_STATE_QUEUED;
            rec_ptr->ch_pref         = WMS_CLIENT_CH_PREF_TCH_ONLY;
            rec_ptr->retry_wait_time = (sint31)1;
            wms_set_retry_timer();
          }
        }

        /* Prepare the event info in case we need to notify clients
        */
        msg_event_info.submit_report_info.as_id  = cmd_ptr->hdr.as_id;
        msg_event_info.submit_report_info.report_status =
          wms_msg_cdma_map_report_status( cmd_ptr->cmd.mc_mo_status.status );
        msg_event_info.submit_report_info.user_data   = rec_ptr->user_data;
        msg_event_info.submit_report_info.client_id    = rec_ptr->client_id;
        msg_event_info.submit_report_info.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
        msg_event_info.submit_report_info.alpha_id.len = 0;
        msg_event_info.submit_report_info.alpha_id.data = NULL;
        msg_event_info.submit_report_info.message_mode= WMS_MESSAGE_MODE_CDMA;

        /* Setting the message_delivery_failure_type, when IMS reports SIP error.
           AP never retries over IMS. Permanent error from IMS means no more retry over IMS is expected
           but AP being master may retry the SMS over other RAT(1X). So, map permanent error from IMS to
           temporary error so that AP may trigger their retry over 1X. 
        */
        msg_event_info.submit_report_info.delivery_failure_permanent = FALSE;


        /* Handle other error cases
        */
        if( (cmd_ptr->cmd.mc_mo_status.status != WMS_OK_S ) &&
            (cmd_ptr->cmd.mc_mo_status.status != WMS_ACCESS_TOO_LARGE_S ) )
        {
          MSG_HIGH_1("MO SMS L2 error: %d", cmd_ptr->cmd.mc_mo_status.status);

          if((msg_s_ptr->mo_retry_period > 0)                                                        &&
             (wms_msg_is_mo_retry_allowed(rec_ptr->retry_count, &msg_event_info.submit_report_info)) &&
             (rec_ptr->retry_time_left > rec_ptr->retry_wait_time))
          {
            if ( WMS_MO_PREF_CHN_NOT_AVAIL_S == cmd_ptr->cmd.mc_mo_status.status )
            {
              /* Wait to hear from DC connected/disconnect event before retry again. */
              rec_ptr->retry_wait_time = (sint31)1;
            }
            else
            {
              rec_ptr->retry_wait_time = (sint31)msg_s_ptr->mo_retry_interval;
              rec_ptr->retry_count++;
            }

            /* queue the message */

            /* Fall back to 1X for retry when AP is not the master. */
            if (WMS_SIP_PERM_ERROR_S == cmd_ptr->cmd.mc_mo_status.status) 
            {
              rec_ptr->transport_type = WMS_MSG_TRANSPORT_TYPE_CS;
            }

            rec_ptr->prev_report = msg_event_info.submit_report_info;
            rec_ptr->state       = WMS_MSG_STATE_QUEUED;
            wms_set_retry_timer();
          }
          else if( rec_ptr->send_status_to_clients == TRUE )
          {
            /* Send permanent error always when the MO SMS requested over IMS fails in hvolte co.
               In HVOLTE, IMS is registered for both VoLTE and SMS. So, 1X bearer is not
               available. App client shouldnt retry on 1X. */
            if ( (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == rec_ptr->transport_type) &&
                 (wms_cfg_check_hvolte_oper_mode() || !cfg_s_ptr->active_bearers[WMS_BEARER_CDMA_1X])
               )
            {
              MSG_HIGH_0("Reporting permanent failure");
              msg_event_info.submit_report_info.delivery_failure_permanent = TRUE;
            }

            (void)wms_msg_event_notify( WMS_MSG_EVENT_SUBMIT_REPORT,
                                        & msg_event_info );

			if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == rec_ptr->transport_type ||
                WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102 == rec_ptr->transport_type)
            {
              wms_prov_app_info_s_type *appInfo_ptr = NULL;

              appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

              if (NULL != appInfo_ptr)
              {
                appInfo_ptr->mo_sms_count--;

                if (0 == appInfo_ptr->mo_sms_count)
                {
                  wms_mm_sms_call_info_T  mm_status;

                  memset(&mm_status, 0x00, sizeof(wms_mm_sms_call_info_T));

                  mm_status.as_id = msg_event_info.submit_report_info.as_id;

                  (void) wms_cm_gw_send_sms_status_to_mm(mm_status);
                }
              }
            }

            /* Send event to diag interface
            */
            wms_msg_evt_mo_sms_status( & msg_event_info, & rec_ptr->msg_hdr );

            /* Clear the MO msg record
            */
            (void)wms_msg_cdma_get_next_mo_rec_before_delete( rec_ptr );

            if ( wms_msg_suspend_ps_call_is_supported() )
            {
              wms_cm_resume_1x_pscall(wms_get_appinfo_1x_asid());
            }

            return;
          }
          else
          {
            /* Send event to diag interface
            */
            wms_msg_evt_mo_sms_status( & msg_event_info, & rec_ptr->msg_hdr );

            /* Clear the MO msg record
            */
            (void)wms_msg_cdma_get_next_mo_rec_before_delete( rec_ptr );

            if ( wms_msg_suspend_ps_call_is_supported() )
            {
              wms_cm_resume_1x_pscall(wms_get_appinfo_1x_asid());
            }

            return;
          }
        }
        else
        {
          /* MO msg without TL ack requested was successful.
             Send OK status to client now.
          */
          MSG_HIGH_0( "MO Msg (No TL Ack) OK.");

          /* Update the tag from NOT_SENT to SENT
          */
          if( rec_ptr->send_mode == WMS_SEND_MODE_MEMORY_STORE )
          {
            (void) wms_msg_do_modify_tag( cmd_ptr->hdr.appInfo_ptr,
                                          rec_ptr->msg_hdr.mem_store,
                                          rec_ptr->msg_hdr.index,
                                          WMS_TAG_MO_SENT,
                                          NULL );
          }
        }

        if( rec_ptr->send_status_to_clients  == TRUE
            && cmd_ptr->cmd.mc_mo_status.status == WMS_OK_S )
        {
          /* Send MO status event to clients
          */
          msg_event_info.submit_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;

          (void)wms_msg_event_notify( WMS_MSG_EVENT_SUBMIT_REPORT,
                                      & msg_event_info );

          if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == rec_ptr->transport_type ||
              WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102 == rec_ptr->transport_type)
          {
            wms_prov_app_info_s_type *appInfo_ptr = NULL;

            appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

            if (NULL != appInfo_ptr)
            {
              appInfo_ptr->mo_sms_count--;
              if (0 == appInfo_ptr->mo_sms_count)
              {
                wms_mm_sms_call_info_T  mm_status;

                memset(&mm_status, 0x00, sizeof(wms_mm_sms_call_info_T));

                mm_status.as_id = msg_event_info.submit_report_info.as_id;

                (void) wms_cm_gw_send_sms_status_to_mm(mm_status);
              }
            }
          }

          /* Send event to diag interface
          */
          wms_msg_evt_mo_sms_status( & msg_event_info, & rec_ptr->msg_hdr );

          /* Clear the MO msg record
          */
          (void)wms_msg_cdma_get_next_mo_rec_before_delete( rec_ptr );

          if ( wms_msg_suspend_ps_call_is_supported() )
          {
            wms_cm_resume_1x_pscall(wms_get_appinfo_1x_asid());
          }

        }
      }
    }
    else if( rec_ptr->state == WMS_MSG_STATE_IDLE )
    {
      /*
      ** record was invalidated because timer has expired or
      ** TL Ack arrived before layer2 Ack.
      */
      MSG_HIGH_1("MO msg rec %d not valid", rec_ptr->seq_num);
    }
    else
    {
      MSG_ERROR_1("Unexpected msg state %d", rec_ptr->state);
      (void)wms_msg_cdma_get_next_mo_rec_before_delete( rec_ptr );

      if ( wms_msg_suspend_ps_call_is_supported() )
      {
        wms_cm_resume_1x_pscall(wms_get_appinfo_1x_asid());
      }
    }

  } /* if is_ack==TRUE - else */

  /* layer2 processing done */
  msg_s_ptr->mc_msg.is_valid = FALSE;
  msg_s_ptr->mc_msg.rec_ptr = NULL;

  /* done */
  return;

} /* wms_msg_mc_mo_status_proc() */


/*
*/
void wms_msg_cdma_mt_mwi_proc(
  wms_cmd_type  *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err      = WMS_CMD_ERR_NONE;
  wms_prov_app_info_s_type   *appInfo_ptr;
  wms_client_message_s_type  *msg_ptr;

  MSG_HIGH_0("WMS_CMD_MSG_MC_MWI_E");

  appInfo_ptr = wms_get_appinfo_1x();

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return;
  }

  /* reset the idle timer if auto disconnect is enabled
  */
  if (dc_s_ptr->auto_disc_active == TRUE)
  {
    (void) rex_set_timer( &dc_s_ptr->idle_timer, dc_s_ptr->idle_timeout );
  }

  /* Store message and notify clients
  ** NOTE: MWI is always stored in NV.
  */
  msg_event_info.mt_message_info.as_id          = cmd_ptr->hdr.as_id;
  msg_event_info.mt_message_info.client_id      = WMS_CLIENT_TYPE_MAX;
  msg_event_info.mt_message_info.transaction_id = WMS_DUMMY_TRANSACTION_ID;
  msg_event_info.mt_message_info.route          = WMS_ROUTE_STORE_AND_NOTIFY;
  msg_event_info.mt_message_info.transport_type = WMS_MSG_TRANSPORT_TYPE_CS;

  msg_ptr = &msg_event_info.mt_message_info.message;

  msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
  msg_ptr->msg_hdr.mem_store    = WMS_MEMORY_STORE_NV_CDMA;
  msg_ptr->msg_hdr.tag          = WMS_TAG_MT_NOT_READ;

  msg_ptr->u.cdma_message.address.number_of_digits    = 0;
  msg_ptr->u.cdma_message.subaddress.number_of_digits = 0;
  msg_ptr->u.cdma_message.is_mo                       = FALSE;
  msg_ptr->u.cdma_message.is_service_present          = FALSE;
  msg_ptr->u.cdma_message.is_tl_ack_requested         = FALSE;
  msg_ptr->u.cdma_message.teleservice                 = WMS_TELESERVICE_MWI;

  /* only one byte in raw bd is valid which carries the msg count */
  msg_ptr->u.cdma_message.raw_ts.format               = WMS_FORMAT_MWI;
  msg_ptr->u.cdma_message.raw_ts.len                  = 1;
  msg_ptr->u.cdma_message.raw_ts.data[0]              = cmd_ptr->cmd.mwi.msg_count;

  MSG_HIGH_1("Message count = %d", cmd_ptr->cmd.mwi.msg_count);

  if (cfg_s_ptr->cdma_voice_mail_index != (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX)
  {
    /* replace a previous voice mail notification in NV
    */
    msg_ptr->msg_hdr.index = cfg_s_ptr->cdma_voice_mail_index;
    cmd_err = wms_msg_do_write( cmd_ptr->hdr.appInfo_ptr,
                                WMS_WRITE_MODE_REPLACE,
                                msg_ptr,
                                NULL );
    if (cmd_err != WMS_CMD_ERR_NONE)
    {
      /* msg cannot be stored
      */
      msg_event_info.mt_message_info.route = WMS_ROUTE_TRANSFER_AND_ACK;
      msg_ptr->msg_hdr.mem_store           = WMS_MEMORY_STORE_NONE;
      msg_ptr->msg_hdr.index               = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
    }
  }
  else
  {
    /* There was voice mail stored before. Insert this one.
    */
    cmd_err = wms_msg_do_write( cmd_ptr->hdr.appInfo_ptr,
                                WMS_WRITE_MODE_INSERT,
                                msg_ptr,
                                NULL );
    if (cmd_err != WMS_CMD_ERR_NONE)
    {
      /* msg cannot be stored
      */
      msg_event_info.mt_message_info.route = WMS_ROUTE_TRANSFER_AND_ACK;
      msg_ptr->msg_hdr.mem_store           = WMS_MEMORY_STORE_NONE;
      msg_ptr->msg_hdr.index               = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
    }
  }

  /* Send msg to clients
  */
  (void)wms_msg_event_notify(WMS_MSG_EVENT_RECEIVED_MESSAGE, &msg_event_info);

  return;
} /* wms_msg_cdma_mt_mwi_proc() */

/*===========================================================================
FUNCTION   wms_msg_cdma_has_wap_udh

DESCRIPTION
  Check if a given Message has a WAP Push Header

DEPENDENCIES
  None

RETURNS
  TRUE:   WAP UDH Present
  FALSE:  WAP UDH Absent

SIDE EFFECTS
  None

===========================================================================*/
boolean wms_msg_cdma_has_wap_udh
(
  wms_client_bd_s_type  * cl_ptr
)
{
  uint32  i;

  if( cl_ptr->message_id.udh_present == TRUE &&
      cl_ptr->user_data.num_headers != 0 )
  {
    for( i=0; i < cl_ptr->user_data.num_headers; i++ )
    {
      if( cl_ptr->user_data.headers[i].header_id == WMS_UDH_PORT_8 ||
          cl_ptr->user_data.headers[i].header_id == WMS_UDH_PORT_16 )
      {
        /* found a WAP header */
        return TRUE;
      }
    }
  }

  /* No WAP header found */
  return FALSE;

} /* wms_msg_cdma_has_wap_udh() */

#ifdef FEATURE_CDSMS_BROADCAST
static wms_status_e_type wms_msg_process_mt_SCPT
(
 wms_tl_message_type         * tl_ptr,
 wms_msg_transport_type_e_type transport_type
)
{
  wms_status_e_type           st = WMS_OK_S;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ! (tl_ptr->mask & WMS_MASK_TL_BEARER_DATA) ||
      ! (tl_ptr->cl_bd.mask & WMS_MASK_BD_SCPT_DATA) ||
      tl_ptr->cl_bd.scpt_data_ptr->num_entries == 0 )
  {
    /* SCPT data parameter not present */
    st = WMS_MISSING_PARM_S;
  }
  else
  {
    /* Send a new command to WMS to process SCPT data */
    st = wms_bc_scpt_data( tl_ptr, transport_type );
  }

  return st;

} /* wms_msg_process_mt_SCPT() */
#endif /* FEATURE_CDSMS_BROADCAST */

#if (defined(FEATURE_UIM_TOOLKIT_UTK) || defined(FEATURE_CCAT)) || defined(FEATURE_GSTK)
wms_status_e_type wms_msg_utk_pp_download_proc(
  wms_cdma_message_s_type *cdma_msg,
  wms_tl_message_type     *tl_msg
)
{
  wms_OTA_message_type       cdma_OTA;
  gstk_address_type          gstk_address;
  gstk_sms_tpdu_type         gstk_sms_tpdu;
  wms_status_e_type          st = WMS_OK_S;
  gstk_status_enum_type      gstk_status;
  wms_prov_app_info_s_type  *appInfo_ptr;
  wms_gstk_user_data_s_type *user_data_ptr;

  (void)tl_msg; // parameter not used

  appInfo_ptr = wms_get_appinfo_1x();

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return WMS_NULL_PTR_S;
  }

  st = wms_ts_encode_CDMA_tl( &cdma_tl,
                              &cdma_msg->raw_ts,
                              &cdma_OTA );

  /* GSTK does not need to look at the address, so 0 the length */
  gstk_address.length = 0;

  /* Copy the ota data into the tpdu of the envelope. */
  gstk_sms_tpdu.length = (uint8)MIN(cdma_OTA.data_len, sizeof(cdma_OTA.data));
  gstk_sms_tpdu.tpdu   = wms_mem_malloc(gstk_sms_tpdu.length);
  if (NULL == gstk_sms_tpdu.tpdu)
  {
    /* early return */
    return WMS_GENERAL_ERROR_S;
  }

  (void)memscpy( gstk_sms_tpdu.tpdu,
                 gstk_sms_tpdu.length,
                 (uint8 *)cdma_OTA.data,
                 gstk_sms_tpdu.length);

  /* Send ENVELOPE command to the card
  */
  user_data_ptr = wms_mem_malloc(sizeof(wms_gstk_user_data_s_type));
  if (NULL == user_data_ptr)
  {
    /* early return */
    return WMS_GENERAL_ERROR_S;
  }
  else
  {
    /* Cache some specific data for use when acking the status of the pp download (user ack). */
    user_data_ptr->as_id                     = appInfo_ptr->asid;
    user_data_ptr->msg_mode                  = WMS_MESSAGE_MODE_CDMA;
    user_data_ptr->pp_download_address       = cdma_msg->address;
    user_data_ptr->pp_download_subaddress    = cdma_msg->subaddress;
    user_data_ptr->pp_download_teleservice   = cdma_msg->teleservice;
    user_data_ptr->pp_download_msg_id        = cdma_tl.cl_bd.message_id.id_number;

    MSG_HIGH_1("Send GSTK slot ID %d ", appInfo_ptr->mmgsdi_appInfo.slot_id);

    gstk_status = gstk_slot_send_envelope_sms_pp_dl_command( (gstk_slot_id_enum_type)appInfo_ptr->mmgsdi_appInfo.slot_id,
                                                             (uint32)user_data_ptr,
                                                             TRUE,
                                                             &gstk_address,
                                                             &gstk_sms_tpdu,
                                                             wms_msg_gstk_evt_cb );
  }

  wms_mem_free(gstk_sms_tpdu.tpdu);

  if (gstk_status != GSTK_SUCCESS)
  {
    /* Sending down the PP download was unsuccessful */
    st = WMS_GENERAL_ERROR_S;
  }

  return st;
} /* wms_msg_utk_pp_download_proc() */
#endif /* (FEATURE_UIM_TOOLKIT_UTK||FEATURE_CCAT) && FEATURE_GSTK */


/*===========================================================================
FUNCTION   wms_msg_cdma_check_esn

DESCRIPTION
  Check whether the received ESN parameter matches the ESN of the phone.

DEPENDENCIES
  None

RETURNS
  WMS_OK_S: Matches (including case of ESN not being present in message)
  WMS_ESN_MISMATCH_S:  Does not match
  Other errors (e.g. no memory or null pointer)

SIDE EFFECTS
  None

===========================================================================*/
wms_status_e_type wms_msg_cdma_check_esn
(
  const wms_raw_ts_data_s_type      * raw_bd_ptr,
  wms_client_bd_s_type              * cl_bd_ptr,
  uint32                            * received_esn
)
{
  wms_status_e_type  st = WMS_OK_S;
  uint8  parm_id, parm_len;
  uint8  pos = 0;

  if( NULL == raw_bd_ptr || NULL == cl_bd_ptr || NULL == received_esn )
  {
    wms_check_for_null_ptr();
    st = WMS_NULL_PTR_S;
  }
  else if( cl_bd_ptr->mask & WMS_MASK_BD_OTHER &&
           cl_bd_ptr->other.desired_other_len > 0 )
  {
    *received_esn = 0;
    cl_bd_ptr->other.other_data = NULL;
    cl_bd_ptr->other.input_other_len =
            MIN( cl_bd_ptr->other.desired_other_len, WMS_MAX_LEN );
    cl_bd_ptr->other.other_data =
            wms_mem_malloc(cl_bd_ptr->other.input_other_len );

    if( NULL == cl_bd_ptr->other.other_data )
    {
      MSG_ERROR_0("No memory available");
      st = WMS_OUT_OF_RESOURCES_S;
    }
    else if( (st=wms_ts_decode_CDMA_bd( raw_bd_ptr,
                                        FALSE, /* is_unpacked?  */
                                        TRUE,  /* decode_other? */
                                        cl_bd_ptr)) != WMS_OK_S )
    {
      MSG_ERROR_0("Decoding other parameters failed");
      st = WMS_INVALID_FORMAT_S;
    }
    else if( ( st = wms_ts_validate_bd_subparms( & cdma_tl.cl_bd, cdma_tl.teleservice ) )
             != WMS_OK_S )
    {
      MSG_ERROR_0("BD sub parameter validation failed");
    }
    else if( cl_bd_ptr->mask & WMS_MASK_BD_OTHER &&
             cl_bd_ptr->other.desired_other_len > 0 )
    {
      while( pos < cl_bd_ptr->other.input_other_len )
      {
        parm_id  = cl_bd_ptr->other.other_data[pos++];
        parm_len = cl_bd_ptr->other.other_data[pos++];

        if( (uint8)WMS_BD_ESN == parm_id && parm_len >= 4 )
        {
          /* Found the ESN parameter and do the comparison */
          *received_esn = ((uint32)cl_bd_ptr->other.other_data[pos] << 24) +
                                (cl_bd_ptr->other.other_data[pos+1] << 16) +
                                (cl_bd_ptr->other.other_data[pos+2] << 8) +
                                (cl_bd_ptr->other.other_data[pos+3]);
          MSG_HIGH_2( "Received ESN=0x%lx; phone ESN=0x%lx",
                      *received_esn,
                      cfg_s_ptr->phone_esn );
          if( *received_esn != cfg_s_ptr->phone_esn )
          {
            MSG_ERROR_0("ESN mismatch! Msg ignored.");
            st = WMS_ESN_MISMATCH_S;
            break;
          }
        }
        pos += parm_len;
      } /* while */
    } /* if */
  } /* if */
  if( NULL != cl_bd_ptr &&
      NULL != cl_bd_ptr->other.other_data )
  {
    wms_mem_free(cl_bd_ptr->other.other_data);
    cl_bd_ptr->other.input_other_len = 0;
    cl_bd_ptr->other.other_data = NULL;
  }
  return st;

} /* wms_msg_cdma_check_esn() */

/*=========================================================================
FUNCTION
wms_msg_check_flash_sms_message

DESCRIPTION
  Check if a CDMA message is a Flash SMS Message.
  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Is Flash SMS Message
  FALSE: Is Flash SMS Message

SIDE EFFECTS
  None

=========================================================================*/
static boolean wms_msg_check_flash_sms_message
(
  const wms_tl_message_type *cdma_tl_ptr
)
{
  boolean ret_value = FALSE;

  MSG_HIGH_0("Function wms_msg_check_flash_sms_message entered");

  if (NULL == cdma_tl_ptr)
  {
    MSG_ERROR_0("Null Parameter Passed in wms_msg_check_flash_sms_message");
    return ret_value;
  }

  if (cdma_tl_ptr->mask & WMS_MASK_TL_BEARER_DATA)
  {
    if((cdma_tl_ptr->cl_bd.mask & WMS_MASK_BD_VALID_REL)
       && (WMS_RELATIVE_VALIDITY_IMMEDIATE ==
           wms_ts_encode_relative_time(&cdma_tl_ptr->cl_bd.validity_relative)))
    {
      ret_value = TRUE;
    }
    else if ( (cdma_tl_ptr->cl_bd.mask & WMS_MASK_BD_DISPLAY_MODE)
              && (WMS_DISPLAY_MODE_IMMEDIATE == cdma_tl_ptr->cl_bd.display_mode) )
    {
      ret_value = TRUE;
    }
  }

  MSG_HIGH_1("Function wms_msg_check_flash_sms_message result = %d", ret_value);
  return ret_value;

}
#ifdef FEATURE_GOBI_VZW
boolean wms_msg_is_vzw_supported(wms_teleservice_e_type ts)
{
  /* VZW requires special handling of teleservices that are not supported.
     According to Product Test (Balaji V.), the only ID's that are supported
     are 4097, 4098, and 4099 */
  switch (ts)
  {
    case WMS_TELESERVICE_CPT_95:
    case WMS_TELESERVICE_CMT_95:
    case WMS_TELESERVICE_VMN_95:
      return TRUE;

    default:
      return FALSE;
  }
}
#endif /* FEATURE_GOBI_VZW */
/*=========================================================================
FUNCTION
  wms_msg_is_mt_msg_type

DESCRIPTION
  This function checks whether the msg type is applicable to MT msg

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_is_mt_msg_type
(
  wms_bd_message_type_e_type type
)
{
  return ( WMS_BD_TYPE_DELIVER      == type ||
           WMS_BD_TYPE_DELIVERY_ACK == type ||
           WMS_BD_TYPE_USER_ACK     == type ||
           WMS_BD_TYPE_READ_ACK     == type );
}

/* Deliver or store CDMA or Analog messages.
** It does not handle the TL ack of the MT msg
*/
wms_status_e_type wms_msg_cdma_deliver(
  wms_prov_app_info_s_type           *appInfo_ptr,
  wms_transaction_id_type             tid,
  wms_cdma_message_s_type            *cl_ptr,
  wms_routing_s_type                 *routing_ptr,
  wms_msg_cdma_message_record_type   *rec_ptr,
  wms_msg_transport_type_e_type       transport_type
)
{
  wms_status_e_type           st                     = WMS_OK_S;
  wms_cmd_err_e_type          cmd_err                = WMS_CMD_ERR_NONE;
  wms_write_mode_e_type       write_mode             = WMS_WRITE_MODE_INSERT;
  wms_client_message_s_type  *msg_ptr                = &msg_event_info.mt_message_info.message;
  boolean                     deliver_to_all_clients = TRUE;
  wms_client_type_e_type      client                 = WMS_CLIENT_TYPE_MAX;
  wms_client_type_e_type      parsed_client          = WMS_CLIENT_TYPE_MAX;
  boolean                     shared                 = TRUE;
  wms_msg_transport_s_type   *transport_ptr          = wms_msg_choose_registered_transport( wms_get_appinfo_1x_asid() );
  uint32                      received_esn           = 0;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return WMS_NULL_PTR_S;
  }

  // For testing purpose, return WMS_TERMINAL_BLOCKED_S here.

  MSG_HIGH_0("Delivering CDMA/AMPS messages");

  /* Unpack all fields, including BD subparameters
  */
  wms_ts_convert_cl2tl( cl_ptr, &cdma_tl );

  if (rec_ptr != NULL)
  {
     cdma_tl.bearer_reply_seq_num = rec_ptr->seq_num;
  }

  st = wms_ts_decode_bearer_data( &cl_ptr->raw_ts, &cdma_tl.cl_bd );

  if (st != WMS_OK_S)
  {
    MSG_ERROR_1("wms_ts_decode_bearer_data() returns  %d", st);
    return st;
  }

  if (!wms_msg_is_mt_msg_type( cdma_tl.cl_bd.message_id.type ))
  {
     MSG_ERROR_1("Invalid message type %d in MT SMS", cdma_tl.cl_bd.message_id.type);
     st = WMS_INVALID_PARM_VALUE_S;
  }
  else
  {
     st = wms_ts_validate_bd_subparms( &cdma_tl.cl_bd, cdma_tl.teleservice );
  }


  if (st != WMS_OK_S)
  {
    MSG_ERROR_1("wms_ts_validate_bd_subparms returns  %d", st);
    return st;
  }
  // Drop non-admin MT SMS when IMS is in limited service
  if ( (transport_ptr != NULL) &&
       (WMS_MSG_TRANSPORT_NW_REG_STATUS_LIMITED_SRV == transport_ptr->nw_reg_status) &&
       (cl_ptr->teleservice != WMS_TELESERVICE_WAP) &&
       (cl_ptr->teleservice != WMS_TELESERVICE_CATPT) &&
       !(wms_cm_CT_features_supported(wms_get_appinfo_1x_asid()) &&
        cl_ptr->teleservice == WMS_TELESERVICE_CT_CDMA_WAP))
  {
    MSG_HIGH_0("Drop non-admin MT SMS");

    routing_ptr->route     = WMS_ROUTE_DISCARD;
    routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;

    if (rec_ptr != NULL)
    {
      rec_ptr->client_id = WMS_CLIENT_TYPE_MAX;
    }

    return WMS_OK_S;
  }

  /* ------------------------------------------------------------------------
  ** ------------------------------------------------------------------------
  ** Before processing according to the routing, change the routing for special
  ** cases first.
  ** ------------------------------------------------------------------------
  ** ------------------------------------------------------------------------
  */

#ifdef FEATURE_CDSMS_BROADCAST
  if (WMS_TELESERVICE_BROADCAST == cdma_tl.teleservice)
  {

    /* Check if the Message is CMAS and if the mandatory parameter
     ** "type_0" is present
     */
    if ( WMS_SRV_CMAS_PRESIDENTIAL_ALERT <= cl_ptr->service && cl_ptr->service <= WMS_SRV_CMAS_TEST_MSG )
    {
      wms_client_ts_data_s_type *ts_data = wms_mem_malloc(sizeof(wms_client_ts_data_s_type));
      if (NULL == ts_data)
      {
        /* early return */
        return WMS_GENERAL_ERROR_S;
      }

      st = wms_ts_decode( &cl_ptr->raw_ts, ts_data );

      if ( WMS_OK_S == st )
      {
        wms_cmas_msg_s_type  *cmas_msg = wms_mem_malloc(sizeof(wms_cmas_msg_s_type));

        if (NULL != cmas_msg)
        {
          st = wms_ts_decode_cmas_msg( ts_data->u.cdma.user_data.data,
                                       ts_data->u.cdma.user_data.data_len,
                                       cmas_msg);

          if ( WMS_INVALID_FORMAT_S == st )
          {
            MSG_HIGH_0("Invaild CMAS msg with missing required type_0");
          }
        }
        else
        {
          st = WMS_GENERAL_ERROR_S;
        }
        WMS_MEM_FREEIF(cmas_msg);
      }
      else
      {
        MSG_HIGH_0("Decoding failed for CDMA BC SMS");
      }
      WMS_MEM_FREEIF(ts_data);

      if (WMS_OK_S != st)
      {
        return st;
      }
    }

    /* All broadcast messages go to the clients directly;
    ** enhancements later will save the messages in the broadcast SMS memories.
    */
    routing_ptr->route     = WMS_ROUTE_TRANSFER_AND_ACK;
    routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;
  }
#endif /* FEATURE_CDSMS_BROADCAST */


  if (WMS_TELESERVICE_SCPT == cdma_tl.teleservice)
#ifdef FEATURE_CDSMS_BROADCAST
  {
    routing_ptr->route = WMS_ROUTE_INTERNAL_PROC;
    st = wms_msg_process_mt_SCPT( & cdma_tl, transport_type );
    return st;
  }
#else /* FEATURE_CDSMS_BROADCAST */
  {
    routing_ptr->route     = WMS_ROUTE_DISCARD;
    routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;
  }
#endif /* FEATURE_CDSMS_BROADCAST */

  st = wms_msg_cdma_check_esn( & cl_ptr->raw_ts, & cdma_tl.cl_bd, & received_esn );
  /* Check ESN and discard the message if ESN does not match */
  if (st != WMS_OK_S)
  {
      /* If error is WMS_ESN_MISMATCH_S, message is not delivered and no ack
      ** will be sent; in case of any other errors, an ack with error code
      ** will be sent to the network by the calling function.
      */
      return st;
  }
  else if (rec_ptr != NULL )
  {
    rec_ptr->esn_present = (received_esn > 0) ? TRUE : FALSE;
  }

#ifdef FEATURE_CCAT
  if (WMS_TELESERVICE_CATPT == cdma_tl.teleservice)
  {
#ifdef FEATURE_GSTK

    // Reject the PP Download SMS if SIM refresh is in progress or init not completed
    // If TL ACK is requested, send some TEMP error
    if (appInfo_ptr->refresh_in_progress || !appInfo_ptr->sim_init_complete)
    {
      return WMS_GENERAL_ERROR_S;
    }

    /* Check if service is allocated and activated */
    if ( appInfo_ptr->pp_data_download_enabled &&
         (nv_rtre_control() == NV_RTRE_CONTROL_USE_RUIM) )
    {
      st = wms_msg_utk_pp_download_proc(cl_ptr, &cdma_tl);

      if (WMS_OK_S == st)
      {
        /* Set the route, such that this message is acked properly */
        routing_ptr->route = WMS_ROUTE_INTERNAL_PROC;
      }

      /* return early no more processing by cdma_deliver() is needed */
      return st;
    }
    else
    {
      /* Either svc is not activated or allocated.
      ** Treat this as a normal message.
      */
      routing_ptr->route       = WMS_ROUTE_STORE_AND_NOTIFY;
      routing_ptr->mem_store   = WMS_MEMORY_STORE_RUIM;
    }
#endif /* FEATURE_GSTK */
  }
#endif /* FEATURE_CCAT */

#if defined(FEATURE_UIM_TOOLKIT_UTK )|| defined(FEATURE_GSTK)
  /* The routing for UTK messages should be changed to TRANSFER_ONLY,
  ** so that the application needs to ack it later.
  */
  if ( (cdma_tl.cl_bd.mask & WMS_MASK_BD_DISPLAY_MODE) &&
       (cdma_tl.cl_bd.display_mode == WMS_DISPLAY_MODE_RESERVED) )
  {
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
    /* Make sure that the current rtre control is not NV.
    */
    if ( ( (WMS_DOWNLOAD_MODE_PP_VAS     == cdma_tl.cl_bd.download_mode) ||
           (WMS_DOWNLOAD_MODE_PP_PRL     == cdma_tl.cl_bd.download_mode) ||
           (WMS_DOWNLOAD_MODE_UPDATE_PRL == cdma_tl.cl_bd.download_mode) ) &&
         (nv_rtre_control() != NV_RTRE_CONTROL_USE_RUIM) )
    {
      /* This PRL update can not be sent down to the card, return bad status
      ** and the returning function will ack the message appropriately.
      */
      return WMS_GENERAL_ERROR_S;
    }
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    if ( (WMS_DOWNLOAD_MODE_PP_VAS == cdma_tl.cl_bd.download_mode) ||
         (WMS_DOWNLOAD_MODE_PP_PRL == cdma_tl.cl_bd.download_mode) )
    {
#ifdef FEATURE_GSTK

      // Reject the PP Download SMS if SIM refresh is in progress or init not completed
      // If TL ACK is requested, send some TEMP error
      if (appInfo_ptr->refresh_in_progress || !appInfo_ptr->sim_init_complete)
      {
        return WMS_GENERAL_ERROR_S;
      }

      /* Check if service is allocated and activated */
      if (appInfo_ptr->pp_data_download_enabled)
      {
        st = wms_msg_utk_pp_download_proc(cl_ptr, &cdma_tl);

        if (WMS_OK_S == st)
        {
          /* Set the route, such that this message is acked properly */
          routing_ptr->route = WMS_ROUTE_INTERNAL_PROC;
        }

        /* return early no more processing by cdma_deliver() is needed */
        return st;
      }
      else
      {
        /* Either svc is not activated or allocated.
        ** Treat this as a normal message.
        */
        routing_ptr->route       = WMS_ROUTE_STORE_AND_NOTIFY;
        routing_ptr->mem_store   = WMS_MEMORY_STORE_RUIM;
      }
#else /* FEATURE_GSTK */
      /* The UI client will send down the envelope command to the card */
      routing_ptr->route     = WMS_ROUTE_TRANSFER_ONLY;
      routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;

      deliver_to_all_clients = FALSE;
      client                 = WMS_CLIENT_TYPE_UI;
#endif /* FEATURE_GSTK */
    }
    /* Treat UPDATE RECORDS as regular messages, but explicitly set the
    ** route and mem_store.
    */
    else if (WMS_DOWNLOAD_MODE_UPDATE_PRL == cdma_tl.cl_bd.download_mode)
    {
      routing_ptr->route         = WMS_ROUTE_STORE_AND_NOTIFY;
      routing_ptr->mem_store     = WMS_MEMORY_STORE_RUIM;
    }
  }
  else
#endif /* FEATURE_UIM_TOOLKIT_UTK || FEATURE_GSTK */

  /* Always store voice mails to NV
  */
  if (cdma_tl.cl_bd.mask & WMS_MASK_BD_NUM_OF_MSGS)
  {
    tid                    = (uint32)WMS_DUMMY_TRANSACTION_ID;
    routing_ptr->route     = WMS_ROUTE_STORE_AND_NOTIFY;
    routing_ptr->mem_store = WMS_MEMORY_STORE_NV_CDMA;
  }

  /* Fill in msg header
  */
  msg_event_info.mt_message_info.as_id          = appInfo_ptr->asid;
  msg_event_info.mt_message_info.route          = routing_ptr->route;
  msg_event_info.mt_message_info.transaction_id = tid;
  msg_event_info.mt_message_info.transport_type = transport_type;

  msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
  msg_ptr->msg_hdr.mem_store    = routing_ptr->mem_store;
  msg_ptr->msg_hdr.tag          = WMS_TAG_MT_NOT_READ;
  msg_ptr->u.cdma_message       = *cl_ptr;

  /* voice mail handling
  */
  if (cdma_tl.cl_bd.mask & WMS_MASK_BD_NUM_OF_MSGS)
  {
    MSG_HIGH_0("Got voice mail");

#ifdef FEATURE_GOBI_CHINATEL
    routing_ptr->route     = WMS_ROUTE_DISCARD;
    routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;
#else /* FEATURE_GOBI_CHINATEL */
    if (cfg_s_ptr->cdma_voice_mail_index != (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX)
    {
      /* replace a previous voice mail notification in NV
      */
      msg_ptr->msg_hdr.index      = cfg_s_ptr->cdma_voice_mail_index;
      write_mode                  = WMS_WRITE_MODE_REPLACE;
      MSG_HIGH_0("replacing old voice mail");
    }
#endif /* FEATURE_GOBI_CHINATEL */
  }

#ifdef FEATURE_GOBI_VZW
  if (FALSE == wms_msg_is_vzw_supported(cdma_tl.teleservice))
  {
      MSG_HIGH_1("Invalid teleservice ID recieved (%d), discarding", cdma_tl.teleservice);
      return WMS_UNSUPPORTED_S;
  }
#endif /* FEATURE_GOBI_VZW */

  /* ------------------------------------------------------------------------
  ** ------------------------------------------------------------------------
  ** Processing message according to the final routing value.
  ** ------------------------------------------------------------------------
  ** ------------------------------------------------------------------------
  */

  /* Check if this message is to be delivered to a specific client.
  */
  parsed_client = wms_msg_parse_msg(msg_ptr);

  if (parsed_client != WMS_CLIENT_TYPE_MAX)
  {
    /* This message is not to be shared */
    deliver_to_all_clients                                   = FALSE;
    msg_event_info.mt_message_info.route                     = WMS_ROUTE_TRANSFER_ONLY;
    msg_event_info.mt_message_info.message.msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
    routing_ptr->route                                       = WMS_ROUTE_TRANSFER_ONLY;
    routing_ptr->mem_store                                   = WMS_MEMORY_STORE_NONE;
    client                                                   = parsed_client;
  }

  /* Handling for WAP Push Messages */
  if (wms_cfg_check_wap_push_message(msg_ptr))
  {
    MSG_HIGH_0("WAP Push Message Detected! Routes Changed");
    msg_event_info.mt_message_info.route                     = WMS_ROUTE_TRANSFER_AND_ACK;
    msg_event_info.mt_message_info.message.msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
    routing_ptr->route                                       = WMS_ROUTE_TRANSFER_AND_ACK;
    routing_ptr->mem_store                                   = WMS_MEMORY_STORE_NONE;
  }

  if (wms_msg_check_flash_sms_message(&cdma_tl))
  {
    MSG_HIGH_0("Flash SMS Message Detected! Routes Changed to No Store");
    msg_event_info.mt_message_info.route                     = WMS_ROUTE_TRANSFER_AND_ACK;
    msg_event_info.mt_message_info.message.msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
    routing_ptr->route                                       = WMS_ROUTE_TRANSFER_AND_ACK;
    routing_ptr->mem_store                                   = WMS_MEMORY_STORE_NONE;
  }

  if ((appInfo_ptr->refresh_in_progress || !appInfo_ptr->sim_init_complete) &&
      (WMS_ROUTE_STORE_AND_NOTIFY == routing_ptr->route))
  {
    MSG_HIGH_2( "refresh_in_progress: %d or cdma_init_complete is FALSE: %d, Discarding SMS",
                appInfo_ptr->refresh_in_progress,
                appInfo_ptr->sim_init_complete );

    return WMS_GENERAL_ERROR_S;
  }

  /* set the client id to the mt_message_info */
  msg_event_info.mt_message_info.client_id = client;

  MSG_HIGH_2( "Routing=%d, Mem=%d",
              routing_ptr->route,
              routing_ptr->mem_store );

  /* Change transaction id to dummy if route is changed to either
     STORE_AND_NOTIFY and TRANSFER_AND_ACK
  */
  if ( (WMS_ROUTE_TRANSFER_AND_ACK == routing_ptr->route) ||
       (WMS_ROUTE_STORE_AND_NOTIFY == routing_ptr->route) )
  {
     msg_event_info.mt_message_info.transaction_id = (uint32)WMS_DUMMY_TRANSACTION_ID;
  }

  switch (routing_ptr->route)
  {
    case WMS_ROUTE_DISCARD:
      /* Do NOT transfer the message to the clients.
      ** The Ack will be sent after this function is called.
      */
      MSG_HIGH_0("Routing is DISCARD");
      cmd_err = WMS_CMD_ERR_NONE;
      break;

    case WMS_ROUTE_TRANSFER_ONLY:
      /* Transfer the message to clients.
      ** The Ack will NOT be sent after this function is called.
      */
      cmd_err = WMS_CMD_ERR_NONE;
      break;

    case WMS_ROUTE_TRANSFER_AND_ACK:
      /* Transfer the message to clients.
      ** The Ack will be sent after this function is called.
      */
      cmd_err = WMS_CMD_ERR_NONE;
      break;

    case WMS_ROUTE_STORE_AND_NOTIFY:
      /* Check for duplicate messages before any more processing is done to this
      ** message.  Do not notify clients if this is a duplicate, simply return
      ** WMS_OK_S and change the routing to DISCARD so the msg will be acked.
      */
      if( wms_msg_cdma_check_dups( & cdma_tl ) == TRUE )
      {
        MSG_HIGH_0("Dup msg not stored");
        routing_ptr->route = WMS_ROUTE_DISCARD;
        return WMS_OK_S;
      }

      /* Store the message and notify the clients.
      ** The Ack will be sent after this function is called.
      */
      cmd_err = wms_msg_do_write( appInfo_ptr, write_mode, msg_ptr, NULL );
      MSG_HIGH_1("Writing msg result: %d", cmd_err);
      break;

    default:
      MSG_ERROR_1("Bad route: %d", routing_ptr->route);
      cmd_err = WMS_CMD_ERR_CFG_ROUTE;
      break;
  }

  /* Report events to Diag
  */
  if (WMS_CMD_ERR_NONE == cmd_err)
  {
    wms_evt_mt_sms_notify_s_type   evt;

    evt.mem_type      = (uint8)msg_ptr->msg_hdr.mem_store;
    evt.msg_index     = msg_ptr->msg_hdr.index;

    event_report_payload( EVENT_MT_SMS_NOTIFY, sizeof(evt), (void*) &evt );
  }

  /* Send msg event to clients
  */
  if ( (cmd_err == WMS_CMD_ERR_NONE) &&
       (routing_ptr->route != WMS_ROUTE_DISCARD) )
  {
    /* Send msg to clients.
    */
    if (deliver_to_all_clients == TRUE)
    {
      shared = wms_msg_event_notify( WMS_MSG_EVENT_RECEIVED_MESSAGE,
                                     &msg_event_info );

      /* Check if WMS is required to ack this message. */
      if ( (FALSE == shared) &&
           (WMS_DUMMY_TRANSACTION_ID != msg_event_info.mt_message_info.transaction_id) )
      {
        /* The client will ack this message, change the route accordingly */
        routing_ptr->route = WMS_ROUTE_TRANSFER_ONLY;
      }
    }
    else
    {
      wms_msg_event_notify_single_client( client,
                                          WMS_MSG_EVENT_RECEIVED_MESSAGE,
                                          &msg_event_info );
    }
    MSG_HIGH_0("Msg delivered to clients");

    if (appInfo_ptr->ecbm_mode_is_enabled)
    {
      appInfo_ptr->mt_msg_rec_count_in_ecbm++;
      MSG_HIGH_1( "MT messages count in ECBM mode: %d", appInfo_ptr->mt_msg_rec_count_in_ecbm );
    }
  }


  if (rec_ptr != NULL)
  {
    rec_ptr->client_id = client;
  }

  st = wms_msg_map_cmd_err_to_status(cmd_err);

  return st;
} /* wms_msg_cdma_deliver() */

/*===========================================================================
FUNCTION wms_msg_mt_CDMA_tl_ack_proc

DESCRIPTION
  Handle the TL ack from network.
  rec_ptr : is the message recored for which the TL Ack arrived.
  cause_code_ptr : is the cause code response from the network.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_msg_mt_CDMA_tl_ack_proc(
  wms_prov_app_info_s_type           *appInfo_ptr,
  wms_msg_cdma_message_record_type   *rec_ptr,
  wms_cause_code_type                *cause_code_ptr
)
{
   if ( (WMS_MSG_STATE_WAIT_FOR_ACK == rec_ptr->state) ||
        (WMS_MSG_STATE_LAYER2_IN_PROGRESS == rec_ptr->state) )
   { /*
       ** If TL Ack arrives before layer2 ack, assume layer2 status is OK,
       ** and it will be ignored when it comes later.
        */
      MSG_HIGH_3( "Got TL Ack (tid 0x%lx), error_class %d, reply_seq_num %d ",
                  rec_ptr->user_data,
                  cause_code_ptr->error_class,
                  cause_code_ptr->reply_seq_num );

      if (rec_ptr->send_status_to_clients == TRUE)
      {
         // deliver status to client
         msg_event_info.submit_report_info.as_id                      = appInfo_ptr->asid;
         msg_event_info.submit_report_info.client_id                  = rec_ptr->client_id;
         msg_event_info.submit_report_info.user_data                  = rec_ptr->user_data;
         msg_event_info.submit_report_info.report_status              = (cause_code_ptr->error_class == WMS_ERROR_NONE) ?
                                                                        WMS_RPT_OK : WMS_RPT_CDMA_TL_ERROR;
         msg_event_info.submit_report_info.cause_info.error_class     = cause_code_ptr->error_class;
         msg_event_info.submit_report_info.cause_info.tl_status       = cause_code_ptr->tl_status;
         msg_event_info.submit_report_info.alpha_id.len               = 0;
         msg_event_info.submit_report_info.alpha_id.data              = NULL;
         msg_event_info.submit_report_info.mo_ctrl_status             = WMS_MO_CTRL_STATUS_NO_MOD;
         msg_event_info.submit_report_info.message_mode               = WMS_MESSAGE_MODE_CDMA;
         msg_event_info.submit_report_info.delivery_failure_permanent = FALSE;

        (void)wms_msg_event_notify( WMS_MSG_EVENT_SUBMIT_REPORT, &msg_event_info );
     
        if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == rec_ptr->transport_type ||
            WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102 == rec_ptr->transport_type)
        {
          wms_prov_app_info_s_type *appInfo_ptr = NULL;

          appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

          if (NULL != appInfo_ptr)
          {
            appInfo_ptr->mo_sms_count--;
            if (0 == appInfo_ptr->mo_sms_count)
            {
              wms_mm_sms_call_info_T  mm_status;
  
              memset(&mm_status, 0x00, sizeof(wms_mm_sms_call_info_T));
  
              mm_status.as_id = msg_event_info.submit_report_info.as_id;
  
              (void) wms_cm_gw_send_sms_status_to_mm(mm_status);
            }
          }
        }

      }

      wms_msg_evt_mo_sms_status( &msg_event_info, &rec_ptr->msg_hdr );

      /* Update message status if it is in a memory store and got ack:
      */
      if ( (WMS_SEND_MODE_MEMORY_STORE  == rec_ptr->send_mode) &&
           (WMS_RPT_OK == msg_event_info.submit_report_info.report_status) )
      {
         (void) wms_msg_do_modify_tag( appInfo_ptr,
                                       rec_ptr->msg_hdr.mem_store,
                                       rec_ptr->msg_hdr.index,
                                       WMS_TAG_MO_SENT,
                                       NULL );
      }

      /* Free the mo_record:
      */
      (void)wms_msg_cdma_get_next_mo_rec_before_delete( rec_ptr );

      if ( wms_msg_suspend_ps_call_is_supported() )
      {
        wms_cm_resume_1x_pscall(wms_get_appinfo_1x_asid());
      }
   }
   else
   {
      MSG_HIGH_0("Ignore unexpected TL Ack");
   }

   return;
} /* wms_msg_mt_CDMA_tl_ack_proc() */

/*
*/
void wms_msg_mt_CDMA_proc(
  wms_prov_app_info_s_type  *appInfo_ptr,
  wms_cmd_mc_mt_msg_type    *mt_msg_cmd_ptr,
  wms_routing_s_type        *routing_ptr
)
{
  wms_OTA_message_type             *OTA_ptr;
  wms_transaction_id_type           tid;
  wms_status_e_type                 st;
  byte                              seq_num     = 0;
  wms_msg_cdma_message_record_type *rec_ptr     = NULL;
  boolean                           no_delivery = FALSE;

  OTA_ptr = &mt_msg_cmd_ptr->ota;

  /* get next transaction id
  */
#ifdef FEATURE_FUZZ_TEST
  if (WMS_BEARER_MAX == mt_msg_cmd_ptr->bearer) {
     tid = WMS_DBG_TRANSACTION_ID;
  }
  else
#endif /* FEATURE_FUZZ_TEST */
  {
     tid = wms_msg_get_next_tid();
  }

  /* extract transport layer fields
  */
  st = wms_ts_decode_CDMA_tl( OTA_ptr,        /* IN  */
                              &cdma_tl,       /* OUT */
                              &cdma_cl.raw_ts /* OUT */ );

  MSG_LOW_1("OTA len=%d", OTA_ptr->data_len);
  MSG_LOW_1("bd_len=%d", cdma_cl.raw_ts.len);

  // if UASMS_MISSING_MANDATORY_PARM_S, decoding was successful
  // for each parameter, but address was missing in received msg.
  if ( (st == WMS_MISSING_PARM_S) &&
       (cdma_tl.tl_message_type == WMS_TL_TYPE_POINT_TO_POINT) &&
       (cdma_tl.mask & WMS_MASK_TL_BEARER_REPLY_OPTION) )
  {
    st = WMS_OK_S;        // as if received a good message
    no_delivery = TRUE;   // will send back ack immediately without
                          // devliering msg to client
  }

  if (st != WMS_OK_S)
  {
    /* error msg already reported in decoding functions */
  }
  else if (WMS_TL_TYPE_ACK == cdma_tl.tl_message_type)
  {
    if (TRUE == appInfo_ptr->mmgsdi_is_busy)
    {
      MSG_HIGH_0("TL ack not processed, SIM is busy");
      return;
    }

   /* ----------------------------------------------------- */
   /* -------------- Process Ack to MO msg ---------------- */
   /* ----------------------------------------------------- */

       seq_num = cdma_tl.cause_code.reply_seq_num;

       /* Only cause codes field should be present in an ack
       */
       if (cdma_tl.mask != WMS_MASK_TL_CAUSE_CODES)
       {
         MSG_HIGH_2( "TL Ack's mask %d not expected %d",
                     cdma_tl.mask,
                     WMS_MASK_TL_CAUSE_CODES );

         st = WMS_MISSING_PARM_S;
       }

       rec_ptr = wms_msg_cdma_map_mo_rec(seq_num);

       if (NULL == rec_ptr )
       {
         /* no matching msg pending */
         MSG_HIGH_1("No matching mo rec %d", seq_num);
         st = WMS_INVALID_PARM_VALUE_S;
       }
       else
       {
          wms_msg_mt_CDMA_tl_ack_proc(appInfo_ptr, rec_ptr, &cdma_tl.cause_code);
       }

  } /* -------- End of processing received SMS TL Ack. -------- */

  else
  {
    wms_msg_transport_type_e_type transport_type =
       (WMS_BEARER_IMS_3GPP2 == mt_msg_cmd_ptr->bearer)?
       WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS: WMS_MSG_TRANSPORT_TYPE_CS;

    if (TRUE == appInfo_ptr->mmgsdi_is_busy)
    {
      MSG_ERROR_0("CDMA MT failed, SIM is busy");

      if ( (cdma_tl.mask & WMS_MASK_TL_BEARER_REPLY_OPTION) &&
           (TRUE == mt_msg_cmd_ptr->ack_pending) )
      {
        wms_msg_Gack_info.message_mode       = WMS_MESSAGE_MODE_CDMA;
        wms_msg_Gack_info.transaction_id     = tid;
        wms_msg_Gack_info.u.cdma.error_class = WMS_ERROR_TEMP;
        wms_msg_Gack_info.u.cdma.tl_status   = WMS_TL_DESTINATION_RESOURCE_SHORTAGE_S;

        wms_msg_cdma_map_status( st,
                                 &wms_msg_Gack_info.u.cdma.error_class,
                                 &wms_msg_Gack_info.u.cdma.tl_status );

        (void) wms_msg_ms_ack_ext( (uint8) WMS_CLIENT_TYPE_INTERNAL,
                                   appInfo_ptr->asid,
                                   NULL, /* cmd_cb */
                                   NULL, /* user_data */
                                   &wms_msg_Gack_info,
                                   transport_type );
      }
      return;
    }

/* -------------------------------------------------------- */
/* ------ process received MT PtoP or Broadcast msg: ------ */
/* -------------------------------------------------------- */

    if (FALSE == wms_msg_convert_address_to_plus(&cdma_tl.address))
    {
        MSG_ERROR_0("wms_msg_convert_address_to_plus Failed!");
    }

    wms_ts_convert_tl2cl( & cdma_tl, & cdma_cl );

    if (WMS_TL_TYPE_BROADCAST == cdma_tl.tl_message_type)
    {
      /* remove the TL Ack mask if any
      */
      cdma_tl.mask &=  ~ WMS_MASK_TL_BEARER_REPLY_OPTION;

      /* Map the teleservice to the API's unique broadcast teleservice
      */
      cdma_tl.teleservice = WMS_TELESERVICE_BROADCAST;

#ifdef FEATURE_CDSMS_BROADCAST
      if (FALSE == wms_bc_enable_check(appInfo_ptr) )
      {
        /* drop the message since BC is disabled
        */
        no_delivery = TRUE;
        MSG_HIGH_0("Dropping BC SMS msg.");
      }
#else /* FEATURE_CDSMS_BROADCAST */
      no_delivery = TRUE;
      MSG_HIGH_0("Dropping BC SMS msg.");
#endif /* FEATURE_CDSMS_BROADCAST */
    }

    if ( (cdma_tl.mask & WMS_MASK_TL_BEARER_REPLY_OPTION) &&
         (TRUE == mt_msg_cmd_ptr->ack_pending) )
    {
      /* ---------------------------------------------- */
      /* ----------- process reply option ------------- */
      /* ---------------------------------------------- */
      seq_num = cdma_tl.bearer_reply_seq_num;

      MSG_HIGH_1("MT msg: TL ack (%d) required!", seq_num);

      rec_ptr = wms_msg_cdma_map_mt_rec( seq_num );

      if (NULL != rec_ptr)
      {
        /* the same seq num is in use. ignore this msg
        */
        MSG_HIGH_1( "MT msg: seq %d in use!", seq_num);
        st = WMS_INVALID_PARM_VALUE_S;
        return;
      }

      rec_ptr = wms_msg_cdma_get_free_mt_rec();

      if (NULL == rec_ptr)
      {
        MSG_HIGH_0("No more MT rec available!");
        st = WMS_OUT_OF_RESOURCES_S;
      }
      else  /* found a free record */
      {
        /* deliver msg to client depending on the routing.
        */
        rec_ptr->seq_num = seq_num;
        rec_ptr->esn_present = FALSE;
        rec_ptr->transport_type = transport_type;
        if( no_delivery == FALSE )
        {
          st = wms_msg_cdma_deliver(appInfo_ptr, tid, &cdma_cl, routing_ptr, rec_ptr, transport_type);
        }

        if (WMS_ESN_MISMATCH_S == st)
        {
          /* Msg is ignored; no ack will be sent */
          MSG_ERROR_0("No ack will be sent due to ESN mismatch!");
          return;
        }

        /* fill in mt record
        */
        rec_ptr->state     = WMS_MSG_STATE_WAIT_FOR_ACK;
        rec_ptr->user_data = (void*) tid;

        if (cdma_tl.mask & WMS_MASK_TL_ADDRESS)
        {
          rec_ptr->address    = cdma_tl.address;
        }
        else
        {
          rec_ptr->address.number_of_digits = 0;
        }

        if (cdma_tl.mask & WMS_MASK_TL_SUBADDRESS)
        {
          rec_ptr->subaddress = cdma_tl.subaddress;
        }
        else
        {
          rec_ptr->subaddress.number_of_digits = 0;
        }

        /* init the corresponding timer in the record queue
        */
        (void) rex_set_timer(&rec_ptr->timer, WMS_TIMER_MT_ACK);

        MSG_HIGH_0( "MT: ack timer set.");
        MSG_HIGH_2( "MT: tid=%d, seq=%d",
                    tid,
                    seq_num );

        /* If msg was good, and routing is Transfer Only,
        ** set up timer, but don't send ack yet.
        ** The client is supposed to ack the msg.
        */
        if ( (FALSE == no_delivery) &&
             (WMS_OK_S == st) &&
             (WMS_ROUTE_TRANSFER_ONLY == routing_ptr->route) )
        {
          /* No ack is sent immediately */
        }
        else
        {
          // send TL ack for good or bad MT msg immediately when needed.
          wms_msg_Gack_info.message_mode       = WMS_MESSAGE_MODE_CDMA;
          wms_msg_Gack_info.transaction_id     = tid;
#ifdef FEATURE_GOBI_VZW
          /* st is set by wms_msg_cdma_deliver() and will be WMS_UNSUPPORTED_S
             only if the VZW check for teleservice ID failed.  If this is the
             case then VZW's requirements say to return an error in the ACK
             (class=TEMP, tl_status=INVALID_TS_ID) */
          if ( (WMS_UNSUPPORTED_S == st) &&
               (FALSE == wms_msg_is_vzw_supported(cdma_tl.teleservice)) )
          {
            wms_msg_Gack_info.u.cdma.error_class = WMS_ERROR_TEMP;
            wms_msg_Gack_info.u.cdma.tl_status   = WMS_TL_INVALID_TELESERVICE_ID_S;
          } else
#endif /* FEATURE_GOBI_VZW */
          wms_msg_cdma_map_status( st,
                                   & wms_msg_Gack_info.u.cdma.error_class,
                                   & wms_msg_Gack_info.u.cdma.tl_status );
          (void) wms_msg_ms_ack_ext( (uint8) WMS_CLIENT_TYPE_INTERNAL,
                                     appInfo_ptr->asid,
                                     NULL, /* cmd_cb */
                                     NULL, /* user_data */
                                     &wms_msg_Gack_info,
                                     transport_type );
        }
      }
    } /* ----- done with reply option processing ----- */
    else
    {
/* ---------------------------------------------- */
/* ----------- No SMS Ack requested ------------- */
/* ---------------------------------------------- */

      /* deliver msg to client
      */
      if (no_delivery == FALSE)
      {
        MSG_HIGH_0( "MT: No ACK requested");

        (void) wms_msg_cdma_deliver( appInfo_ptr,
                                     (uint32)WMS_DUMMY_TRANSACTION_ID,
                                     &cdma_cl,
                                     routing_ptr,
                                     NULL,
                                     transport_type );
      }
    } /* if */

  } /* ---- done with CDMA PtoP or Broadcast msg processing ---- */

} /* wms_msg_mt_CDMA_proc() */


/*
*/
void wms_msg_mt_IS91EP_proc (
  wms_prov_app_info_s_type *appInfo_ptr,
  wms_OTA_message_type     *OTA_ptr,
  wms_routing_s_type       *routing_ptr
)
{
  byte                          num_digits;  /* total raw bd len */

  word   i, j;    /* Index for looping through char/digits */
  word   msg_pos; /* Bit position in SMS message relative to start of data */

  word   chari_size; /* Bit size of each chari digit/address */

  /* Character/Digit in each Extended Protocol 28-bit word */
  /* (12-bit parity field excluded in each EP word)        */
  byte   chr_per_word;

  byte   num_of_ep_words; /* Num of Extended Protocol 28-bit words */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize current bit position to message length field, */
  /* char/digit size, characters in an EP word and the number */
  /* of EP words in the SMS message.                          */
  msg_pos      = 15;
  chari_size   = 6;
  chr_per_word = 4;


  /* All EP teleservices are mobile-terminated PTP only
  */
  if( OTA_ptr->format == WMS_FORMAT_ANALOG_CLI )
  {
    cdma_cl.teleservice = WMS_TELESERVICE_IS91_PAGE;

    chari_size   = 4;
    chr_per_word = 6;
  }
  else if( OTA_ptr->format == WMS_FORMAT_ANALOG_VOICE_MAIL )
  {
    cdma_cl.teleservice = WMS_TELESERVICE_IS91_VOICE_MAIL;
  }
  else
  {
    cdma_cl.teleservice = WMS_TELESERVICE_IS91_SHORT_MESSAGE;
  }

  cdma_cl.address.number_of_digits    = 0;
  cdma_cl.subaddress.number_of_digits = 0;
  cdma_cl.is_tl_ack_requested         = FALSE;

  cdma_cl.raw_ts.format = OTA_ptr->format;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* EP Word 1 - Extended Protocol Header    */
  /* Unpack number of EP Message words, not  */
  /* including header word and set the       */
  /* number of fields for user data          */
  num_of_ep_words = b_unpackb( (byte*)OTA_ptr->data,
                               msg_pos,
                               WMS_EP_MSG_LEN_BITS );

  num_digits = 0;

  msg_pos += WMS_EP_MSG_LEN_BITS;  /* skip msg len */

  msg_pos += WMS_EP_MST_BITS;      /* skip message type */


  /* Loop - EP Words 2-N and unpack each Message word */
  for( i=0; i < num_of_ep_words; i++ )
  {
    /* Skip over end of EP data word padding (4 bits) and    */
    /* header data (4 bits) at the beginning of EP data word */
    msg_pos += 8;


    /* Loop - unpack each character to user data or callback number
    */
    for( j=0; j < chr_per_word && num_digits < WMS_MAX_LEN; j++ )
    {
      cdma_cl.raw_ts.data[ num_digits ] =
           b_unpackb( (byte*)OTA_ptr->data, msg_pos, chari_size );

      msg_pos += chari_size;
      num_digits ++;

    } /* for each chari */

  } /* for each word */


  /* Set bd len
  */
  cdma_cl.raw_ts.len = num_digits;


  /* deliver msg to client
  */
  (void) wms_msg_cdma_deliver( appInfo_ptr,
                               (uint32)WMS_DUMMY_TRANSACTION_ID,
                               &cdma_cl,
                               routing_ptr,
                               NULL,
                               WMS_MSG_TRANSPORT_TYPE_CS );

  return;
} /* wms_msg_mt_IS91EP_proc() */


/*
*/
void wms_msg_mt_AWISMS_proc(
  wms_prov_app_info_s_type *appInfo_ptr,
  wms_OTA_message_type     *OTA_ptr,
  wms_routing_s_type       *routing_ptr
)
{
#ifdef FEATURE_ACP
  wms_transaction_id_type    tid;
  wms_status_e_type          st;

  /* get next transaction id
  */
  tid = wms_msg_get_next_tid();

  /* decode data
  */
  st = wms_ts_decode_AWISMS_tl( OTA_ptr,        /* IN  */
                                &cdma_tl,       /* OUT */
                                &cdma_cl.raw_ts /* OUT */ );

  MSG_LOW_1("OTA len=%d", OTA_ptr->data_len);
  MSG_LOW_1("bd_len=%d", cdma_cl.raw_ts.len);


  if( st != WMS_OK_S )
  {
    /* error msg already reported in decoding functions */
  }
  else
  {
    wms_ts_convert_tl2cl( &cdma_tl, &cdma_cl );
    cdma_cl.is_tl_ack_requested = TRUE;

    /* ack is always needed for AWISMS */
    {
      msg_s_ptr->awi_info.is_valid  = TRUE;
      msg_s_ptr->awi_info.user_data = (void *)tid;

      /* deliver msg to client
      */
      st = wms_msg_cdma_deliver( appInfo_ptr,
                                 tid,
                                 &cdma_cl,
                                 routing_ptr,
                                 NULL,
                                 WMS_MSG_TRANSPORT_TYPE_CS );

      (void) rex_set_timer(&msg_s_ptr->awi_info.timer, WMS_TIMER_AWISMS_ACK);

      MSG_HIGH_2( "mt awi: timer set. tid=%d, seq=%d",
                  tid,
                  msg_s_ptr->awi_info.seq_no );

    } /* msg delivered */
  } /* ---- done with ASWSMS PtoP msg processing ---- */

#else /* FEATURE_ACP */
  (void)appInfo_ptr; // paramter not used
  (void)OTA_ptr; // paramter not used
  (void)routing_ptr; // paramter not used
#endif /* FEATURE_ACP */

  return;
} /* wms_msg_mt_AWISMS_proc() */

/*
*/
void wms_msg_cdma_mt_msg_proc(
  wms_cmd_type  *cmd_ptr
)
{
  wms_routing_s_type         routing;
  wms_prov_app_info_s_type  *appInfo_ptr;

  appInfo_ptr = cmd_ptr->hdr.appInfo_ptr;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  if (!asid_is_valid(appInfo_ptr->asid))
  {
    MSG_ERROR_0("Invalid as_id in appInfo_ptr");
    return;
  }

  MSG_HIGH_0("WMS_CMD_MSG_MT_MSG_E");

  if (TRUE == appInfo_ptr->mmgsdi_is_busy && WMS_FORMAT_CDMA != cmd_ptr->cmd.mt_msg.ota.format)
  {
    MSG_HIGH_0("Dropping the CDMA ANALOG msgs, SIM is busy");
    return;
  }

  if (appInfo_ptr->ecbm_mode_is_enabled)
  {
    wms_cfg_s_type *lcfg_s_ptr  = wms_cfg_s_ptr();

    MSG_HIGH_2( "ECBM is enabled, mt_msg_limit: %d, mt_msg_rec_count: %d",
                lcfg_s_ptr->mt_msg_limit_in_ecbm_mode,
                appInfo_ptr->mt_msg_rec_count_in_ecbm );

    if (appInfo_ptr->mt_msg_rec_count_in_ecbm >= lcfg_s_ptr->mt_msg_limit_in_ecbm_mode)
    {
      MSG_HIGH_1( "Dropping MT message beyond max limit: %d in ecbm mode", lcfg_s_ptr->mt_msg_limit_in_ecbm_mode );
      return;
    }
  }

  /* reset the idle timer if auto disconnect is enabled
  */
  if (dc_s_ptr->auto_disc_active == TRUE)
  {
    (void) rex_set_timer( &dc_s_ptr->idle_timer, dc_s_ptr->idle_timeout );
  }

  /* Check message routing for CDMA messages.
  ** Note: Analog messages are always stored in NV.
  */
  switch (cmd_ptr->cmd.mt_msg.ota.format)
  {
    case WMS_FORMAT_CDMA:
      routing = cfg_s_ptr->routes[appInfo_ptr->asid].pp_routes[WMS_MESSAGE_CLASS_CDMA];
      wms_msg_mt_CDMA_proc(appInfo_ptr, &cmd_ptr->cmd.mt_msg, &routing);

      break;

    case WMS_FORMAT_ANALOG_CLI:
    case WMS_FORMAT_ANALOG_VOICE_MAIL:
    case WMS_FORMAT_ANALOG_SMS:
      routing.route     = WMS_ROUTE_STORE_AND_NOTIFY;
      routing.mem_store = WMS_MEMORY_STORE_NV_CDMA;
      wms_msg_mt_IS91EP_proc(appInfo_ptr, &cmd_ptr->cmd.mt_msg.ota, &routing);
      break;

    case WMS_FORMAT_ANALOG_AWISMS:
      routing.route     = WMS_ROUTE_STORE_AND_NOTIFY;
      routing.mem_store = WMS_MEMORY_STORE_NV_CDMA;
      wms_msg_mt_AWISMS_proc(appInfo_ptr, &cmd_ptr->cmd.mt_msg.ota, &routing);
      break;

    default:
      MSG_ERROR_1("Invalid format: %d", cmd_ptr->cmd.mt_msg.ota.format);
      break;
  }

  return;
} /* wms_msg_cdma_mt_msg_proc() */

#ifdef FEATURE_CDSMS_BROADCAST
/*
*/
void wms_msg_send_scpt_result
(
  wms_cmd_type          *cmd_ptr,
  wms_tl_message_type   *tl_ptr
)
{
  wms_status_e_type                 st = WMS_OK_S;

  /* use the next message number
  */
  tl_ptr->cl_bd.message_id.id_number = wms_cfg_get_ruim_next_message_number();

  if (FALSE == wms_msg_convert_plus_address(&tl_ptr->address))
  {
    MSG_ERROR_0("wms_msg_convert_plus_address Failed!");
  }

  /* Validate the bearer data before encoding
  */
  if (( st = wms_ts_validate_bd_subparms( &tl_ptr->cl_bd, tl_ptr->teleservice ) ) == WMS_OK_S )
  {
    /* Encode the client bearer data
    */
    st = wms_ts_encode_CDMA_bd( &tl_ptr->cl_bd, &cdma_raw_bd );
  }

  if (WMS_OK_S == st)
  {
    /* ----------------- TL ack is always requested   ------------------- */
    /* Use the same TL fields as in the incoming message, except for
    ** for the bearer reply option.
    */
    tl_ptr->mask |= WMS_MASK_TL_BEARER_REPLY_OPTION;

    /* use the next sequence number
    */
    tl_ptr->bearer_reply_seq_num = (uint8)wms_msg_cdma_next_seq_num();
    st = wms_msg_get_next_unused_seq_num(tl_ptr->bearer_reply_seq_num,
                                         &tl_ptr->bearer_reply_seq_num);

    if (WMS_OK_S == st)
    {
       st = wms_ts_encode_CDMA_tl( tl_ptr,
                                   & cdma_raw_bd,
                                   & ruim_sms.ota );
    }
  }

  if (WMS_OK_S == st)
  {
    wms_cmd_err_e_type cmd_err;

    cmd_ptr->cmd.msg_send.transport_type               = cmd_ptr->cmd.bc_scpt_data.transport_type;
    cmd_ptr->cmd.msg_send.ch_pref                      = cmd_ptr->cmd.bc_scpt_data.ch_pref;
    cmd_ptr->hdr.cmd_id                                = WMS_CMD_MSG_SEND;
    cmd_ptr->hdr.appInfo_ptr                           = cmd_ptr->hdr.appInfo_ptr;
    cmd_ptr->cmd.msg_send.send_mode                    = WMS_SEND_MODE_CLIENT_MESSAGE;
    cmd_ptr->cmd.msg_send.resend_msg                   = FALSE;
    cmd_ptr->cmd.msg_send.client_msg_id                = WMS_DUMMY_MESSAGE_NUMBER;
    cmd_ptr->cmd.msg_send.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
    cmd_ptr->cmd.msg_send.message.msg_hdr.tag          = WMS_TAG_MO_SENT;
    cmd_ptr->cmd.msg_send.message.msg_hdr.mem_store    = WMS_MEMORY_STORE_NONE;
    cmd_ptr->cmd.msg_send.message.msg_hdr.index        = WMS_DUMMY_MESSAGE_INDEX;

    cmd_err = wms_msg_cdma_send_mo_channel( FALSE,
                                            cmd_ptr,
                                            cmd_ptr->cmd.msg_send.send_mode,
                                            &cmd_ptr->cmd.msg_send.message.msg_hdr,
                                            tl_ptr,
                                            &ruim_sms.ota );

    /* if no resource, drop the MO SCPT result message */
    if (WMS_CMD_ERR_NONE != cmd_err)
    {
      MSG_HIGH_1("MO SCPT result failed: cmd_err %d", cmd_err);
    }
  }

  return;
} /* wms_msg_send_scpt_result() */

#endif /* FEATURE_CDSMS_BROADCAST */

#endif /* FEATURE_CDSMS */


/*=========================================================================
FUNCTION
  wms_msg_evt_mo_sms_status

DESCRIPTION
  Event Notification function for MO SMS Status to DIAG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_evt_mo_sms_status
(
  wms_msg_event_info_s_type  * msg_evt_ptr,
  wms_client_msg_hdr_s_type  * msg_hdr_ptr
)
{
  wms_evt_mo_sms_status_s_type  evt;

  evt.transaction_id = (uint32) msg_evt_ptr->submit_report_info.user_data;
  evt.mem_type       = (uint8)msg_hdr_ptr->mem_store;
  evt.msg_index      = msg_hdr_ptr->index;
  evt.report_status  = (uint8)msg_evt_ptr->submit_report_info.report_status;

  if( msg_evt_ptr->submit_report_info.message_mode == WMS_MESSAGE_MODE_CDMA )
  {
    evt.cause_code     = (uint8)msg_evt_ptr->submit_report_info.cause_info.tl_status;
  }
  else
  {
    evt.cause_code     = (uint8) msg_evt_ptr->submit_report_info.cause_info.cause_value;
  }

  event_report_payload( EVENT_MO_SMS_STATUS, sizeof(evt), (void*) & evt );

  return;

} /* wms_msg_evt_mo_sms_status() */

/*=========================================================================
FUNCTION
  wms_msg_evt_mo_sms_retry_attempt

DESCRIPTION
  Event Notification function for MO SMS Retry Attempt to DIAG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_evt_mo_sms_retry_attempt
(
  uint32 transaction_id,
  uint32 record_id,
  uint8  prev_report_status,
  uint8  prev_cause_code,
  uint32 retry_time_left
)
{
  wms_evt_mo_sms_retry_attempt_s_type  evt;

  evt.transaction_id     = transaction_id;
  evt.record_id          = record_id;
  evt.prev_report_status = prev_report_status;
  evt.prev_cause_code    = prev_cause_code;
  evt.retry_time_left    = retry_time_left;

  event_report_payload( EVENT_MO_SMS_RETRY_ATTEMPT, sizeof(evt), (void*) & evt );

  return;

} /* wms_msg_evt_mo_sms_retry_attempt() */

/*===========================================================================
FUNCTION wms_msg_transport_ptr

DESCRIPTION
  Return the pointer to transport data struct.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to transport data.

SIDE EFFECTS
  None.

===========================================================================*/
wms_msg_transport_s_type* wms_msg_transport_ptr
(
  wms_msg_transport_id_type transport_id
)
{
  int index;

  for (index = 0; index < WMS_MSG_TRANSPORT_ARR_MAX; index++)
  {
    if (msg_transports[index].transport_id == transport_id &&
        msg_transports[index].is_registered)
    {
      return &msg_transports[index];
    }
  }

  return NULL;
} /* wms_msg_transport_ptr() */

wms_msg_transport_s_type* wms_msg_transport_get_transport_ptr
(
  wms_msg_transport_type_e_type transport_type,
  sys_modem_as_id_e_type as_id
)
{
  int index;

  for (index = 0; index < WMS_MSG_TRANSPORT_ARR_MAX; index++)
  {
    if (msg_transports[index].transport_type == transport_type &&
        msg_transports[index].as_id          == as_id &&
        msg_transports[index].is_registered)
    {
      return &msg_transports[index];
    }
  }

  return NULL;
  
}


/*=========================================================================
FUNCTION
  wms_msg_transport_init

DESCRIPTION
  Allow SMS transport layer to register with WMS handle MO and MT SMS messages
   with specified capability.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_NULL_PTR_S
  WMS_UNSUPPORTED_S


SIDE EFFECTS

=========================================================================*/
wms_status_e_type  wms_msg_transport_init
(
  wms_msg_transport_type_e_type     transport_type,
  wms_msg_transport_cap_type        transport_cap,
  wms_msg_transport_id_type        *transport_id_ptr
)
{
  wms_msg_transport_id_type l_transport_id;
  // TODO: add checking for transport_cap
  if (transport_type >= WMS_MSG_TRANSPORT_TYPE_MAX || 0 == transport_cap)
  {
    return WMS_UNSUPPORTED_S;
  }

  if (NULL == transport_id_ptr)
  {
    return WMS_NULL_PTR_S;
  }

  MSG_HIGH_2( "wms_msg_transport_init transport_type: %d, transport_cap: %d", 
              transport_type, transport_cap );

  l_transport_id = (wms_msg_transport_id_type)transport_type;


  if (!msg_transports[transport_type].is_registered)
  {
    msg_transports[transport_type].is_registered  = TRUE;
    msg_transports[transport_type].cap            = transport_cap;
    msg_transports[transport_type].mo_sms_cb      = NULL;
    msg_transports[transport_type].transport_id   = l_transport_id;
    msg_transports[transport_type].transport_type = transport_type;
  }

  *transport_id_ptr = l_transport_id;

  return WMS_OK_S;
}

/*=========================================================================
FUNCTION
  wms_msg_transport_release

DESCRIPTION
  Allow SMS transport layer to release itself with WMS..

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_UNSUPPORTED_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type  wms_msg_transport_release
(
  wms_msg_transport_id_type     transport_id
)
{
  wms_status_e_type st = WMS_OK_S;
  wms_msg_transport_s_type  *transport_ptr = wms_msg_transport_ptr(transport_id);

  MSG_HIGH_1("wms_msg_transport_release, transport_id: %d", transport_id);

  if (NULL == transport_ptr || !transport_ptr->is_registered)
  {
    return WMS_UNSUPPORTED_S;
  }

  if (transport_ptr->mo_sms_cb != NULL)
  {
    st = wms_msg_transport_reg_mo_sms_cb(transport_id, NULL);
  }

  if (WMS_OK_S == st)
  {
    transport_ptr->is_registered = FALSE;
  }

  return st;

} /* wms_msg_transport_release() */

/*===========================================================================
FUNCTION wms_msg_choose_registered_transport

DESCRIPTION
  Select a registered trasnport to send mo sms

DEPENDENCIES
  none

RETURN VALUE
  mo sms callback function.

SIDE EFFECTS
  None.

===========================================================================*/
wms_msg_transport_s_type* wms_msg_choose_registered_transport
(
  sys_modem_as_id_e_type as_id
)
{
  uint8 transport_type;
  wms_msg_transport_s_type* transport_ptr = NULL;

  for (transport_type = 0; transport_type < (uint8) WMS_MSG_TRANSPORT_TYPE_MAX; transport_type++)
  {
    transport_ptr = wms_msg_transport_get_transport_ptr(transport_type, as_id);
    if (NULL != transport_ptr && transport_ptr->is_registered && NULL != transport_ptr->mo_sms_cb)
    {
      return transport_ptr;
    }
  }

  return NULL;
}

#ifdef FEATURE_GWSMS

/*=========================================================================
FUNCTION
  wms_msg_trasnsport_map_wms_to_mn_status

DESCRIPTION
  Convert wms status to mn status for SMS over IMS


DEPENDENCIES
  None

RETURN VALUE
  sms_ims_cause_type_T

SIDE EFFECTS


=========================================================================*/
sms_ims_cause_type_T wms_msg_trasnsport_map_wms_to_mn_status(wms_status_e_type status)
{
   switch(status)
   {
      case WMS_OK_S:
         return SMS_IMS_OK;

      case WMS_SIP_PERM_ERROR_S:
         return SMS_IMS_PERM_ERROR;

      case WMS_SIP_TEMP_ERROR_S:
         return SMS_IMS_TEMP_ERROR;

      default:
         return SMS_IMS_PERM_ERROR;
   }
}
/*=========================================================================
FUNCTION
  wms_mn_mo_sms_callback

DESCRIPTION
  Callback function registered with NAS for NAS to send 3GPP MO SMS messages
  over the SMS transport layer.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS


=========================================================================*/
void wms_mn_mo_sms_callback(void *msg_ptr)
{
  SM_RL_RPDU_T             *mn_msg_ptr              = msg_ptr;
  wms_msg_transport_s_type *transport_ptr           = NULL;
  boolean                   send_to_transport_layer = FALSE;
  sys_modem_as_id_e_type    as_id                   = SYS_MODEM_AS_ID_1;

  if (WMS_DEVICE_MODE_SS != wms_cfg_s_ptr()->device_mode)
  {
    as_id = mn_msg_ptr->as_id;
  }

  transport_ptr = wms_msg_transport_get_transport_ptr( WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS, as_id );

  if (transport_ptr == NULL || transport_ptr->mo_sms_cb == NULL || msg_ptr == NULL || 
     (NULL !=mn_msg_ptr && 0 == mn_msg_ptr->rpdu_length))
  {
    return;
  }

  MSG_HIGH_1("wms_mn_mo_sms_callback is called to send RP message with type %d", mn_msg_ptr->rpdu[0] & 0x07);

  switch (mn_msg_ptr->rpdu[0] & 0x07)
  {
  case RP_DATA_MS:
    if (mn_msg_ptr->RP_DA.present && mn_msg_ptr->RP_DA.length > 0)
    {
      send_to_transport_layer = TRUE;
    }
    break;

  case RP_ACK_MS:
  case RP_ERROR_MS:
  case RP_SMMA_MS:
    send_to_transport_layer = TRUE;
    break;

  default:
    break;
  }

  if (send_to_transport_layer) 
  {
    wms_ota_msg_s_type wms_msg;
    wms_msg.format = WMS_FORMAT_GW_PP;
    wms_ts_convert_from_bcd_address(&(mn_msg_ptr->RP_DA), &wms_msg.address);
    wms_msg.data_len = MIN(mn_msg_ptr->rpdu_length, sizeof(mn_msg_ptr->rpdu));
    (void)memscpy( wms_msg.data,
                   wms_msg.data_len,
                   mn_msg_ptr->rpdu,
                   wms_msg.data_len );

    if (NULL != transport_ptr->mo_sms_cb)
    {
      (void) transport_ptr->mo_sms_cb( transport_ptr->as_id, &wms_msg );
    }
  }
}

#endif  /* FEATURE_GWSMS */

/*=========================================================================
FUNCTION  wms_msg_send_transport_reg_event

DESCRIPTION
  Fills up the transport reg event info and notifies the client/clients
  about transport reg event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_msg_send_transport_reg_event(
  wms_msg_transport_s_type  *transport_ptr,
  wms_client_s_type         *client_ptr
)
{
  wms_msg_event_info_s_type *msg_event_ptr;

  msg_event_ptr = (wms_msg_event_info_s_type *)wms_mem_malloc(sizeof(wms_msg_event_info_s_type));

  if (NULL != msg_event_ptr)
  {
    if (NULL != transport_ptr)
    {
      msg_event_ptr->transport_reg_info.transport_cap = transport_ptr->cap;

      if (WMS_MSG_TRANSPORT_CAP_3GPP_IMS_LIMITED == transport_ptr->cap)
      {
        msg_event_ptr->transport_reg_info.transport_cap = WMS_MSG_TRANSPORT_CAP_3GPP_IMS;
      }
      else if (WMS_MSG_TRANSPORT_CAP_3GPP2_IMS_LIMITED == transport_ptr->cap)
      {
        msg_event_ptr->transport_reg_info.transport_cap = WMS_MSG_TRANSPORT_CAP_3GPP2_IMS;
      }

      msg_event_ptr->transport_reg_info.transport_type = transport_ptr->transport_type;
      msg_event_ptr->transport_reg_info.is_registered  = (NULL != transport_ptr->mo_sms_cb) ? TRUE : FALSE;
      msg_event_ptr->transport_reg_info.as_id          = transport_ptr->as_id;

      MSG_HIGH_2( "Current transport cap = %d; transport capability notified to clients = %d",
                  transport_ptr->cap,
                  msg_event_ptr->transport_reg_info.transport_cap );

      if (NULL != client_ptr)
      {
        if (NULL != client_ptr->msg_event_cb)
        {
          client_ptr->msg_event_cb(WMS_MSG_EVENT_TRANSPORT_REG, msg_event_ptr, NULL);
        }
      }
      else
      {
        (void)wms_msg_event_notify(WMS_MSG_EVENT_TRANSPORT_REG, msg_event_ptr);
      }
    }
    else
    {
      MSG_HIGH_0("Failed to notify WMS_MSG_EVENT_TRANSPORT_REG event, transport_ptr is NULL ");
    }
  }
  else
  {
    MSG_ERROR_0("msg_event_ptr is NULL");
  }

  WMS_MEM_FREEIF(msg_event_ptr);
} /* wms_msg_send_transport_reg_event() */

/*=========================================================================
FUNCTION
  wms_msg_reg_transport_reg_proc

DESCRIPTION
  Process the (de)registration request from SMS transport layer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=========================================================================*/
void  wms_msg_transport_reg_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_msg_transport_id_type   transport_id = cmd_ptr->cmd.msg_transport_reg.transport_id;
  wms_msg_transport_s_type   *transport_ptr = wms_msg_transport_ptr(transport_id);

  if (NULL != transport_ptr)
  {
    MSG_HIGH_0("wms_msg_transport_reg_proc()");

    if (cmd_ptr->cmd.msg_transport_reg.cb_updated)
    {
      transport_ptr->mo_sms_cb = cmd_ptr->cmd.msg_transport_reg.mo_sms_cb;

      if (NULL != transport_ptr->mo_sms_cb)
      {
        transport_ptr->as_id = cmd_ptr->hdr.as_id;
      }
    }
    else
    {
       transport_ptr->cap = cmd_ptr->cmd.msg_transport_reg.transport_cap;
    }
    
    if (WMS_MSG_TRANSPORT_CAP_3GPP_IMS_LIMITED == transport_ptr->cap)
    {
      MSG_HIGH_0("IMS only allows MO admin SMS in 3GPP.");
      cfg_s_ptr->admin_sms_only = TRUE;
    }
    else if (WMS_MSG_TRANSPORT_CAP_3GPP2_IMS_LIMITED == transport_ptr->cap)
    {
      MSG_HIGH_0("IMS only allows MO admin SMS in 3GPP2.");
      cfg_s_ptr->admin_sms_only = TRUE;
    }
    else
    {
      cfg_s_ptr->admin_sms_only = FALSE;
    }

#ifdef FEATURE_GWSMS
    if (NULL == transport_ptr->mo_sms_cb)
    {
       if (transport_ptr->is_mn_registered)
       {
          SM_RL_IMS_MSG_T   mn_msg;

          memset(&mn_msg, 0, sizeof(mn_msg));
          mn_msg.Ims_msg_type = SMS_IMS_ERROR_IND;
          mn_msg.cause = SMS_IMS_TEMP_ERROR;
          mn_msg.rpdu_length = 0;
          mn_msg.as_id = transport_ptr->as_id;
          memset(mn_msg.rpdu, 0, sizeof(mn_msg.rpdu));

        (void) UASMS_put_mn_ims_msg(mn_msg);

          MSG_HIGH_0("Deregister IMS with NAS");
          transport_ptr->is_mn_registered = FALSE;
       }
    }
    else
    {
       if (!transport_ptr->is_mn_registered)
       {
          MSG_HIGH_0("Register IMS with NAS");
          transport_ptr->is_mn_registered = TRUE;
       }
    }
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
    if (NULL == transport_ptr->mo_sms_cb)
    {
      wms_msg_cdma_message_record_type  *rec_ptr = NULL;

      rec_ptr = msg_s_ptr->mc_msg.rec_ptr;

      if(NULL != rec_ptr)
      {
        if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == rec_ptr->transport_type)
        {
          MSG_HIGH_0("Deregister IMS with WMS, so WMS is retrying the SMS after retry time interval");
          wms_msg_mc_mo_status(WMS_ERROR_TEMP,WMS_SIP_TEMP_ERROR_S,WMS_BEARER_IMS_3GPP2);
        }
      }
        
    }
#endif /* FEATURE_CDSMS */
    
    wms_msg_send_transport_reg_event(transport_ptr, NULL);
  }
  else
  {
    MSG_HIGH_0("Invalid transport ID");
  }

  return;

}

/*=========================================================================
FUNCTION
  wms_msg_transport_reg_mo_sms_cb

DESCRIPTION
  Allow SMS transport layer to register with WMS with a specified function callback
  to be used to send SMS over the transport layer. If a null pointer is passed,
  the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_UNSUPPORTED_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type  wms_msg_transport_ms_reg_mo_sms_cb
(
  wms_msg_transport_id_type         transport_id,
  sys_modem_as_id_e_type            as_id,
  wms_msg_transport_mo_sms_cb_type  mo_sms_cb
)
{
  wms_cmd_type              *cmd_ptr = NULL;

  if (mo_sms_cb != NULL)
  {
    MSG_HIGH_1("TRANSPORT %d registers with WMS.", transport_id);
  }
  else
  {
    MSG_HIGH_1("TRANSPORT %d deregisters with WMS.", transport_id);
  }

  cmd_ptr = wms_get_cmd_buf();
  if (NULL == cmd_ptr)
  {
    return WMS_OUT_OF_RESOURCES_S;
  }

  cmd_ptr->hdr.cmd_id      = WMS_CMD_MSG_TRANSPORT_REG;
  cmd_ptr->hdr.client_id   = (uint8)WMS_CLIENT_TYPE_INTERNAL;
  cmd_ptr->hdr.cmd_cb      = NULL;
  cmd_ptr->hdr.user_data   = (void*)NULL;
  cmd_ptr->hdr.appInfo_ptr = NULL;
  cmd_ptr->hdr.as_id       = as_id;

  cmd_ptr->cmd.msg_transport_reg.transport_id = transport_id;
  cmd_ptr->cmd.msg_transport_reg.mo_sms_cb = mo_sms_cb;
  cmd_ptr->cmd.msg_transport_reg.cb_updated = TRUE;

  wms_put_cmd(cmd_ptr);

  return WMS_OK_S;
}


/*=========================================================================
FUNCTION
  wms_msg_transport_cap_update

DESCRIPTION
  Allow SMS transport layer to notify WMS when its registered capability chanage

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_UNSUPPORTED_S
  WMS_OUT_OF_RESOURCES_S;

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type  wms_msg_transport_ms_cap_update
(
  wms_msg_transport_id_type         transport_id,
  sys_modem_as_id_e_type            as_id,
  wms_msg_transport_cap_type        transport_cap
)
{
  wms_cmd_type              *cmd_ptr = NULL;


  cmd_ptr = wms_get_cmd_buf();
  if (NULL == cmd_ptr)
  {
    return WMS_OUT_OF_RESOURCES_S;
  }

  cmd_ptr->hdr.cmd_id      = WMS_CMD_MSG_TRANSPORT_REG;
  cmd_ptr->hdr.client_id   = (uint8)WMS_CLIENT_TYPE_INTERNAL;
  cmd_ptr->hdr.cmd_cb      = NULL;
  cmd_ptr->hdr.user_data   = (void*)NULL;
  cmd_ptr->hdr.appInfo_ptr = NULL;
  cmd_ptr->hdr.as_id       = as_id;

  cmd_ptr->cmd.msg_transport_reg.transport_id = transport_id;
  cmd_ptr->cmd.msg_transport_reg.transport_cap = transport_cap;
  cmd_ptr->cmd.msg_transport_reg.cb_updated = FALSE;
  wms_put_cmd(cmd_ptr);

  return WMS_OK_S;

}

/*=========================================================================
FUNCTION
  wms_msg_transport_nw_reg_status_update

DESCRIPTION
  Allow SMS transport layer to notify WMS when its network registration status change

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_UNSUPPORTED_S

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type  wms_msg_transport_ms_nw_reg_status_update
(
  wms_msg_transport_id_type               transport_id,
  sys_modem_as_id_e_type                  as_id,
  wms_msg_transport_nw_reg_status_e_type  transport_nw_reg_status
)
{

  wms_msg_transport_s_type        *transport_ptr = wms_msg_transport_ptr( transport_id );
  wms_client_s_type               *client_ptr;
  wms_msg_transport_status_s_type  status;
  wms_msg_transport_status_s_type  ob_status;  // Outbound data
  uint8                            i;

  MSG_HIGH_1( "In wms_msg_transport_nw_reg_status_update() receive transport status = %d",
              transport_nw_reg_status);

  if (NULL == transport_ptr)
  {
    return WMS_UNSUPPORTED_S;
  }

  transport_ptr->nw_reg_status = transport_nw_reg_status;
  
  status.transport_status = transport_nw_reg_status;
  status.transport_cap    = transport_ptr->cap;
  status.transport_type   = transport_ptr->transport_type;
  status.as_id            = as_id;
  
  if (WMS_MSG_TRANSPORT_CAP_3GPP_IMS_LIMITED == transport_ptr->cap)
  {
    MSG_HIGH_0("IMS only allows MO admin SMS in 3GPP.");
    status.transport_cap = WMS_MSG_TRANSPORT_CAP_3GPP_IMS;
  }
  else if (WMS_MSG_TRANSPORT_CAP_3GPP2_IMS_LIMITED == transport_ptr->cap)
  {
    MSG_HIGH_0("IMS only allows MO admin SMS in 3GPP2.");
    status.transport_cap = WMS_MSG_TRANSPORT_CAP_3GPP2_IMS;
  }

  /* Notify clients
  */
  for( i=0; i< (uint8) WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr_fetch( (wms_client_id_type) i );
    if( client_ptr != NULL &&
        client_ptr->activated == TRUE &&
        client_ptr->transport_status_cb )
    {
        MSG_HIGH_1("Notify client %d transport status change", i);

        // Update the outbound data for each client to avoid data
        // tampering or corruption for downstream clients
        ob_status = status;
        client_ptr->transport_status_cb(&ob_status);
    }
    WMS_CLIENT_PTR_RELEASEIF(client_ptr);
  }

  return WMS_OK_S;
}

/*=========================================================================
FUNCTION
wms_msg_map_mn_to_wms_status

DESCRIPTION
Mapping function to map MN status to WMS status.

DEPENDENCIES
None

RETURN VALUE
wms_status_e_type

SIDE EFFECTS
None.

=========================================================================*/
wms_status_e_type wms_msg_map_mn_to_wms_status(sms_ims_state_e_type mn_status)
{
  wms_status_e_type wms_status;

  MSG_HIGH_1("Received mn_status %d", mn_status);

  switch (mn_status)
  {
  case SMS_OK:
    wms_status = WMS_OK_S;
    break;

  case SMS_ACK_OK:
    wms_status = WMS_ACK_OK_S;
    break; 

  case SMS_TRANSACTION_INVALID:
    wms_status = WMS_INVALID_TRANSACTION_ID_S;
    break;

  case SMS_TRANSACTION_ONGOING:
    wms_status = WMS_TERMINAL_BUSY_S;
    break;

  case SMS_TRANSACTION_UNKNOWN:
  default:
    wms_status = WMS_GENERAL_ERROR_S;
    break;

  }

  return wms_status;
}/* wms_msg_map_mn_to_wms_status() */

/*=========================================================================
FUNCTION
  wms_msg_transport_rpt_ind

DESCRIPTION
  Function called by SMS transport layer to notify WMS
  when there is a MT SMS or MO status report.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_UNSUPPORTED_S
  WMS_NULL_PTR_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type  wms_msg_transport_ms_rpt_ind
(
  wms_msg_transport_rpt_ind_s_type   *rpt_ind_ptr,
  sys_modem_as_id_e_type              as_id
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  wms_status_e_type st = WMS_OK_S;
  if (rpt_ind_ptr == NULL)
  {
    MSG_ERROR_0("rpt_ind_ptr is NULL");
    return WMS_NULL_PTR_S;
  }

  MSG_HIGH_1("Got transport indication %d", rpt_ind_ptr->ind_type);

  /* look for indication type that we are interested in */
  switch (rpt_ind_ptr->ind_type)
  {
#ifdef FEATURE_CDSMS
   case WMS_MSG_TRANSPORT_3GPP2_MO_SMS_STATUS_IND:
      wms_msg_mc_mo_status(
        (WMS_OK_S == rpt_ind_ptr->mo_status)? WMS_ERROR_NONE : WMS_ERROR_TEMP,
        rpt_ind_ptr->mo_status,
        WMS_BEARER_IMS_3GPP2);

      break;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
   case WMS_MSG_TRANSPORT_3GPP_MO_SMS_STATUS_IND:
      // Only forward nagative ack from IMS to NAS and ignore the positive one
      // because NAS does not handle the positive ack
      if (WMS_OK_S != rpt_ind_ptr->mo_status) {
        SM_RL_IMS_MSG_T   mn_msg;

        mn_msg.Ims_msg_type = SMS_IMS_ERROR_IND;
        mn_msg.cause = wms_msg_trasnsport_map_wms_to_mn_status(rpt_ind_ptr->mo_status);
        mn_msg.rpdu_length = 0;
        mn_msg.as_id = as_id;
		mn_msg.ims_cause_value = rpt_ind_ptr->ims_cause_code;
        if (WMS_SIP_TEMP_ERROR_S == rpt_ind_ptr->mo_status && rpt_ind_ptr->retry_after_time > 0)
        {
           mn_msg.retry_interval.present = TRUE;
           mn_msg.retry_interval.value   = rpt_ind_ptr->retry_after_time;
        }
        else
        {
           mn_msg.retry_interval.present = FALSE;
           mn_msg.retry_interval.value   = 0;
        }
        memset(mn_msg.rpdu, 0, sizeof(mn_msg.rpdu));

       st = wms_msg_map_mn_to_wms_status(UASMS_put_mn_ims_msg(mn_msg));
      }
      break;
#endif /* FEATURE_GWSMS */

   case WMS_MSG_TRANSPORT_MT_SMS_IND:
#ifdef FEATURE_CDSMS
     if (WMS_FORMAT_CDMA == rpt_ind_ptr->mt_sms.format)
     {
        wms_msg_cdma_mt_msg(rpt_ind_ptr->mt_sms.format,
                            rpt_ind_ptr->mt_sms.data_len,
                            rpt_ind_ptr->mt_sms.data,
                            WMS_BEARER_IMS_3GPP2,
                            TRUE, // ack_pending
                            as_id);
     }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
     if (WMS_FORMAT_GW_PP == rpt_ind_ptr->mt_sms.format)
     {
        SM_RL_IMS_MSG_T   mn_msg;

        mn_msg.Ims_msg_type = SMS_IMS_RPDU_IND;
        mn_msg.cause = SMS_IMS_OK;
        mn_msg.rpdu_length = (byte) MIN(rpt_ind_ptr->mt_sms.data_len, sizeof(mn_msg.rpdu));
        (void)memscpy( mn_msg.rpdu,
                       mn_msg.rpdu_length,
                       rpt_ind_ptr->mt_sms.data,
                       mn_msg.rpdu_length);
        mn_msg.as_id = as_id;
       st = wms_msg_map_mn_to_wms_status(UASMS_put_mn_ims_msg(mn_msg));
     }
#endif /* FEATURE_GWSMS */
     break;


    default:
      /* indication that we are not interested in */
      st = WMS_UNSUPPORTED_S;
      break;

  } /* switch */


  return st;

} /* end of wms_msg_transport_rpt_ind() */

/*=========================================================================
FUNCTION
  wms_msg_transport_rpt_mwi_ind

DESCRIPTION
  Function called by SMS transport layer to notify WMS
  when the MWI information needs to be notified to clients.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_NULL_PTR_S
  WMS_INVALID_FORMAT_S
  WMS_OUT_OF_RESOURCES_S
 
SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type  wms_msg_transport_ms_rpt_mwi_ind
(
  wms_msg_transport_rpt_mwi_ind_s_type   *rpt_mwi_ind_ptr,
  sys_modem_as_id_e_type                  as_id
)
{
  wms_status_e_type st                   = WMS_OK_S;

  MSG_HIGH_0("Got transport MWI indication");

  if (NULL == rpt_mwi_ind_ptr)
  {
    MSG_ERROR_0("rpt_ind_ptr is NULL!");
    return WMS_NULL_PTR_S;
  }

  if (!asid_is_valid(as_id)) 
  {
    return WMS_GENERAL_ERROR_S;          
  }

  MSG_HIGH_1("Number of summary instances = %d", rpt_mwi_ind_ptr->num_instances_summary);
  MSG_HIGH_1("Number of detail instances = %d", rpt_mwi_ind_ptr->num_instances_details);
  
  do
  {
    uint8 address_size                                  = rpt_mwi_ind_ptr->addr_len;
    uint8 count                                         = rpt_mwi_ind_ptr->num_instances_summary;
    int buffer_size                                     = 0;
    wms_msg_mwi_summary_s_type *local_mwi_summary_copy  = NULL;
    wms_msg_mwi_detail_s_type *local_mwi_details_copy   = NULL;

    if (WMS_TRANSPORT_MWI_ADDR_LENGTH_MAX < address_size || WMS_TRANSPORT_MWI_MAX <= count)
    {
      MSG_ERROR_0("Invalid data passed by Transport Layer");
      st = WMS_INVALID_FORMAT_S;
      break;
    }

    /* Summary data is mandatory. */
    if (count <= 0)
    {
      MSG_ERROR_1("Invalid number of summary instances %d", count);
      st = WMS_INVALID_FORMAT_S;
      break;
    }

    /* Parse the data passed from transport (IMS) and pack it into event structure. */
    if (WMS_OK_S == st)
    {
      msg_event_info.transport_mwi_info.as_id = as_id;

      /* Copying the "Message Account" address. */
      msg_event_info.transport_mwi_info.address_len = address_size;
      (void)memscpy (msg_event_info.transport_mwi_info.msg_account_addr,
                     address_size + 1,
                     rpt_mwi_ind_ptr->ue_addr,
                     address_size);

      /* Copying MWI summary information. */
      msg_event_info.transport_mwi_info.summary_tuple_count = count;
      buffer_size = ((int)count) * sizeof(wms_msg_mwi_summary_s_type);
      local_mwi_summary_copy = (wms_msg_mwi_summary_s_type *)wms_mem_malloc(buffer_size);
      MSG_HIGH_3("count = %d, buffer_size = %d, summary type size = %d", count, buffer_size, sizeof(wms_msg_mwi_summary_s_type));

      if (NULL != local_mwi_summary_copy)
      {
        (void)memscpy (local_mwi_summary_copy,
                       buffer_size,
                       rpt_mwi_ind_ptr->transport_mwi_summary,
                       buffer_size);

		msg_event_info.transport_mwi_info.mwi_msg_summary = local_mwi_summary_copy;
      }
      else
      {
        MSG_ERROR_0("Memory heap is exhausted");
        st = WMS_OUT_OF_RESOURCES_S;
        break;
      }
        
      /* Copying MWI details information. */
      msg_event_info.transport_mwi_info.details_tuple_count = rpt_mwi_ind_ptr->num_instances_details;

      if (msg_event_info.transport_mwi_info.details_tuple_count > 0)
      {
        buffer_size = (rpt_mwi_ind_ptr->num_instances_details) * sizeof(wms_msg_mwi_detail_s_type);
        local_mwi_details_copy = (wms_msg_mwi_detail_s_type *)wms_mem_malloc(buffer_size);
        MSG_HIGH_3("count = %d, buffer_size = %d, detail type size = %d", 
                   rpt_mwi_ind_ptr->num_instances_details, 
                   buffer_size, 
                   sizeof(wms_msg_mwi_detail_s_type));

        if (NULL != local_mwi_details_copy)
        {
          (void)memscpy (local_mwi_details_copy,
                         buffer_size,
                         rpt_mwi_ind_ptr->transport_mwi_details,
                         buffer_size);

          msg_event_info.transport_mwi_info.mwi_msg_details = local_mwi_details_copy;
        }
        else
        {
          MSG_ERROR_0("Memory heap is exhausted");
          st = WMS_OUT_OF_RESOURCES_S;
          break;
        }
      }
      else
      {
        MSG_HIGH_0("No details on MWI instances provided");
        msg_event_info.transport_mwi_info.mwi_msg_details = NULL;
      }

      wms_msg_event_notify_single_client( WMS_CLIENT_TYPE_QMI,
                                          WMS_MSG_EVENT_TRANSPORT_MWI_NOTIFICATION,
                                          &msg_event_info );
        
      WMS_MEM_FREEIF(local_mwi_summary_copy);
      WMS_MEM_FREEIF(local_mwi_details_copy);
    }
  } while(0);

  return st;

} /* end of wms_msg_transport_rpt_ind() */

wms_status_e_type  wms_msg_transport_reg_mo_sms_cb
(
  wms_msg_transport_id_type         transport_id,
  wms_msg_transport_mo_sms_cb_type  mo_sms_cb
)
{
  wms_status_e_type st = WMS_OK_S;
  
  st = wms_msg_transport_ms_reg_mo_sms_cb(transport_id, SYS_MODEM_AS_ID_1, mo_sms_cb);
  return st;
}

wms_status_e_type  wms_msg_transport_cap_update
(
  wms_msg_transport_id_type         transport_id,
  wms_msg_transport_cap_type        transport_cap
)
{
  wms_status_e_type st = WMS_OK_S;

  st = wms_msg_transport_ms_cap_update( transport_id, SYS_MODEM_AS_ID_1, transport_cap );
  return st;
}

wms_status_e_type  wms_msg_transport_nw_reg_status_update
(
  wms_msg_transport_id_type               transport_id,
  wms_msg_transport_nw_reg_status_e_type  transport_nw_reg_status
)
{
  wms_status_e_type st = WMS_OK_S;
  
  st = wms_msg_transport_ms_nw_reg_status_update( transport_id, SYS_MODEM_AS_ID_1, transport_nw_reg_status );
  return st;
}

wms_status_e_type  wms_msg_transport_rpt_ind
(
  wms_msg_transport_rpt_ind_s_type   *rpt_ind_ptr
)
{
  wms_status_e_type st = WMS_OK_S;

  st = wms_msg_transport_ms_rpt_ind( rpt_ind_ptr, SYS_MODEM_AS_ID_1 );
  return st;
}

wms_status_e_type  wms_msg_transport_rpt_mwi_ind
(
  wms_msg_transport_rpt_mwi_ind_s_type   *rpt_mwi_ind_ptr
)
{
  wms_status_e_type st = WMS_OK_S;

  st = wms_msg_transport_ms_rpt_mwi_ind( rpt_mwi_ind_ptr, SYS_MODEM_AS_ID_1 );
  return st;
}

/*
*/
void wms_msg_process_cmd
(
  wms_cmd_type  *cmd_ptr
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( cmd_ptr->hdr.cmd_id )
  {
    case WMS_CMD_MSG_SEND:
      wms_msg_send_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_ACK:
      wms_msg_ack_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_READ:
      wms_msg_read_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_WRITE:
      wms_msg_write_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_DELETE:
      wms_msg_delete_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_DELETE_ALL:
      wms_msg_delete_all_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_READ_TEMPLATE:
      wms_msg_read_template_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_WRITE_TEMPLATE:
      wms_msg_write_template_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_DELETE_TEMPLATE:
      wms_msg_delete_template_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_DELETE_TEMPLATE_ALL:
      wms_msg_delete_template_all_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_MODIFY_TAG:
      wms_msg_modify_tag_proc( cmd_ptr );
      break;

#ifdef FEATURE_CDSMS
    case WMS_CMD_MSG_MC_MO_STATUS_E:
      wms_msg_mc_mo_status_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_MC_MT_MSG_E:
      wms_msg_cdma_mt_msg_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_MC_MWI_E:
      wms_msg_cdma_mt_mwi_proc( cmd_ptr );
      break;

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
    case WMS_CMD_MSG_READ_STS_REPORT:
      wms_msg_read_sts_report_proc(cmd_ptr);
      break;

    case WMS_CMD_MSG_WRITE_STS_REPORT:
      wms_msg_write_sts_report_proc(cmd_ptr);
      break;

    case WMS_CMD_MSG_DELETE_STS_REPORT:
      wms_msg_del_sts_report_proc(cmd_ptr);
      break;

    case WMS_CMD_MSG_DELETE_STS_REPORT_ALL:
      wms_msg_del_sts_report_all_proc(cmd_ptr);
      break;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GSTK
    case WMS_CMD_MSG_GSTK_EVT:
      wms_msg_gstk_evt_proc( cmd_ptr );
      break;
#endif /* FEATURE_GSTK */

   case WMS_CMD_MSG_TRANSPORT_REG:
      wms_msg_transport_reg_proc(cmd_ptr);
      break;

    default:
      MSG_ERROR_1("Invalid cmd: %d", cmd_ptr->hdr.cmd_id);
      break;
  }

  /* done */
  return;

} /* wms_msg_process_cmd() */



/*
*/
wms_transaction_id_type wms_msg_get_next_tid
(
  void
)
{
  static  wms_transaction_id_type     tid = 0;

  return tid++;

} /* end of wms_msg_get_next_tid() */




#ifdef FEATURE_GWSMS

static wms_cmd_err_e_type wms_msg_do_internal_ack(
  sys_modem_as_id_e_type  as_id,
  wms_status_e_type     status,
  wms_tp_cause_e_type   tp_cause,
  wms_rp_cause_e_type   rp_cause,
  wms_cm_conn_id_type   connection_id
)
{
  uint8   tpdu[3];

  /* process ack generated internally
  */

  if (WMS_OK_S == status) {
     return wms_cm_gw_send_rl_report(as_id, connection_id, TRUE, 0, 0, 0);
  }

  tpdu[0] = 0;
  tpdu[1] = (uint8)tp_cause;
  tpdu[2] = 0;

  return wms_cm_gw_send_rl_report(as_id, connection_id, FALSE, (uint8)rp_cause, tpdu, 3);
} /* wms_msg_do_internal_ack() */

static wms_gw_tpdu_type_e_type wms_get_tpdu_type
(
  uint8 *data
)
{
  wms_gw_tpdu_type_e_type tpdu_type;

  tpdu_type = (wms_gw_tpdu_type_e_type) (data[0] & 0x03); /*bit 0 and bit 1*/

  if ((int)tpdu_type == 0) /*Deliver*/
  {
    tpdu_type = WMS_TPDU_DELIVER;
  }
  else if ((int)tpdu_type == 2)
  {
    tpdu_type = WMS_TPDU_STATUS_REPORT;
  }
  else if ((int)tpdu_type == 3)
  {
    tpdu_type = WMS_TPDU_RESERVED;
  }

  return tpdu_type;
}

#ifdef FEATURE_GWSMS_DEPERSO

/*===========================================================================
FUNCTION wms_msg_simlock_ota_unlock_cb()

DESCRIPTION
  Callback function called by SIMLOCK Module after the processing of
  simlock_ota_unlock deperso message processing

RETURN VALUE
  None

DEPENDENCIES
  Callback registered for SIMLOCK API: simlock_ota_unlock

SIDE EFFECTS
  None
===========================================================================*/
void wms_msg_simlock_ota_unlock_cb(
  simlock_result_enum_type              status,
  const simlock_ota_unlock_resp_type    resp_data,
  const void                           *user_data_ptr)
{
  wms_prov_app_info_s_type  *appInfo_ptr;
  wms_status_e_type          wmsret;

  MSG_HIGH_2( "wms_msg_simlock_ota_unlock_cb: status = %d, user_data_ptr = 0x%x",
              status,
              user_data_ptr );
  
  if (SIMLOCK_SUCCESS == status && NULL != user_data_ptr)
  {
    wms_simlock_ota_user_data_s_type *simlock_ota_user_data_ptr;
    simlock_ota_user_data_ptr = (wms_simlock_ota_user_data_s_type *) user_data_ptr;

    appInfo_ptr = wms_get_appinfo_by_session_id( simlock_ota_user_data_ptr->session_id );

    if (NULL != appInfo_ptr)
    {
      wms_msg_Gack_info.transaction_id  = simlock_ota_user_data_ptr->tid;
      wms_msg_Gack_info.u.gw.u.ack.pid  = simlock_ota_user_data_ptr->pid;
      wms_msg_Gack_info.u.gw.u.ack.dcs  = simlock_ota_user_data_ptr->dcs;

      wms_msg_Gack_info.message_mode                        = WMS_MESSAGE_MODE_GW;
      wms_msg_Gack_info.u.gw.success                        = TRUE;
      wms_msg_Gack_info.u.gw.tpdu_type                      = WMS_TPDU_DELIVER_REPORT_ACK;

      wms_msg_Gack_info.u.gw.u.ack.user_data_header_present = FALSE;
      wms_msg_Gack_info.u.gw.u.ack.mask                     = WMS_TPDU_MASK_PID | WMS_TPDU_MASK_DCS | WMS_TPDU_MASK_USER_DATA;
      wms_msg_Gack_info.u.gw.u.ack.user_data.num_headers    = 0;
      wms_msg_Gack_info.u.gw.u.ack.user_data.sm_len         = SIMLOCK_OTA_UNLOCK_RESP_DATA_LEN;

      (void)memscpy( wms_msg_Gack_info.u.gw.u.ack.user_data.sm_data,
                     SIMLOCK_OTA_UNLOCK_RESP_DATA_LEN,
                     resp_data,
                     SIMLOCK_OTA_UNLOCK_RESP_DATA_LEN );

      wmsret = wms_msg_ms_ack( (wms_client_id_type)WMS_CLIENT_TYPE_INTERNAL,
                               appInfo_ptr->asid,
                               NULL, /* cmd_cb */
                               NULL, /* user_data */
                               &wms_msg_Gack_info );
      if (WMS_OK_S != wmsret)
      {
        MSG_ERROR_0( "Out of cmd buffer. SIMLOCK ack dropped!" );
      }
    }
    else
    {
      MSG_ERROR_1( "appInfo_ptr is NULL for session_id: 0x%x", simlock_ota_user_data_ptr->session_id );
    }
  }
  WMS_MEM_FREEIF( user_data_ptr );
  
  return;
} /* wms_msg_simlock_ota_unlock_cb() */

/*===========================================================================
FUNCTION wms_msg_simlock_ota_unlock_proc()

DESCRIPTION
  Function to check if incoming GW MT SMS is of DEPERSO Type.
  If it is, then further processing is done by SIMLOCK Module
  Upon completeion callback is called by SIMLOCK API which tells
  the status based on which WMS ACKs for the MT SMS

RETURN VALUE
  None

DEPENDENCIES
  SIMLOCk module API: simlock_ota_unlock

SIDE EFFECTS
  None
===========================================================================*/
static boolean wms_msg_simlock_ota_unlock_proc(
  wms_prov_app_info_s_type      *appInfo_ptr,
  wms_cm_conn_id_type            uConnectionId,
  wms_gw_deliver_s_type         *deliver_ptr,
  wms_msg_transport_type_e_type  transport_type
)
{
  uint32                  i;

  if (NULL == appInfo_ptr)
  {
    return FALSE;
  }

  if ( (WMS_PID_ME_DEPERSONALIZE == deliver_ptr->pid) &&
       (WMS_MESSAGE_CLASS_1      == deliver_ptr->dcs.msg_class) )
  {
    /* This is a De-personalization msg; processing follows... */
  }
  else
  {
    return FALSE; /* SHORT-RETURN */
  }

  MSG_HIGH_0( "MT SMS: Incoming Depersonalization Message" );

  /* Allocate an MT record
  */
  if (wms_msg_get_free_mt_record( &i ) == FALSE)
  {
    /* Ack with error */
    (void) wms_msg_do_internal_ack( appInfo_ptr->asid,
                                    WMS_MT_MSG_FAILED_S,
                                    WMS_TP_CAUSE_ERROR_IN_MS,
                                    WMS_RP_CAUSE_TEMPORARY_FAILURE,
                                    uConnectionId );

    MSG_ERROR_0( "WMS MT FAILED: WMS_TP_CAUSE_ERROR_IN_MS, WMS_RP_CAUSE_TEMPORARY_FAILURE" );
  }
  else
  {
    simlock_ota_unlock_data_type      unlock_data;
    wms_simlock_ota_user_data_s_type *simlock_ota_user_data_ptr = NULL;

    /* Remember information about the MT message */
    msg_s_ptr->gw_mt_rec[i]->as_id          = appInfo_ptr->asid;
    msg_s_ptr->gw_mt_rec[i]->appInfo_ptr    = appInfo_ptr;
    msg_s_ptr->gw_mt_rec[i]->client_id      = (wms_client_id_type)WMS_CLIENT_TYPE_INTERNAL;
    msg_s_ptr->gw_mt_rec[i]->tid            = wms_msg_get_next_tid();
    msg_s_ptr->gw_mt_rec[i]->smi            = uConnectionId;
    msg_s_ptr->gw_mt_rec[i]->ack_client_id  = WMS_CLIENT_TYPE_INTERNAL;
    msg_s_ptr->gw_mt_rec[i]->user_data      = NULL;
    msg_s_ptr->gw_mt_rec[i]->transport_type = transport_type;
    msg_s_ptr->gw_mt_rec[i]->state          = WMS_MSG_STATE_WAIT_FOR_ACK;

    unlock_data.ota_data_len = MIN( deliver_ptr->user_data.sm_len, SIMLOCK_OTA_UNLOCK_DATA_MAX );
    (void)memscpy( unlock_data.ota_data,
                   SIMLOCK_OTA_UNLOCK_DATA_MAX,
                   deliver_ptr->user_data.sm_data,
                   unlock_data.ota_data_len );

    simlock_ota_user_data_ptr = wms_mem_malloc( sizeof( wms_simlock_ota_user_data_s_type ) );
    if (NULL == simlock_ota_user_data_ptr)
    {
      MSG_ERROR_0( "Run out of heap!" );
      (void)wms_msg_do_internal_ack( appInfo_ptr->asid,
                                     WMS_GENERAL_ERROR_S,
                                     WMS_TP_CAUSE_ERROR_IN_MS,
                                     WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED,
                                     uConnectionId );
    }
    else
    {
      /* remember information needed for the ack later */
      simlock_ota_user_data_ptr->tid        = msg_s_ptr->gw_mt_rec[i]->tid;
      simlock_ota_user_data_ptr->pid        = deliver_ptr->pid;
      simlock_ota_user_data_ptr->dcs        = deliver_ptr->dcs;
      simlock_ota_user_data_ptr->session_id = appInfo_ptr->mmgsdi_appInfo.session_id;

      if (SIMLOCK_SUCCESS != simlock_ota_unlock( &unlock_data,
                                                 (simlock_slot_enum_type)(appInfo_ptr->mmgsdi_appInfo.slot_id - 1),
                                                 wms_msg_simlock_ota_unlock_cb,
                                                 simlock_ota_user_data_ptr ))
      {
        MSG_ERROR_0( "NOT able to send simlock ota to MMGSDI!" );
        (void)wms_msg_do_internal_ack( appInfo_ptr->asid,
                                       WMS_GENERAL_ERROR_S,
                                       WMS_TP_CAUSE_ERROR_IN_MS,
                                       WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED,
                                       uConnectionId );
        wms_mem_free( simlock_ota_user_data_ptr );
      }
         /* Do not free deperso_cb_user_data_ptr here. Free will be done in the callback */
    }
  }

  return TRUE;
} /* wms_msg_simlock_ota_unlock_proc() */

#endif /* FEATURE_GWSMS_DEPERSO */

#ifdef FEATURE_GWSMS_PP_DOWNLOAD
/*
*/
static boolean wms_msg_pp_download_proc(
  wms_prov_app_info_s_type      *appInfo_ptr,
  wms_cm_conn_id_type            uConnectionId,
  cm_called_party_bcd_no_T      *pSCA, /* originating service center address */
  uint8                         *pubTPDU,
  int                            nTPDULen,
  wms_gw_deliver_s_type         *deliver_ptr,
  wms_msg_transport_type_e_type  transport_type
)
{
  gstk_address_type       gstk_address;
  uint32                  i;
  gstk_status_enum_type   gstk_status;
  wms_address_s_type      temp_wms_addr;

  if ( (NULL == appInfo_ptr) ||
       !appInfo_ptr->pp_data_download_enabled )
  {
     return FALSE;
  }

  if ( ( (deliver_ptr->pid == WMS_PID_SIM_DATA_DOWNLOAD) ||
         (deliver_ptr->pid == WMS_PID_ANSI136_R_DATA) ) &&
       (deliver_ptr->dcs.msg_class == WMS_MESSAGE_CLASS_2) )
  {
    /* This is a PP download msg; processing follows... */
  }
  else
  {
    return FALSE; /* SHORT-RETURN */
  }

  MSG_HIGH_0("MT SMS: Incoming PP Download Message");

  /* Allocate an MT record
  */
  if (wms_msg_get_free_mt_record( &i ) == FALSE)
  {
    /* Ack with error */
    (void) wms_msg_do_internal_ack( appInfo_ptr->asid,
                                    WMS_MT_MSG_FAILED_S,
                                    WMS_TP_CAUSE_ERROR_IN_MS,
                                    WMS_RP_CAUSE_TEMPORARY_FAILURE,
                                    uConnectionId );

    MSG_ERROR_0("WMS MT FAILED,TP CAUSE : WMS_TP_CAUSE_ERROR_IN_MS ,RP_CAUSE: WMS_RP_CAUSE_TEMPORARY_FAILURE");
  }
  else
  {
    gstk_sms_tpdu_type  gstk_sms_tpdu;
    wms_transaction_id_type           pp_download_tid;
    wms_pid_e_type                    pp_download_pid;
    wms_gw_dcs_s_type                 pp_download_dcs;

    pp_download_tid = wms_msg_get_next_tid();
    pp_download_pid = deliver_ptr->pid;
    pp_download_dcs = deliver_ptr->dcs;

    /* Remember information about the MT message
    */
    msg_s_ptr->gw_mt_rec[i]->as_id          = appInfo_ptr->asid;
    msg_s_ptr->gw_mt_rec[i]->appInfo_ptr    = appInfo_ptr;
    msg_s_ptr->gw_mt_rec[i]->client_id      = (wms_client_id_type)WMS_CLIENT_TYPE_INTERNAL;
    msg_s_ptr->gw_mt_rec[i]->tid            = pp_download_tid;
    msg_s_ptr->gw_mt_rec[i]->smi            = uConnectionId;
    msg_s_ptr->gw_mt_rec[i]->ack_client_id  = WMS_CLIENT_TYPE_INTERNAL;
    msg_s_ptr->gw_mt_rec[i]->user_data      = NULL;
    msg_s_ptr->gw_mt_rec[i]->transport_type = transport_type;
    msg_s_ptr->gw_mt_rec[i]->state          = WMS_MSG_STATE_WAIT_FOR_ACK;

    /* fill up GSTK cmd info
    */
    if ( (gstk_parse_ton_npi( pSCA->data[0],
                              &gstk_address.TON,
                              &gstk_address.NPI ) ) != GSTK_SUCCESS)
    {
      MSG_ERROR_0("NOT able to parse TON, NPI!");
      (void)wms_msg_do_internal_ack( appInfo_ptr->asid,
                                     WMS_GENERAL_ERROR_S,
                                     WMS_TP_CAUSE_ERROR_IN_MS,
                                     WMS_RP_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE,
                                     uConnectionId );
    }

    /* Convert address format to GSTK format */
    wms_ts_convert_from_bcd_address(pSCA, &temp_wms_addr);
    (void)memscpy( gstk_address.address,
                   temp_wms_addr.number_of_digits,
                   temp_wms_addr.digits,
                   temp_wms_addr.number_of_digits );
    gstk_address.length = MIN(temp_wms_addr.number_of_digits, GSTK_MAX_ADDRESS_LEN);

    gstk_sms_tpdu.length = (uint8)nTPDULen;
    gstk_sms_tpdu.tpdu = wms_mem_malloc(gstk_sms_tpdu.length);

    if (NULL == gstk_sms_tpdu.tpdu)
    {
      MSG_ERROR_0("Run out of heap!");
      (void)wms_msg_do_internal_ack( appInfo_ptr->asid,
                                     WMS_GENERAL_ERROR_S,
                                     WMS_TP_CAUSE_ERROR_IN_MS,
                                     WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED,
                                     uConnectionId );
    }
    else
    {
      wms_gstk_user_data_s_type* user_data_ptr;

      (void)memscpy( gstk_sms_tpdu.tpdu,
                     gstk_sms_tpdu.length,
                     pubTPDU,
                     (unsigned int)nTPDULen );

      /* Send ENVELOPE command to the card
      */
      user_data_ptr = wms_mem_malloc(sizeof(wms_gstk_user_data_s_type));
      if (NULL == user_data_ptr)
      {
         MSG_ERROR_0("Run out of heap!");
         (void)wms_msg_do_internal_ack( appInfo_ptr->asid,
                                        WMS_GENERAL_ERROR_S,
                                        WMS_TP_CAUSE_ERROR_IN_MS,
                                        WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED,
                                        uConnectionId );
      }
      else
      {
         user_data_ptr->as_id = appInfo_ptr->asid;
         user_data_ptr->msg_mode = WMS_MESSAGE_MODE_GW;
         user_data_ptr->pp_download_tid = pp_download_tid;
         user_data_ptr->pp_download_pid = pp_download_pid;
         user_data_ptr->pp_download_dcs = pp_download_dcs;

         MSG_HIGH_1("Send GSTK slot ID %d", appInfo_ptr->mmgsdi_appInfo.slot_id);

         gstk_status = gstk_slot_send_envelope_sms_pp_dl_command( (gstk_slot_id_enum_type)appInfo_ptr->mmgsdi_appInfo.slot_id,
                                                                  (uint32)user_data_ptr,
                                                                  FALSE,
                                                                  &gstk_address,
                                                                  &gstk_sms_tpdu,
                                                                  wms_msg_gstk_evt_cb );

         wms_mem_free( gstk_sms_tpdu.tpdu );

         if( gstk_status != GSTK_SUCCESS )
         {
           MSG_ERROR_0("SMS PP download failed");
           (void)wms_msg_do_internal_ack( appInfo_ptr->asid,
                                          WMS_GENERAL_ERROR_S,
                                          WMS_TP_CAUSE_SIM_DATA_DOWNLOAD_ERROR,
                                          WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED,
                                          uConnectionId );

           MSG_ERROR_0("TP CAUSE : WMS_TP_CAUSE_SIM_DATA_DOWNLOAD_ERROR,RP_CAUSE: WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED, ");
         }

         /* Otherwise, the GSTK event later will trigger WMS to send
            the Deliver Report to the network
         */
      }
    }
  }

  return TRUE;
} /* wms_msg_pp_download_proc() */

#endif /* FEATURE_GWSMS_PP_DOWNLOAD */

#ifdef FEATURE_GWSMS_BROADCAST

/*===========================================================================
FUNCTION   wms_msg_gw_cb_data_download_proc

DESCRIPTION
  Procedure to download the CB Data to the card on verification

DEPENDENCIES
  None

RETURNS
  TRUE: If the cb_page was meant to be downloaded
  FALSE: If not

SIDE EFFECTS
  None
===========================================================================*/
static boolean wms_msg_gw_cb_data_download_proc(
  wms_prov_app_info_s_type      *appInfo_ptr,
  uint8                         *cb_data
)
{
  boolean                        ret_val = FALSE;
#ifdef FEATURE_GSTK

  if ( cb_data != NULL )
  {
    if ( (NULL != appInfo_ptr) &&
         appInfo_ptr->cb_data_download_enabled &&
         asid_is_valid(appInfo_ptr->asid) )
    {
      wms_gw_cb_page_header_s_type  *page_header_ptr = NULL;
      /* Atleast one slot support CB Data Download */

      /* Decode CB Page Header */
      if ((page_header_ptr = wms_mem_malloc(sizeof(wms_gw_cb_page_header_s_type))) != NULL)
      {
        if (wms_ts_decode_gw_cb_page_header(cb_data, page_header_ptr) != 0)
        {
          uint8 i;

          /* Check for Service ID in the range of 0x1000 - 0x10FF */
          for ( i = 0; i < appInfo_ptr->bc.gw.sim_cbmid_list_size; i++)
          {
            if (page_header_ptr->cb_srv_id == appInfo_ptr->bc.gw.sim_cbmid_list[i])
            {
              gstk_cb_page_type      gstk_cb_page;
              gstk_status_enum_type  gstk_st;

              /* This is a CB Data Download Message */
              ret_val = TRUE;

              /* Fill up cb_page_ptr to be sent to GSDI */
              gstk_cb_page.length       = WMS_GW_CB_PAGE_SIZE;
              gstk_cb_page.cb_page_data = cb_data;    /* Pointer Assignment */

              gstk_st = gstk_slot_send_envelope_cell_broadcast_dl_command( (gstk_slot_id_enum_type)appInfo_ptr->mmgsdi_appInfo.slot_id,
                                                                           (uint32)WMS_MESSAGE_MODE_GW,
                                                                           &gstk_cb_page,
                                                                           wms_msg_gstk_evt_cb );

              if (gstk_st != GSTK_SUCCESS)
              {
                MSG_ERROR_0("Failure Sending CB Download Envelope Command");
              }
              break;
            }
          }
        }
        wms_mem_free(page_header_ptr);
      }
      else
      {
        MSG_ERROR_0("Memory Allocation Failed");
      }
    }
  }
#endif /* FEATURE_GSTK */

  return ret_val;
} /* wms_msg_gw_cb_data_download_proc() */

#endif /* FEATURE_GWSMS_BROADCAST */

/* REPLACE_SMS + */
/*=========================================================================
FUNCTION
  wms_msg_message_should_replace_stored

DESCRIPTION
  Determines whether an incoming (mt) message should replace the one in
  the message store by comparing tpdu type, PID, and originating address.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: if the incoming message should replace the one stored
  FALSE: if the incoming message does not meet the requirements for replace

SIDE EFFECTS

=========================================================================*/
boolean wms_msg_message_should_replace_stored(
  wms_pid_e_type              mtmsg_pid,            // IN: pid of incoming message
  wms_address_s_type         *mtmsg_address,        // IN: origination address of incoming message
  wms_client_ts_data_s_type  *stored_client_ts_data // IN: stored message information
)
{

  if ( (NULL != mtmsg_address) &&
       (NULL != stored_client_ts_data) &&
       (WMS_TPDU_DELIVER                == stored_client_ts_data->u.gw_pp.tpdu_type) &&
       (mtmsg_pid                       == stored_client_ts_data->u.gw_pp.u.deliver.pid) &&
       (mtmsg_address->number_of_digits == stored_client_ts_data->u.gw_pp.u.deliver.address.number_of_digits) &&
       (memcmp( stored_client_ts_data->u.gw_pp.u.deliver.address.digits,
                mtmsg_address->digits,
                mtmsg_address->number_of_digits) == 0) )
  {
       return TRUE;
  }

  return FALSE;
}

/*=========================================================================
FUNCTION
  wms_msg_search_for_replacement

DESCRIPTION
  Searches the memory stores for a message matching the replace-pid and
  the originating address.

DEPENDENCIES
  None

RETURN VALUE
  WMS_CMD_ERR_NONE:         The search completed without error. The value
                            returned in replace_index indicates the index
                            in the memory store searched where a match was
                            found.  If no match was found then replace_index
                            will be set to WMS_DUMMY_MESSAGE_INDEX.
  WMS_CMD_ERR_BAD_PARM:     If address or replace_index not provided (null)
  WMS_CMD_ERR_UNSUPPORTED:  If memory store not supported
  WMS_CMD_ERR_BAD_INDEX:    If an index used for the search went out of range
  WMS_CMD_ERR_MSG_SIM_READ: If an error occurred reading SIM data
  WMS_CMD_ERR_MSG_NV_READ:  If an error occurred reading NV data
  WMS_CMD_ERR_MSG_DECODE:   If an error occurred during message decoding

SIDE EFFECTS

=========================================================================*/
wms_cmd_err_e_type wms_msg_search_mem_store_for_replacement(
  wms_prov_app_info_s_type     *appInfo_ptr,      // IN
  wms_memory_store_e_type       mem_store,        // IN: store to search
  wms_pid_e_type                pid,              // IN: replace-id to match
  wms_address_s_type           *address,          // IN: origination addr to match
  wms_message_index_type       *replace_index     // OUT: index of match
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  uint32                      i;
  uint32                      max_msg_slots = 0;
  uint32                      msg_index = 0;
  wms_client_ts_data_s_type  *local_client_ts_data;
  sys_modem_as_id_e_type      as_id;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
     return WMS_CMD_ERR_NULL_PTR;
  }

  as_id = appInfo_ptr->asid;

  MSG_HIGH_3( "wms_msg_search_mem_store_for_replacement(as_id %d, mem_store %d, pid 0x%02X)",
              as_id,
              mem_store,
              pid );

  if ( (NULL == replace_index) || (NULL == address) )
  {
     MSG_ERROR_2( "Required PARM is NULL: replace_index 0x%x, address 0x%x",
                  replace_index,
                  address );

     return WMS_CMD_ERR_BAD_PARM;
  }

  *replace_index = WMS_DUMMY_MESSAGE_INDEX;

  local_client_ts_data = (wms_client_ts_data_s_type *)wms_mem_malloc (sizeof (wms_client_ts_data_s_type));

  if (NULL == local_client_ts_data)
  {
     MSG_ERROR_0("Memory allocation failed");
     return WMS_CMD_ERR_NULL_PTR;
  }

  switch( mem_store )
  {
    case WMS_MEMORY_STORE_RAM_GW:
      max_msg_slots = WMS_MAX_RAM_MESSAGES;

      if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
      {
        max_msg_slots /= 2;
      }

      for ( msg_index = 0; msg_index < max_msg_slots; msg_index++ )
      {

        // Map the message slot (relative to as_id) to the array index
        cmd_err = wms_msg_map_gw_ram_index(as_id, msg_index, &i);

        if (WMS_CMD_ERR_NONE != cmd_err)
        {
          wms_check_valid_message_index(msg_index);
          break; // Error, done with for
        }

        if (NULL == cfg_s_ptr->ram_gw_sms[i])
        {
          //MSG_ERROR_1("Empty  message %d", msg_index);
          continue;  // ignore empties
        }

        (void)wms_ts_unpack_sim_sms( cfg_s_ptr->ram_gw_sms[i],
                                     &sim_sms_unpacked );

        if (wms_ts_decode(&sim_sms_unpacked.raw_ts_data, local_client_ts_data) != WMS_OK_S)
        {
          cmd_err = WMS_CMD_ERR_MSG_DECODE;
          MSG_ERROR_3( "Error decoding message: storage: %d, index: %d, as_id: %d",
                       mem_store,
                       i,
                       as_id );

          break;  // Error, done with for
        }

        if (wms_msg_message_should_replace_stored( pid, address, local_client_ts_data) )
        {
          // The message in the store is a deliver message and both the pid
          // and the origination address match.  This is a message to replace.
          *replace_index = msg_index;
          break; // done with for
        }
      }
      break;

    case WMS_MEMORY_STORE_SIM:
    case WMS_MEMORY_STORE_USIM:
      if (!wms_sim_mmgsdi_mem_store_supported(appInfo_ptr->mmgsdi_appInfo.app_type, mem_store))
      {
         cmd_err =  WMS_CMD_ERR_UNSUPPORTED;
         break; // done with case
      }

      for ( i = 0; i < (uint32) appInfo_ptr->sms_rec_count; i++ )
      {

        if (appInfo_ptr->sms_rec_tags[i] == WMS_TAG_NONE)
        {
          continue;  // skip empties
        }
        else if ( wms_sim_mmgsdi_read_data( appInfo_ptr,
                                            appInfo_ptr->sms_file_enum,
                                            (mmgsdi_rec_num_type)i,
                                            appInfo_ptr->sms_rec_len,
                                            sim_data,
                                            sizeof(sim_data) ) != MMGSDI_SUCCESS)
        {
          cmd_err = WMS_CMD_ERR_MSG_SIM_READ;
          MSG_ERROR_3( "Error reading message: storage: %d, index: %d, as_id: %d",
                       mem_store,
                       i,
                       as_id );
          break; // Error, done with for
        }
        else
        {
          sim_sms.status = sim_data[0];
          sim_sms.len = (uint8) MIN(appInfo_ptr->sms_rec_len-1, WMS_SIM_SMS_MAX-1);

          (void)memscpy( (void *)sim_sms.data,
                         sim_sms.len,
                         sim_data + 1,
                         sim_sms.len );

          (void)wms_ts_unpack_sim_sms( &sim_sms,
                                       &sim_sms_unpacked );

          if (wms_ts_decode(&sim_sms_unpacked.raw_ts_data, local_client_ts_data) != WMS_OK_S)
          {
            cmd_err = WMS_CMD_ERR_MSG_DECODE;
            MSG_ERROR_3( "Error decoding message: storage: %d, index: %d, as_id: %d",
                         mem_store,
                         i,
                         as_id );
            break;  // Error, done with for
          }


          if (wms_msg_message_should_replace_stored( pid, address, local_client_ts_data) )
          {
             // The message in the store is a deliver message and both the pid
             // and the origination address match.  This is a message to replace.
             *replace_index = i;
             break; // done with for
          }

        }  // else data read ok
      } // for
      break;

    case WMS_MEMORY_STORE_NV_GW:
      max_msg_slots = cfg_s_ptr->nv_gw_max_slots;

      if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
      {
        max_msg_slots /= 2;
      }

      for ( msg_index = 0; msg_index < max_msg_slots; msg_index++)
      {

        // Map the message slot (relative to as_id) to the array index
        cmd_err =  wms_msg_map_gw_nv_index(as_id, msg_index, &i);

        if (WMS_CMD_ERR_NONE != cmd_err)
        {
          wms_check_valid_message_index(msg_index);
          break; // done with for
        }

        if (cfg_s_ptr->nv_gw_tags[i] == WMS_TAG_NONE)
        {
          //MSG_ERROR_1("Empty  message %d", msg_index);
          continue;  // skip empties
        }

        if ( wms_nv_read_gw_sms( i, sim_data ) == FALSE )
        {
          cmd_err = WMS_CMD_ERR_MSG_NV_READ;
          MSG_ERROR_1("Unreadable message %d", msg_index);
          break;  // Error, done with for
        }
        else
        {
          sim_sms.status = sim_data[0];
          sim_sms.len = (uint8) MIN(appInfo_ptr->sms_rec_len-1, WMS_SIM_SMS_MAX-1);
          (void)memscpy( (void *)sim_sms.data,
                         sim_sms.len,
                         sim_data + 1,
                         sim_sms.len );

          (void)wms_ts_unpack_sim_sms( &sim_sms,
                                       &sim_sms_unpacked );

          if (wms_ts_decode(&sim_sms_unpacked.raw_ts_data, local_client_ts_data) != WMS_OK_S)
          {
            cmd_err = WMS_CMD_ERR_MSG_DECODE;
            MSG_ERROR_3( "Error decoding message: storage: %d, index: %d, as_id: %d",
                         mem_store,
                         i,
                         as_id );
            break;  // Error, done with for
          }

          if (wms_msg_message_should_replace_stored(pid, address, local_client_ts_data) )
          {
             // The message in the store is a deliver message and both the pid
             // and the origination address match.  This is a message to replace.
             *replace_index = msg_index;
             break; // done with for
          }

        } // else data read successful
      } // for
      break;

    default:
      MSG_ERROR_1("Unhandled memory store %d", mem_store);
      break;
  } /* switch mem_store */

  wms_mem_free(local_client_ts_data);

  MSG_HIGH_2( "wms_msg_search_mem_store_for_replacement() returns %d, replace_index=%d",
              cmd_err,
              *replace_index );

  return cmd_err;
} /* wms_msg_search_mem_store_for_replacement() */

/*=========================================================================
FUNCTION
  wms_msg_pid_is_replace_type

DESCRIPTION
  Determines whether a TP-PID value indicates a replace-SMS.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if the pid is a replace type.
  FALSE - otherwise.

SIDE EFFECTS

=========================================================================*/
static boolean wms_msg_pid_is_replace_type(
   wms_pid_e_type pid
)
{
   /* Replace SMS PID values are in the range: 0x41-0x47
   */
   if ( pid >= WMS_PID_REPLACE_SM_1 &&
        pid <= WMS_PID_REPLACE_SM_7)
   {
      return TRUE;
   }

   return FALSE;
}

/*=========================================================================
FUNCTION
  wms_msg_replace_sms_message

DESCRIPTION
  Search for an message with a TP-PID and originating address specified
  by 'pid' and 'address'.  If found, replace the contents of that message
  with the contents of the message pointed to by 'msg_ptr'.

DEPENDENCIES
  None

RETURN VALUE
  WMS_CMD_ERR_NONE:  The search was successful and the message was replaced.

  Otherwise, the return value is one of the following AND the message
  was NOT replaced:

  WMS_CMD_ERR_BAD_PARM: If message_mode not WMS_MESSAGE_MODE_GW.
  WMS_CMD_ERR_MSG_EMPTY_MESSAGE: If message to replace not found.
  Unsuccessful return of wms_msg_search_mem_store_for_replacement()
  Unsuccessful return value of wms_msg_do_write()

SIDE EFFECTS

=========================================================================*/
static wms_cmd_err_e_type wms_msg_replace_sms_message(
  wms_prov_app_info_s_type   *appInfo_ptr,
  wms_pid_e_type              pid,
  wms_address_s_type         *address,
  wms_client_message_s_type  *msg_ptr
)
{
  wms_cmd_err_e_type         cmd_err = WMS_CMD_ERR_BAD_PARM;
  wms_message_index_type     replace_index = WMS_DUMMY_MESSAGE_INDEX;

  if ( (NULL != address) &&
       (NULL != msg_ptr) &&
       (msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_GW) )
  {
    // Search
    cmd_err = wms_msg_search_mem_store_for_replacement( appInfo_ptr,
                                                        msg_ptr->msg_hdr.mem_store,
                                                        pid,
                                                        address,
                                                        &replace_index );

    if (WMS_CMD_ERR_NONE == cmd_err)
    {
      if (WMS_DUMMY_MESSAGE_INDEX != replace_index)
      {
         // Found a message to replace
         msg_ptr->msg_hdr.index = replace_index;
         msg_ptr->msg_hdr.tag = WMS_TAG_MT_NOT_READ;

         cmd_err = wms_msg_do_write( appInfo_ptr, WMS_WRITE_MODE_REPLACE, msg_ptr, NULL );
      }
      else
      {
         // No message found to replace
         cmd_err = WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
      }
    }
  } // message mode

  MSG_HIGH_1("wms_msg_replace_sms_message() returns %d", cmd_err);

  return cmd_err;
} /* wms_msg_replace_sms_message() */
/* REPLACE_SMS - */

/*=========================================================================
FUNCTION wms_msg_sm_data_ind_proc()                              (INTERNAL)

DESCRIPTION
  This function processes the Mobile terminated messages from lower layers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

=========================================================================*/
void wms_msg_sm_data_ind_proc(
  sys_modem_as_id_e_type    as_id,
  wms_cm_conn_id_type       uConnectionId,
  wms_transaction_id_type   transaction_id,
  cm_called_party_bcd_no_T *pSCA, /* originating service center address */
  uint8                    *pubTPDU,
  int                       nTPDULen,
  nas_msg_transport_type_e_type nas_transport_type
)
{
  wms_client_message_s_type     *msg_ptr;
  wms_cmd_err_e_type             cmd_err                = WMS_CMD_ERR_NONE;
  wms_routing_s_type             routing;
  wms_status_e_type              st                     = WMS_OK_S;
  uint32                         i                      = 0;
  uint32                         length                 = 0;
  wms_gw_alphabet_e_type         alphabet               = WMS_GW_ALPHABET_7_BIT_DEFAULT;
  wms_client_type_e_type         parsed_client          = WMS_CLIENT_TYPE_MAX;
  boolean                        deliver_to_all_clients = TRUE;
  wms_message_index_type         rec_id                 = WMS_DUMMY_MESSAGE_INDEX;
  wms_message_index_type         smsr_index;
  wms_write_mode_e_type          write_mode             = WMS_WRITE_MODE_INSERT;
  wms_msg_transport_type_e_type transport_type          = WMS_MSG_TRANSPORT_TYPE_MAX;

  wms_evt_mt_sms_notify_s_type   evt;
  wms_prov_app_info_s_type      *appInfo_ptr;
  wms_msg_transport_s_type      *transport_ptr          = wms_msg_choose_registered_transport( as_id );

  appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  if (!asid_is_valid(appInfo_ptr->asid))
  {
    MSG_ERROR_0("Invalid as_id in appInfo_ptr");
    return;
  }

  if (TRUE == appInfo_ptr->mmgsdi_is_busy)
  {
    MSG_ERROR_0("MT failed, SIM is busy");
    (void) wms_msg_do_internal_ack( appInfo_ptr->asid,
                                    WMS_MT_MSG_FAILED_S,
                                    WMS_TP_CAUSE_UNSPECIFIED_ERROR,
                                    WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED,
                                    uConnectionId );
    return;
  }

  raw_ts.format       = WMS_FORMAT_GW_PP;
  raw_ts.len          = (uint32)nTPDULen;

  if ( (FALSE == pSCA->present) || (0 == pSCA->length) )
  {
    (void)wms_msg_do_internal_ack( appInfo_ptr->asid,
                                   WMS_MT_MSG_FAILED_S,
                                   WMS_TP_CAUSE_UNSPECIFIED_ERROR,
                                   WMS_RP_CAUSE_INVALID_MANDATORY_INFORMATION,
                                   uConnectionId );
    MSG_ERROR_0("MT msg SC Address not present!");
    return;
  }

  (void)memscpy( raw_ts.data,
                 raw_ts.len,
                 pubTPDU,
                 (unsigned int)nTPDULen);

  raw_ts.tpdu_type = wms_get_tpdu_type(raw_ts.data);

  /* Check whether message is good or not
  */
  st = wms_ts_decode ( &raw_ts, &wms_msg_Gclient_ts );

  if( st  != WMS_OK_S )
  {
    MSG_ERROR_0("Msg decoding fails, Bad MT SMS");
    (void)wms_msg_do_internal_ack( appInfo_ptr->asid,
                                   st,
                                   WMS_TP_CAUSE_TPDU_NOT_SUPPORTED,
                                   WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED,
                                   uConnectionId );
    return;
  }

  // Drop non-admin MT SMS when IMS is in limited service
  if ( (transport_ptr != NULL) &&
       (transport_ptr->is_mn_registered) &&
       (WMS_MSG_TRANSPORT_NW_REG_STATUS_LIMITED_SRV == transport_ptr->nw_reg_status) &&
       (!wms_is_gw_admin_msg(&raw_ts)) )
  {
      MSG_HIGH_0("Drop non-admin MT SMS");

      /* Ack the message with success and discard the message */
      (void) wms_msg_do_internal_ack( appInfo_ptr->asid,
                                      WMS_OK_S,
                                      (wms_tp_cause_e_type)0,
                                      (wms_rp_cause_e_type)0,
                                      uConnectionId );
      return;
  }

  /* Message is good. Now route the message.
  */

  msg_ptr = & msg_event_info.mt_message_info.message;

  if ( (raw_ts.tpdu_type == WMS_TPDU_DELIVER) ||
       (raw_ts.tpdu_type == WMS_TPDU_RESERVED) )
  {
    routing = wms_cfg_determine_pp_route(appInfo_ptr, &wms_msg_Gclient_ts.u.gw_pp.u.deliver);

    length = wms_msg_Gclient_ts.u.gw_pp.u.deliver.user_data.sm_len;
    alphabet = wms_msg_Gclient_ts.u.gw_pp.u.deliver.dcs.alphabet;
    msg_ptr->msg_hdr.tag   = WMS_TAG_MT_NOT_READ;
  }
  else if (raw_ts.tpdu_type == WMS_TPDU_STATUS_REPORT)
  {
    wms_cfg_determine_sts_rpt_pp_route( appInfo_ptr,
                                        &wms_msg_Gclient_ts.u.gw_pp.u.status_report,
                                        &routing ,
                                        &rec_id,
                                        &write_mode,
                                        &smsr_index );
    length = wms_msg_Gclient_ts.u.gw_pp.u.status_report.user_data.sm_len;
    alphabet = wms_msg_Gclient_ts.u.gw_pp.u.status_report.dcs.alphabet;
    msg_ptr->msg_hdr.index = smsr_index;
    msg_ptr->msg_hdr.tag = WMS_TAG_STATUS_RPT;

    /* If the MO message found was waiting on the STATUS message,
      we need to update the status of the message in SIM*/
    if( rec_id < WMS_MESSAGE_LIST_MAX &&
      WMS_TAG_MO_SENT_ST_NOT_RECEIVED == appInfo_ptr->sms_rec_tags[rec_id] )
    {
      (void) wms_msg_do_modify_tag( appInfo_ptr,
                                    WMS_MEMORY_STORE_SIM,
                                    rec_id,
                                    WMS_TAG_MO_SENT_ST_NOT_STORED,
                                    NULL );
    }
  }
  else
  {
    MSG_ERROR_1("Bad TPDU received: %d", raw_ts.tpdu_type);
    return; /* SHORT-RETURN */
  }

  MSG_HIGH_3( "Stored Route Type=%d & Mem_Type=%d TPDU Type=%d",
              routing.route,
              routing.mem_store,
              raw_ts.tpdu_type );

  if (NAS_MSG_TRANSPORT_TYPE_GW == nas_transport_type)
  {
    transport_type = WMS_MSG_TRANSPORT_TYPE_CS;
  }
  else if (NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == nas_transport_type)
  {
    transport_type = WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS;
  }

  msg_event_info.mt_message_info.as_id                        = as_id;
  msg_event_info.mt_message_info.route                        = routing.route;
  msg_event_info.mt_message_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
  msg_event_info.mt_message_info.transport_type               = transport_type;

  msg_ptr->msg_hdr.mem_store                                  = routing.mem_store;
  msg_ptr->u.gw_message.is_broadcast                          = FALSE;
  msg_ptr->u.gw_message.raw_ts_data                           = raw_ts;
  wms_ts_convert_from_bcd_address( pSCA, &msg_ptr->u.gw_message.sc_address );

  if( (raw_ts.len + pSCA->length + 1 > WMS_SIM_SMS_MAX ) ||
      ((length > WMS_SMS_UDL_MAX_7_BIT) && (alphabet == WMS_GW_ALPHABET_7_BIT_DEFAULT)) ||
      ((length > WMS_SMS_UDL_MAX_8_BIT) && (alphabet == WMS_GW_ALPHABET_8_BIT))
    )
  {
      (void) wms_msg_do_internal_ack( appInfo_ptr->asid,
                                      WMS_MT_MSG_FAILED_S,
                                      WMS_TP_CAUSE_TPDU_NOT_SUPPORTED,
                                      WMS_RP_CAUSE_INVALID_MANDATORY_INFORMATION,
                                      uConnectionId );

      MSG_ERROR_0("RP_CAUSE: WMS_RP_CAUSE_INVALID_MANDATORY_INFORMATION");
      return;
  }

#ifdef FEATURE_GWSMS_DEPERSO
  if (TRUE == wms_msg_simlock_ota_unlock_proc( appInfo_ptr,
                                               uConnectionId,
                                               &wms_msg_Gclient_ts.u.gw_pp.u.deliver,
                                               transport_type ))
  {
    /* the MT msg is a De-personalization msg,
     * special handling was done already */
    return; /* SHORT-RETURN */
  }
#endif /* FEATURE_GWSMS_DEPERSO */


#ifdef FEATURE_GWSMS_PP_DOWNLOAD
  if (TRUE == wms_msg_pp_download_proc( appInfo_ptr,
                                        uConnectionId,
                                        pSCA,
                                        pubTPDU,
                                        nTPDULen,
                                        &wms_msg_Gclient_ts.u.gw_pp.u.deliver,
                                        transport_type ) )
  {
    /* the MT msg is a PP download msg, special handling was done already */
    return; /* SHORT-RETURN */
  }
#endif /* FEATURE_GWSMS_PP_DOWNLOAD */

  /* check if any client is registered with particular source and dest port */
  parsed_client = wms_cfg_check_appport_reg_client(msg_ptr);

  /* Check if this message is to be delivered to a specific client.*/
  if(parsed_client == WMS_CLIENT_TYPE_MAX)
  {
    parsed_client = wms_msg_parse_msg(msg_ptr);
  }

  if (parsed_client != WMS_CLIENT_TYPE_MAX)
  {
    MSG_HIGH_1("MT SMS: Message Consumed by Client: %d", parsed_client);

    /* This message is not to be shared */
    deliver_to_all_clients                                   = FALSE;
    msg_event_info.mt_message_info.route                     = WMS_ROUTE_TRANSFER_ONLY;
    routing.route                                            = WMS_ROUTE_TRANSFER_ONLY;
    msg_event_info.mt_message_info.message.msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
    routing.mem_store                                        = WMS_MEMORY_STORE_NONE;
  }

  /* set the client id to the mt_message_info */
  msg_event_info.mt_message_info.client_id = parsed_client;

  /* check if there are any MWIS information in the MT SMS */
  if ( (WMS_TPDU_DELIVER  == raw_ts.tpdu_type) ||
       (WMS_TPDU_RESERVED == raw_ts.tpdu_type) )
  {
    wms_cfg_update_mwis_info(appInfo_ptr, &raw_ts);
  }

  /* Handling for WAP Push Messages */
  if ( ( (WMS_TPDU_DELIVER  == raw_ts.tpdu_type) ||
         (WMS_TPDU_RESERVED == raw_ts.tpdu_type) ) &&
       (wms_cfg_check_wap_push_message(msg_ptr)) )
  {
    MSG_HIGH_0("WAP Push Message Detected! Routes Changed");

    msg_event_info.mt_message_info.route                     = WMS_ROUTE_TRANSFER_AND_ACK;
    msg_event_info.mt_message_info.message.msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
    routing.route                                            = WMS_ROUTE_TRANSFER_AND_ACK;
    routing.mem_store                                        = WMS_MEMORY_STORE_NONE;

#ifdef FEATURE_SMS_WAP_PUSH_TRASFER_ONLY
    msg_event_info.mt_message_info.route                     = WMS_ROUTE_TRANSFER_ONLY;
    routing.route                                            = WMS_ROUTE_TRANSFER_ONLY;
#endif /* FEATURE_SMS_WAP_PUSH_TRASFER_ONLY */
  }

  /* Client is able to manage MT SMS storage in client memory for routes
     TRANSFER_AND_ACK/TRANSFER_ONLY. Client is not allowed to handle SIM memory
     storage (3GPP spec TS 23.038 mandates Class 2 SMS to be stored in SIM).
     Handling for NV Memory should be replaced by Client Memory for Primary Client
     willing to use its memory */
  if ((WMS_ROUTE_STORE_AND_NOTIFY == routing.route) &&
      (WMS_MEMORY_STORE_NV_GW == routing.mem_store) &&
      (appInfo_ptr->primary_client != (wms_client_id_type)WMS_CLIENT_TYPE_MAX) &&
      appInfo_ptr->use_client_memory)
  {
    deliver_to_all_clients                                   = TRUE;
    parsed_client                                            = WMS_CLIENT_TYPE_MAX;
    msg_event_info.mt_message_info.route                     = WMS_ROUTE_TRANSFER_ONLY;
    msg_event_info.mt_message_info.message.msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
    routing.route                                            = WMS_ROUTE_TRANSFER_ONLY;
    routing.mem_store                                        = WMS_MEMORY_STORE_NONE;
  }

  /* Dont Store Type 0 Messages - Change Routing */
  if ( (WMS_PID_SM_TYPE_0 == wms_msg_Gclient_ts.u.gw_pp.u.deliver.pid) &&
       (WMS_ROUTE_STORE_AND_NOTIFY == routing.route) )
  {
    deliver_to_all_clients                                   = TRUE;
    parsed_client                                            = WMS_CLIENT_TYPE_MAX;
    msg_event_info.mt_message_info.route                     = WMS_ROUTE_TRANSFER_AND_ACK;
    msg_event_info.mt_message_info.message.msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
    routing.route                                            = WMS_ROUTE_TRANSFER_AND_ACK;
    routing.mem_store                                        = WMS_MEMORY_STORE_NONE;
  }

  MSG_HIGH_2( "Msg Route = %d Mem Store type=%d",
              routing.route,
              routing.mem_store );

  switch( routing.route )
  {
    case WMS_ROUTE_STORE_AND_NOTIFY:
      /* Store message and notify clients */
      msg_event_info.mt_message_info.transaction_id = transaction_id;

      if ( (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_DELIVER) ||
           (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_RESERVED) )
      {
        /* REPLACE_SMS + */
        if ( wms_msg_pid_is_replace_type(wms_msg_Gclient_ts.u.gw_pp.u.deliver.pid) &&
             WMS_CMD_ERR_NONE == wms_msg_replace_sms_message( appInfo_ptr,
                                                              wms_msg_Gclient_ts.u.gw_pp.u.deliver.pid,
                                                              &wms_msg_Gclient_ts.u.gw_pp.u.deliver.address,
                                                              msg_ptr ) )
        {
           // A message was received with replace flagged, a search for
           // an existing message with this replace id and origination
           // address was successful, and the message was replaced.
           // If the replacement failed for any reason, this point is not
           // reached and cmd_err has been filled with the error code.
           MSG_HIGH_2( "ReplaceSMS replaced message with pid 0x%x in store %d ",
                       wms_msg_Gclient_ts.u.gw_pp.u.deliver.pid,
                       msg_ptr->msg_hdr.mem_store );

           cmd_err = WMS_CMD_ERR_NONE;
        }
        else
        /* REPLACE_SMS - */
        {
          /* check to see if it is a CPHS message and if line indicator is 2*/
          if (wms_cfg_check_cphs_msg(&wms_msg_Gclient_ts.u.gw_pp.u.deliver.address))
          {
            if (wms_msg_Gclient_ts.u.gw_pp.u.deliver.address.digits[1] == 0x00)
            {
              msg_ptr->msg_hdr.mem_store = wms_sim_mmgsdi_get_supproted_mem_store(appInfo_ptr->mmgsdi_appInfo.app_type);

              if (appInfo_ptr->voice_mail_index != (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX)
              {
                /* Replace a previous voice mail notification in SIM */
                msg_ptr->msg_hdr.index = appInfo_ptr->voice_mail_index;
                cmd_err = wms_msg_do_write( appInfo_ptr, WMS_WRITE_MODE_REPLACE, msg_ptr, NULL );
              }
              else
              {
                /* Insert in SIM */
                cmd_err = wms_msg_do_write( appInfo_ptr, WMS_WRITE_MODE_INSERT, msg_ptr, NULL );
                if (cmd_err == WMS_CMD_ERR_NONE)
                {
                  appInfo_ptr->voice_mail_index = msg_ptr->msg_hdr.index;
                }
              }
              /* Always True for CPHS Message */
              appInfo_ptr->voice_mail_active = TRUE;
            }
#ifdef FEATURE_ALS
            else
            {
              if (wms_msg_Gclient_ts.u.gw_pp.u.deliver.address.digits[1] == 0x01)
              {
                if (cfg_s_ptr->sim_voice_mail_index_line2 != (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX)
                {
                  /* Replace a previous voice mail notification in SIM */
                  msg_ptr->msg_hdr.index = cfg_s_ptr->sim_voice_mail_index_line2;
                  msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_SIM;
                  cmd_err = wms_msg_do_write( appInfo_ptr, WMS_WRITE_MODE_REPLACE, msg_ptr, NULL );
                }
                else
                {
                  /* Insert in SIM */
                  msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_SIM;
                  cmd_err = wms_msg_do_write( appInfo_ptr, WMS_WRITE_MODE_INSERT, msg_ptr, NULL );
                  if (cmd_err == WMS_CMD_ERR_NONE)
                  {
                    cfg_s_ptr->sim_voice_mail_index_line2 = msg_ptr->msg_hdr.index;
                  }
                }
                /* Always True for CPHS Message */
                cfg_s_ptr->sim_voice_mail_active_line2 = TRUE;
              }
            }
#endif /* FEATURE_ALS */
          }
          else
          {
            if ((appInfo_ptr->voice_mail_index != (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX)
                &&(wms_cfg_check_voicemail_contents( WMS_MESSAGE_MODE_GW, &raw_ts, &appInfo_ptr->voice_mail_active, (uint8*)&i)))
            {
              /* Replace a previous voice mail notification in SIM */
              msg_ptr->msg_hdr.index = appInfo_ptr->voice_mail_index;
              msg_ptr->msg_hdr.mem_store = wms_sim_mmgsdi_get_supproted_mem_store(appInfo_ptr->mmgsdi_appInfo.app_type);
              cmd_err = wms_msg_do_write( appInfo_ptr, WMS_WRITE_MODE_REPLACE, msg_ptr, NULL );
            }
            else
            {
              /* Insert the Message in the Memory Store determined by the Routes */
              cmd_err = wms_msg_do_write( appInfo_ptr, WMS_WRITE_MODE_INSERT, msg_ptr, NULL );

              //  FEATURE_SMS_MT_STORE_TO_SIM_IF_NV_FULL
              if (cfg_s_ptr->store_to_sim_if_nv_full)
              {
                /* Store Message to SIM Memory if Phone Memory is Full */
                if ( (cmd_err == WMS_CMD_ERR_MSG_NV_FULL) &&
                     (msg_ptr->msg_hdr.mem_store == WMS_MEMORY_STORE_NV_GW) )
                {
                  msg_ptr->msg_hdr.mem_store = wms_sim_mmgsdi_get_supproted_mem_store(appInfo_ptr->mmgsdi_appInfo.app_type);
                  cmd_err = wms_msg_do_write( appInfo_ptr, WMS_WRITE_MODE_INSERT, msg_ptr, NULL );
                }
                else if ( (cmd_err == WMS_CMD_ERR_MSG_SIM_FULL) &&
                          (msg_ptr->msg_hdr.mem_store == WMS_MEMORY_STORE_SIM ||
                           msg_ptr->msg_hdr.mem_store == WMS_MEMORY_STORE_USIM) &&
                          (wms_msg_Gclient_ts.u.gw_pp.u.deliver.dcs.msg_class != WMS_MESSAGE_CLASS_2) )
                {
                  msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NV_GW;
                  cmd_err = wms_msg_do_write( appInfo_ptr, WMS_WRITE_MODE_INSERT, msg_ptr, NULL );
                }
              }
            }
          }
        }

      }
      else /*STATUS REPORT*/
      {
        if( rec_id < WMS_MESSAGE_LIST_MAX )
        {
          /*Write The Status Report*/
          cmd_err = wms_msg_do_write_sts_report( appInfo_ptr, write_mode, rec_id , msg_ptr, NULL );
          if( WMS_CMD_ERR_NONE == cmd_err &&
              WMS_WRITE_MODE_INSERT == write_mode &&
              WMS_TAG_MO_SENT_ST_NOT_STORED == appInfo_ptr->sms_rec_tags[rec_id] )
          {
            (void) wms_msg_do_modify_tag( appInfo_ptr,
                                          WMS_MEMORY_STORE_SIM,
                                          rec_id,
                                          WMS_TAG_MO_SENT_ST_STORED,
                                          NULL );
          }
        }
        else
        {
          cmd_err = WMS_CMD_ERR_MSG_INDEX ;
        }
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        /* Ack with success */
        (void) wms_msg_do_internal_ack( as_id,
                                        WMS_OK_S,
                                        (wms_tp_cause_e_type)0,
                                        (wms_rp_cause_e_type)0,
                                        uConnectionId );

        /* Send the event to clients
        */
        if ( (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_DELIVER) ||
             (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_RESERVED) )
        {
          (void)wms_msg_event_notify( WMS_MSG_EVENT_RECEIVED_MESSAGE,
                                & msg_event_info );
        }
        else if (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_STATUS_REPORT)
        {
          (void)wms_msg_event_notify( WMS_MSG_EVENT_STATUS_REPORT,
                                  & msg_event_info );
        }
      }
      else if ( (cmd_err == WMS_CMD_ERR_MSG_MEMORY_FULL)
                || ( (wms_msg_nv_gw_memory_full(appInfo_ptr))
                     && ( (cmd_err == WMS_CMD_ERR_MSG_SIM_WRITE)
                          || (cmd_err == WMS_CMD_ERR_MSG_SIM_FULL) ) ) )
      {
         if ( WMS_GW_MSG_WAITING_STORE == wms_msg_Gclient_ts.u.gw_pp.u.deliver.dcs.msg_waiting )
         {
            /* Overwrite default setting to allow upper layer to process */
            msg_event_info.mt_message_info.route = WMS_ROUTE_TRANSFER_AND_ACK;
            msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
            msg_ptr->msg_hdr.index = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
            if (appInfo_ptr->sim_init_complete)
            {
               (void)wms_msg_event_notify( WMS_MSG_EVENT_RECEIVED_MESSAGE,
                                          &msg_event_info );
            }
         }
         /* Ack with Memory Exceeded Error */
        (void) wms_msg_do_internal_ack( as_id,
                                        WMS_MT_MSG_FAILED_S,
                                        WMS_TP_CAUSE_MEMORY_FULL,
                                        WMS_RP_CAUSE_MEMORY_EXCEEDED,
                                        uConnectionId );

         // If the MS notifies the network that the MS has been unable to accept a 
         // short message because its memory capacity has been exceeded, 
         // then the ME shall set the memory capacity Exceeded Notification flag if present
         appInfo_ptr->smss_mem_full_flag &= (uint8)WMS_SIM_MEM_FULL_SET;
         (void)wms_cfg_get_sim_next_message_number( appInfo_ptr );

         MSG_ERROR_0( "WMS acks with WMS_RP_CAUSE_MEMORY_EXCEEDED" );
      }
      else
      {
         if (( WMS_GW_MSG_WAITING_STORE == wms_msg_Gclient_ts.u.gw_pp.u.deliver.dcs.msg_waiting ) &&
             ( ( WMS_CMD_ERR_MSG_SIM_WRITE == cmd_err ) ||
               ( WMS_CMD_ERR_MSG_SIM_FULL == cmd_err )  ||
               ( WMS_CMD_ERR_BUSY == cmd_err )  ) )
         {
             /* Overwrite default setting to allow upper layer to process */
            msg_event_info.mt_message_info.route = WMS_ROUTE_TRANSFER_AND_ACK;
            msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
            msg_ptr->msg_hdr.index = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
            if (appInfo_ptr->sim_init_complete)
            {
              (void)wms_msg_event_notify( WMS_MSG_EVENT_RECEIVED_MESSAGE,
                                          &msg_event_info );
            }
         }

        (void) wms_msg_do_internal_ack( as_id,
                                        WMS_MT_MSG_FAILED_S,
                                        WMS_TP_CAUSE_UNSPECIFIED_ERROR,
                                        WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED,
                                        uConnectionId );

         MSG_ERROR_0("WMS acks with WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED");
      }
      break;

    case WMS_ROUTE_TRANSFER_ONLY:
      if( wms_msg_get_free_mt_record( & i ) == FALSE )
      {
        /* Ack with error */
        (void) wms_msg_do_internal_ack( as_id,
                                        WMS_MT_MSG_FAILED_S,
                                        WMS_TP_CAUSE_ERROR_IN_MS,
                                        WMS_RP_CAUSE_TEMPORARY_FAILURE,
                                        uConnectionId );

         MSG_ERROR_0("WMS acks with WMS_RP_CAUSE_TEMPORARY_FAILURE");
      }
      else
      {
        msg_event_info.mt_message_info.transaction_id
                          = wms_msg_get_next_tid();

        /* Remember information about the MT message
        */
        msg_s_ptr->gw_mt_rec[i]->as_id          = as_id;
        msg_s_ptr->gw_mt_rec[i]->appInfo_ptr    = appInfo_ptr;
        msg_s_ptr->gw_mt_rec[i]->tid            = msg_event_info.mt_message_info.transaction_id;
        msg_s_ptr->gw_mt_rec[i]->smi            = uConnectionId;
        msg_s_ptr->gw_mt_rec[i]->msg_hdr        = msg_ptr->msg_hdr;
        msg_s_ptr->gw_mt_rec[i]->client_id      = (wms_client_id_type)parsed_client;
        msg_s_ptr->gw_mt_rec[i]->state          = WMS_MSG_STATE_WAIT_FOR_ACK;
        msg_s_ptr->gw_mt_rec[i]->transport_type = transport_type;

        /* Send the event to clients
        */
        if ( (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_DELIVER) ||
             (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_RESERVED) )
        {
          if(deliver_to_all_clients)
          {
            (void)wms_msg_event_notify( WMS_MSG_EVENT_RECEIVED_MESSAGE,
                                  & msg_event_info );
          }
          else
          {
            wms_msg_event_notify_single_client( parsed_client,
                                          WMS_MSG_EVENT_RECEIVED_MESSAGE,
                                          & msg_event_info );
          }
        }
        else if (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_STATUS_REPORT)
        {
          (void)wms_msg_event_notify( WMS_MSG_EVENT_STATUS_REPORT,
                                  & msg_event_info );
        }
      }
      msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
      msg_ptr->msg_hdr.index     = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
      break;


    case WMS_ROUTE_TRANSFER_AND_ACK:
      /* Send the message to clients and ack it */
      msg_event_info.mt_message_info.transaction_id
                = (uint32) WMS_DUMMY_TRANSACTION_ID;

      /* Send the event to clients
      */
      if ( (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_DELIVER) ||
           (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_RESERVED) )
      {
        if(deliver_to_all_clients)
        {
          (void)wms_msg_event_notify( WMS_MSG_EVENT_RECEIVED_MESSAGE,
                                      & msg_event_info );
        }
        else
        {
          wms_msg_event_notify_single_client( parsed_client,
                                              WMS_MSG_EVENT_RECEIVED_MESSAGE,
                                              & msg_event_info );
        }
      }
      else if (wms_msg_Gclient_ts.u.gw_pp.tpdu_type == WMS_TPDU_STATUS_REPORT)
      {
        (void)wms_msg_event_notify( WMS_MSG_EVENT_STATUS_REPORT,
                               & msg_event_info );
      }
      (void) wms_msg_do_internal_ack( as_id,
                                      WMS_OK_S,
                                      (wms_tp_cause_e_type)0,
                                      (wms_rp_cause_e_type)0,
                                      uConnectionId );
      msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
      msg_ptr->msg_hdr.index     = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
      break;


    case WMS_ROUTE_DISCARD: /* Discard message */
    case WMS_ROUTE_NO_CHANGE:    /* unspecified */
    default:
      /* Ack the message with success and discard the message */
      (void) wms_msg_do_internal_ack( as_id,
                                      WMS_OK_S,
                                      (wms_tp_cause_e_type)0,
                                      (wms_rp_cause_e_type)0,
                                      uConnectionId );

      MSG_HIGH_0("Message Discarded");
      msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
      msg_ptr->msg_hdr.index     = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;

      break;
  }

  /* Report events to Diag */
  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    evt.mem_type      = (uint8)msg_ptr->msg_hdr.mem_store;
    evt.msg_index     = msg_ptr->msg_hdr.index;
    event_report_payload( EVENT_MT_SMS_NOTIFY, sizeof(evt), (void*) & evt );
  }

  /* done
  */
  return;

} /* wms_msg_sm_data_ind_proc() */

/*=========================================================================
FUNCTION
  wms_msg_mt_error_ind_proc

DESCRIPTION
  This function handles the MT Message Errors received from the lower
  layers and publishes WMS_MSG_EVENT_MT_MESSAGE_ERROR to clients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_mt_error_ind_proc(
   sys_modem_as_id_e_type     as_id,          // The as_id is from lower layer, OK as parameter
   wms_cm_conn_id_type        smi_mt,
   wms_report_status_e_type   error_status,
   uint8                      cause_value
)
{
  uint32    rec_idx;

  if (FALSE == wms_msg_map_mt_record_from_smi(as_id, smi_mt, &rec_idx))
  {
     MSG_ERROR_1("Bad MT err rpt: %ld", smi_mt);
     return;
  }

  msg_event_info.mt_message_error.as_id          = as_id;
  msg_event_info.mt_message_error.transaction_id = msg_s_ptr->gw_mt_rec[rec_idx]->tid;
  msg_event_info.mt_message_error.report_status  = error_status;

  MSG_HIGH_1("MT SMS Failure Cause Value = %d", cause_value);

  (void)wms_msg_event_notify( WMS_MSG_EVENT_MT_MESSAGE_ERROR, &msg_event_info );

  if (WMS_MSG_STATE_LAYER2_IN_PROGRESS == msg_s_ptr->gw_mt_rec[rec_idx]->state)
  {
    // Ack request has been sent to NAS by WMS but not yet processed.
    // NAS will ignore the ack request received after the MT error indication has been notified 
    // and will not send Ack report indication since NAS will be in idle state.

    /* Notify ACK REPORT event to clients */
    wms_msg_ack_report_ind_proc(as_id, smi_mt, error_status);	
  }

  /* Found the matching MT record.
  ** Clear the record and send event to clients.
  */
  wms_msg_release_mt_record(rec_idx);

  return;
} /* wms_msg_mt_error_ind_proc() */

/*=========================================================================
FUNCTION
  wms_msg_ack_report_ind_proc

DESCRIPTION
  This function handles the Ack Reports received from the lower
  layers and publishes WMS_MSG_EVENT_ACK_REPORT to clients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_ack_report_ind_proc(
   sys_modem_as_id_e_type     asid,          // The asid is from lower layer, OK as parameter
   wms_cm_conn_id_type        smi_mt,
   wms_report_status_e_type   error_status
)
{
  uint32    rec_idx;

  if (FALSE == wms_msg_map_mt_record_from_smi(asid, smi_mt, &rec_idx))
  {
     MSG_HIGH_1("Bad MT err rpt: %ld", smi_mt);
     return;
  }

  if (WMS_CLIENT_TYPE_INTERNAL != msg_s_ptr->gw_mt_rec[rec_idx]->ack_client_id)
  {
    msg_event_info.ack_report_info.as_id          = asid;
    msg_event_info.ack_report_info.client_id      = msg_s_ptr->gw_mt_rec[rec_idx]->ack_client_id;
    msg_event_info.ack_report_info.message_mode   = WMS_MESSAGE_MODE_GW;
    msg_event_info.ack_report_info.user_data      = msg_s_ptr->gw_mt_rec[rec_idx]->user_data;
    msg_event_info.ack_report_info.transaction_id = msg_s_ptr->gw_mt_rec[rec_idx]->tid;

    if (WMS_RPT_OK == error_status)
    {
      MSG_HIGH_0("MT SMS Ack Success");
      msg_event_info.ack_report_info.is_success = TRUE;
    }
    else
    {
      MSG_HIGH_1("MT SMS Ack Error Status = %d", error_status);
      msg_event_info.ack_report_info.is_success = FALSE;
    }
    msg_event_info.ack_report_info.error_status = error_status;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_ACK_REPORT, &msg_event_info );
  }

  /* Found the matching MT record.
  ** Clear the record and send event to clients.
  */
  wms_msg_release_mt_record(rec_idx);

  return;
} /* wms_msg_ack_report_ind_proc() */

/*=========================================================================
FUNCTION
  wms_msg_mo_network_no_support

DESCRIPTION
  This function decides whether the error from NAS indicates the CS/PS domain
  tried for MO SMS is not supported by the network.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
static __inline
boolean wms_msg_mo_network_no_support
(
  wms_report_status_e_type error_status,
  uint8                    cause_value,
  wms_domain_e_type        domain
)
{
  MSG_HIGH_1("NAS error status: %d", error_status);

  switch( error_status )
  {
    case WMS_RPT_NO_RESPONSE_FROM_NETWORK:
      MSG_HIGH_0("No Reponse from network");
      if( WMS_DOMAIN_PS == domain )
      {
        MSG_HIGH_0("No Response from Network for PS Domain");
        return TRUE;
      }
      else
      {
        return FALSE;
      }

    case WMS_RPT_RP_ERROR:
    case WMS_RPT_CP_ERROR :
      MSG_HIGH_1("rp_cause is %d", cause_value);
      switch(cause_value)
      {
        case WMS_RP_CAUSE_FACILITY_REJECTED:
        case WMS_RP_CAUSE_NETWORK_OUT_OF_ORDER:
        case WMS_RP_CAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED:
        case WMS_RP_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED:
        case WMS_RP_CAUSE_INTERWORKING_UNSPECIFIED:
        case WMS_RP_CAUSE_MESSAGE_TYPE_NON_EXISTENT:
        case WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED:
          return TRUE;

        default:
          return FALSE;
      }

    default:
      return FALSE;
  }

} /* wms_msg_mo_network_no_support() */

/*=========================================================================
FUNCTION
  wms_msg_enable_tp_rd

DESCRIPTION
  This function set reject duplicate bit in the raw data to 1 for retry message

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_enable_tp_rd
(
  wms_raw_ts_data_s_type* raw_ts_ptr
)
{
  boolean st = TRUE;

  if (raw_ts_ptr == NULL)
  {
    MSG_ERROR_0("Null record pointer!");
    return FALSE;
  }

  if (WMS_OK_S != wms_ts_decode( raw_ts_ptr, & wms_msg_Gclient_ts ))
  {
    st = FALSE;
  }
  else
  {
    wms_msg_Gclient_ts.u.gw_pp.u.submit.reject_duplicates = TRUE;
    if (WMS_OK_S != wms_ts_encode(&wms_msg_Gclient_ts, raw_ts_ptr))
    {
      st = FALSE;
    }
  }

  MSG_HIGH_1("wms_msg_enable_tp_rd() returns %d", st);

  return st;
}

/*=========================================================================
FUNCTION
  wms_msg_sm_report_ind_proc

DESCRIPTION
  This function process the result from NAS for a MO SMS request. It will also
  perform domain selection and retry based on various error conditions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Submit Report may be sent to the clients, and domain preference can be
  changed.

=========================================================================*/
void wms_msg_sm_report_ind_proc
(
   sys_modem_as_id_e_type   asid,
   uint16                   smi,
   wms_report_status_e_type error_status,
   uint8                    cause_value,
   uint8*                   puData,
   uint32                   uLen,
   boolean                  diagnostics_present,
   uint8                    diagnostics,
   boolean                  is_permanent_error,
   boolean                  retry_header_present,
   uint32                   retry_header_value,
   wms_domain_e_type        domain,
   uint16                   ims_cause_code
)
{
  uint32                     rec_idx;
  boolean                    gcf_flag    = FALSE;
  boolean                    ok_to_retry = TRUE; /* By default we allow retry if it is enabled */
  uint32    retry_interval;
  wms_prov_app_info_s_type  *appInfo_ptr;

  appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(asid, WMS_MESSAGE_MODE_GW);

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  /* Find the matching record
  */
  if (FALSE == wms_msg_map_mo_record(asid, smi, &rec_idx) )
  {
    /* Unrecognized report, discard. */
    MSG_ERROR_1("Unrecognized rpt ind: SMI %d not found", smi);
    return;
  }

  MSG_HIGH_2( "Received Report for SMI = %d Record %d",
              smi,
              rec_idx );

  memset(&msg_event_info, 0, sizeof(wms_msg_event_info_s_type));

  /* TPDU */

  if (WMS_RPT_OK == error_status)
  { /* Successful submit report */
    // Default Values
    raw_ts.format     = WMS_FORMAT_GW_PP;
    raw_ts.tpdu_type  = WMS_TPDU_SUBMIT_REPORT_ACK;
    raw_ts.len        = MIN(uLen, WMS_MAX_LEN);

    memset( raw_ts.data, 0, WMS_MAX_LEN);  // TODO : Is this necessary?

    (void)memscpy( raw_ts.data,
                   WMS_MAX_LEN,
                   puData,
                   uLen);

    if (raw_ts.len == 0)
    {
      // If the tpdu is empty, change the TPDU type to NONE
      msg_event_info.submit_report_info.tpdu_type = WMS_TPDU_NONE;
    }
    else if (WMS_OK_S != wms_ts_decode_submit_report_ack( &raw_ts,
                                                          &msg_event_info.submit_report_info.u.ack,
                                                          NULL) )
    {
      MSG_ERROR_0("Bad submit report");
      return;
    }
  }
  else
  { /* failed submit report */
    // Error
    raw_ts.format     = WMS_FORMAT_GW_PP;
    raw_ts.tpdu_type  = WMS_TPDU_SUBMIT_REPORT_ERROR;
    raw_ts.len        = MIN(uLen, WMS_MAX_LEN);
    memset( raw_ts.data, 0, WMS_MAX_LEN);  // TODO : Is this necessary?
    (void)memscpy( raw_ts.data,
                   WMS_MAX_LEN,
                   puData,
                   uLen);

    if (raw_ts.len == 0)
    {
      msg_event_info.submit_report_info.tpdu_type = WMS_TPDU_NONE;
    }
    else if (WMS_OK_S != wms_ts_decode_submit_report_error( &raw_ts,
                                                            &msg_event_info.submit_report_info.u.error,
                                                            NULL) )
    {
      MSG_ERROR_0("Bad submit report");
      return;
    }
    /* Handling the case where the first attempt of sending MO SMS has reached the Service Center, but, due to
       loss of service, lower layers indicated temporary error. SMS was retried again, but Service Center would
       reject this SMS as duplicate.
     
       Since the clients are not aware of WMS retries and SMS has reached Service Center, this transaction should
       be marked as SUCCESS. */
    else if ( (msg_s_ptr->gw_mo_rec[rec_idx]->retry_count > 0) && 
              (WMS_TP_CAUSE_REJECTED_DUPLICATE_SM == msg_event_info.submit_report_info.u.error.tp_cause) )
    {
      MSG_HIGH_2("Received tp_cause %d at retry attempt %d",
                 msg_event_info.submit_report_info.u.error.tp_cause,
                 msg_s_ptr->gw_mo_rec[rec_idx]->retry_count);
      error_status = WMS_RPT_OK;
    }
  }

  /* Fill in event information
  */
  msg_event_info.submit_report_info.user_data                      = msg_s_ptr->gw_mo_rec[rec_idx]->user_data;
  msg_event_info.submit_report_info.client_id                      = msg_s_ptr->gw_mo_rec[rec_idx]->client_id;
  msg_event_info.submit_report_info.as_id                          = asid;

  /* Cause info
  */
  msg_event_info.submit_report_info.report_status                  = error_status;

  if (msg_event_info.submit_report_info.report_status != WMS_RPT_OK)
  {
    MSG_HIGH_1("MO SMS Failure Cause Value = %d", cause_value);
  }

  msg_event_info.submit_report_info.cause_info.cause_value         = (wms_rp_cause_e_type)cause_value;
  msg_event_info.submit_report_info.cause_info.diagnostics_present = diagnostics_present;
  msg_event_info.submit_report_info.cause_info.diagnostics         = diagnostics;
  msg_event_info.submit_report_info.alpha_id.len                   = msg_s_ptr->gw_mo_rec[rec_idx]->prev_report.alpha_id.len;
  msg_event_info.submit_report_info.alpha_id.data                  = msg_s_ptr->gw_mo_rec[rec_idx]->prev_report.alpha_id.data;
  msg_event_info.submit_report_info.mo_ctrl_status                 = msg_s_ptr->gw_mo_rec[rec_idx]->prev_report.mo_ctrl_status;
  msg_event_info.submit_report_info.message_mode                   = WMS_MESSAGE_MODE_GW;

  /* Code to Check GCF Flag for Retry on CS Pref Mode */
  gcf_flag = FALSE;
  (void)wms_nv_read_gcf_flag(&gcf_flag);

  MSG_HIGH_2( "GCF Flag for Retry = %d,  CS Domain Fallback = %d",
              gcf_flag,
              cfg_s_ptr->cs_domain_fallback_on_lte );

  /* ========= BEGIN: SMS Domain Selection Procedures ============ */
  /* Enhanced Domain Selection Procedure
  */
  if( (TRUE == is_permanent_error) ||
      (wms_msg_rp_error_permanent(error_status, cause_value)) )

  {
    MSG_ERROR_0("Got perm error from NAS, stop retrying");
    ok_to_retry = FALSE;
  }
  else if (TRUE == gcf_flag)
  {
    MSG_ERROR_0("GCF Flag is Set, stop retrying");
    ok_to_retry = FALSE;
  }
  else if ((ims_cause_code == TIMER_F_TIMEOUT)&&
           (reg_mode_is_usim_compliant_for_cm(asid)==TRUE))
  {
    MSG_ERROR_0("TIMER_F_TIMEOUT, stop retrying");
    ok_to_retry = FALSE;
  }
  else if( TRUE == wms_msg_mo_network_no_support( error_status,
                                                  cause_value,
                                                  domain ) )
  {
    /* If error is not related to net error, don't go through this procedure */
    MSG_HIGH_1( "Net doesn't support SMS over domain %d", domain);
    if( WMS_DOMAIN_PS == domain )
    {
      if ( (SYS_SYS_MODE_LTE == cfg_s_ptr->sys_mode) && (!cfg_s_ptr->cs_domain_fallback_on_lte) )
      {
        MSG_ERROR_0("CS domain fallback is disabled. No retry");
        ok_to_retry = FALSE;
      }
      else
      {
        /* SMS over PS failed, need to switch to CS_ONLY temporarily */
        MSG_HIGH_0("Temporarily changing domain pref to CS_ONLY");
        appInfo_ptr->curr_gw_domain_pref = WMS_GW_DOMAIN_PREF_CS_ONLY;
        if( FALSE == wms_cfg_mm_set_domain_pref( asid, appInfo_ptr->curr_gw_domain_pref ) )
        {
          MSG_ERROR_0("Cannot send domain switch to MM");
          /* Although command to MM failed, retry on the same domain is
          ** allowed. The temporary domain change in WMS will help
          ** the next MO SMS to go through.
          */
        }
      }
    }
    else if( WMS_DOMAIN_CS == domain )
    {
      MSG_ERROR_0("Got Perm domain error from NAS for CS Domain, stop retrying");
      ok_to_retry = FALSE;
    }
    else
    {
      /* Neither CS nor PS is available with permanent error; stop retrying */
      ok_to_retry = FALSE;
      MSG_ERROR_0("Stop retry: Neither CS nor PS is available");
    }
  }
  else if (WMS_RPT_OK != error_status)
  {
    /* For temporary NAS errors, don't need to switch to a different domain */
    MSG_HIGH_0("Other temp error from NAS; no domain switching");
  }

  //If SMS is deem retryable,  then go through the following producdure to   
  //get an appropiate retry interval timer value.
  retry_interval = 0;

  if ((WMS_RPT_LL_ERROR == error_status) &&
      (WMS_RP_CAUSE_TEMP_REJ == (wms_rp_cause_e_type)cause_value) &&
	  !((ims_cause_code == TIMER_F_TIMEOUT)&&
       (reg_mode_is_usim_compliant_for_cm(asid)==TRUE)))
  {
    ok_to_retry = TRUE;

    MSG_HIGH_0("Received Temporary Reject failure, Queuing for retry in 1 sec");
    retry_interval = 1;
  }
  else if ( ok_to_retry )
  {
    if ( (0 == msg_s_ptr->mo_retry_period)  && 
         (cfg_s_ptr->stk_sms_retry_enable) && 
         (WMS_CLIENT_TYPE_CAT == msg_s_ptr->gw_mo_rec[rec_idx]->client_id) )
    {
      MSG_HIGH_0("WMS_CLIENT_TYPE_CAT retry");
      retry_interval = WMS_TIMER_RETRY_INTERVAL;
    }
    else if (msg_s_ptr->mo_retry_period > 0) 
    {
      MSG_HIGH_0("Default behavior retry");
      if ( (SMS_IMS_TEMP_ERROR == (sms_ims_cause_type_T)msg_event_info.submit_report_info.report_status) &&
           retry_header_present )
      {
         retry_interval = retry_header_value;
         MSG_HIGH_1("Got SIP retry after header, value=%d", retry_interval);
      }
      else
      {
         retry_interval = msg_s_ptr->mo_retry_interval;
      }
    }
  }

  /* ========= END: SMS Domain Selection Procedures ============ */

  /* Retry iff: error && retry period > 0 &&
  ** period timer - interval timer > 0 &&
  ** retry is allowed  && rec is in use
  ** && state = LAYER_2_IN_PROGRESS
  */
  if( (WMS_RPT_OK != error_status) &&
      (TRUE == ok_to_retry) &&
      (retry_interval > 0) &&
      (msg_s_ptr->gw_mo_rec[rec_idx]->retry_time_left > (sint31)retry_interval) &&
      (wms_msg_is_mo_retry_allowed(msg_s_ptr->gw_mo_rec[rec_idx]->retry_count, &msg_event_info.submit_report_info))
      && (gcf_flag == FALSE)
    )
  {
    (void)wms_msg_enable_tp_rd(&(msg_s_ptr->gw_mo_rec[rec_idx]->sent_gw_msg->raw_ts_data));
    MSG_HIGH_0("Prepare to retry");
    MSG_HIGH_2("retry_time_left=%d, retry_interval=%d", msg_s_ptr->gw_mo_rec[rec_idx]->retry_time_left, retry_interval);

    /* Set the retry_wait_time */
    msg_s_ptr->gw_mo_rec[rec_idx]->retry_wait_time = (sint31)retry_interval;

    /* remember this report info, in case retry is unsuccessful */
    msg_s_ptr->gw_mo_rec[rec_idx]->prev_report = msg_event_info.submit_report_info;
    msg_s_ptr->gw_mo_rec[rec_idx]->retry_count++;
  }
  else
  {
    MSG_HIGH_0("No retry; sending event to clients");
    /* Send the event to clients
    */
    if (TRUE == gcf_flag)
    {
      msg_event_info.submit_report_info.delivery_failure_permanent = TRUE;
    }
    else
    {
      msg_event_info.submit_report_info.delivery_failure_permanent = is_permanent_error;
    }
    (void)wms_msg_event_notify( WMS_MSG_EVENT_SUBMIT_REPORT, &msg_event_info );

	{
      wms_prov_app_info_s_type *appInfo_ptr = NULL;

      appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(msg_event_info.submit_report_info.as_id , WMS_MESSAGE_MODE_GW);

      if (NULL != appInfo_ptr)
      {
        appInfo_ptr->mo_sms_count--;
        if (0 == appInfo_ptr->mo_sms_count)
        {
          wms_mm_sms_call_info_T  mm_status;

          memset(&mm_status, 0x00, sizeof(wms_mm_sms_call_info_T));

          mm_status.as_id = msg_event_info.submit_report_info.as_id;

          (void) wms_cm_gw_send_sms_status_to_mm(mm_status);
        }

      }
    }

    /* Update message status if it is in a memory store
    */
    if( msg_s_ptr->gw_mo_rec[rec_idx]->send_mode == WMS_SEND_MODE_MEMORY_STORE &&
        (WMS_RPT_OK == error_status))
    {
      /*If the sim_sms_reference slot is not set to the default value,
      the STATUS for the MO msg is expected.*/
      if( appInfo_ptr->sms_reference[rec_idx] != WMS_DUMMY_MESSAGE_NUMBER )
      {
        (void) wms_msg_do_modify_tag( appInfo_ptr,
                                      msg_s_ptr->gw_mo_rec[rec_idx]->msg_hdr.mem_store,
                                      msg_s_ptr->gw_mo_rec[rec_idx]->msg_hdr.index,
                                      WMS_TAG_MO_SENT_ST_NOT_RECEIVED,
                                      NULL );
      }
      else
      {
        (void) wms_msg_do_modify_tag( appInfo_ptr,
                                      msg_s_ptr->gw_mo_rec[rec_idx]->msg_hdr.mem_store,
                                      msg_s_ptr->gw_mo_rec[rec_idx]->msg_hdr.index,
                                      WMS_TAG_MO_SENT,
                                      NULL );
      }

    }

    wms_msg_evt_mo_sms_status( &msg_event_info, & msg_s_ptr->gw_mo_rec[rec_idx]->msg_hdr );

    /* Clear the msg record
    */
    wms_msg_gw_clear_mo_msg_rec(rec_idx);

    if ( wms_msg_suspend_ps_call_is_supported() )
    {
      wms_cm_resume_gw_pscall(asid);
    }
  }

  return;
} /* wms_msg_sm_report_ind_proc() */

#endif /* FEATURE_GWSMS */



/*===========================================================================
FUNCTION wms_msg_cleanup                                           (INTERNAL)

DESCRIPTION
  Release any resources created during startup.
  Used when the WMS task is stopped.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases various memory allocations acquired at startup.
===========================================================================*/
void wms_msg_cleanup(
  void
)
{
  wms_msg_s_type   *msg_s_ptr = wms_msg_s_ptr();
  int               idx;

  WMS_MEM_FREEIF(cdma_tl.cl_bd.scpt_data_ptr);
  WMS_MEM_FREEIF(cdma_tl.cl_bd.scpt_result_ptr);

  for (idx = 0; idx < WMS_MAX_MO_MESSAGES; idx++)
  {
    if (NULL != msg_s_ptr->gw_mo_rec[idx])
    {
      MSG_HIGH_3( "MSG Cleanup: releasing in-use MO rec: client %d, as_id %d, SMI %d",
                  msg_s_ptr->gw_mo_rec[idx]->client_id,
                  msg_s_ptr->gw_mo_rec[idx]->as_id,
                  msg_s_ptr->gw_mo_rec[idx]->smi );

      wms_mem_free(msg_s_ptr->gw_mo_rec[idx]);
      msg_s_ptr->gw_mo_rec[idx] = NULL;
    }
  }

  for (idx = 0; idx < WMS_MAX_MT_MESSAGES; idx++)
  {
    if (NULL !=msg_s_ptr->gw_mt_rec[idx])
    {
      MSG_HIGH_3( "MSG Cleanup: releasing in-use MT rec: client %d, as_id %d, SMI %d",
                  msg_s_ptr->gw_mt_rec[idx]->client_id,
                  msg_s_ptr->gw_mt_rec[idx]->as_id,
                  msg_s_ptr->gw_mt_rec[idx]->smi );

      wms_mem_free(msg_s_ptr->gw_mt_rec[idx]);
      msg_s_ptr->gw_mt_rec[idx] = NULL;
    }
  }

} /* wms_msg_cleanup() */

/*
*/
void wms_msg_init
(
  void
)
{
  uint32    i;

  msg_s_ptr = wms_msg_s_ptr();
  cfg_s_ptr = wms_cfg_s_ptr();
#ifdef FEATURE_CDSMS
  dc_s_ptr  = wms_dc_s_ptr();
#endif /* FEATURE_CDSMS */

  rex_def_timer(&wms_retry_timer,
                rex_self(),
                 WMS_MO_RETRY_TIMER_SIG);

#ifdef FEATURE_CDSMS
  msg_s_ptr->tid             = 0;
  msg_s_ptr->tl_seq_num      = WMS_TL_SEQ_NUM_MAX - 1;
  msg_s_ptr->mc_msg.is_valid = FALSE;
#ifdef FEATURE_ACP
  msg_s_ptr->awi_info.is_valid = FALSE;
#endif /* FEATURE_ACP */
  /* -------------------- Init message record queues -------------------- */
  for( i=0; i < WMS_MAX_MT_MESSAGES; i++ )
  {
    msg_s_ptr->cdma_mt_rec[ i ] = NULL;
  }

  msg_s_ptr->cdma_mo_rec.head_ptr = NULL;
  msg_s_ptr->cdma_mo_rec.tail_ptr = NULL;
  msg_s_ptr->cdma_mo_rec.count = 0;

#ifdef FEATURE_ACP
  rex_def_timer( & msg_s_ptr->awi_info.timer,
                 rex_self(),
                 WMS_AWISMS_ACK_TIMER_SIG
               );
#endif /* FEATURE_ACP */
  /* ---------------- Init the command queues and buffers ---------------- */

  /* invalidate CDMA sms msg global data
  */
  msg_s_ptr->mc_msg.is_valid = FALSE;

  msg_s_ptr->tl_seq_num = WMS_TL_SEQ_NUM_MAX-1;  /* next value will be 0 */
  msg_s_ptr->non_tl_seq_num = WMS_TL_SEQ_NUM_MAX-1;

#ifdef FEATURE_CDSMS_BROADCAST
  /* Allocate buffers for SCPT data and result pointers:
  */
  cdma_tl.cl_bd.scpt_data_ptr
                  = wms_mem_malloc( sizeof(wms_bc_scpt_data_s_type) );
  cdma_tl.cl_bd.scpt_result_ptr
                  = wms_mem_malloc( sizeof(wms_bc_scpt_result_s_type) );

  if(cdma_tl.cl_bd.scpt_data_ptr == NULL || cdma_tl.cl_bd.scpt_result_ptr == NULL)
  {
    MSG_ERROR_0("Null ptr - cdma_tl.cl_bd.scpt_data_ptr & cdma_tl.cl_bd.scpt_result_ptr");
    return;
  }
#endif /* FEATURE_CDSMS_BROADCAST */

  /* done with all init's
  */
  MSG_HIGH_0( "wms_msg_init(): CDMA done");

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
  for( i=0; i<WMS_MAX_MO_MESSAGES; i++ )
  {
    msg_s_ptr->gw_mo_rec[i] = NULL;
  }

  for( i=0; i<WMS_MAX_MT_MESSAGES; i++ )
  {
    msg_s_ptr->gw_mt_rec[i] = NULL;
  }

  /* done with all init's
  */
  MSG_HIGH_0( "wms_msg_init(): GW done");

#endif /* FEATURE_GWSMS */

  msg_s_ptr->timerf_value = 0; 

  // Make sure as_id field is at default
  memset(&msg_event_info, 0, sizeof(wms_msg_event_info_s_type));

  /* done */
  return;

} /* wms_msg_init() */




#ifdef FEATURE_CDSMS
/*=========================================================================
FUNCTION
  wms_msg_cdma_mt_ack_timer_proc

DESCRIPTION
  This function processed the MT SMS Ack Timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a Negative Ack to the Network

=========================================================================*/
void wms_msg_cdma_mt_ack_timer_proc
(
  void
)
{
  uint32                            i;
  wms_msg_cdma_message_record_type  *rec_ptr = NULL;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH_0( "got WMS_MT_ACK_TIMER_SIG");

  (void) rex_clr_sigs( rex_self(), WMS_MT_ACK_TIMER_SIG );

  /* process each record whose timer expired
  */
  for( i = 0; i < WMS_MAX_MT_MESSAGES; i ++ )
  {
    if( NULL != msg_s_ptr->cdma_mt_rec[i] &&
        rex_get_timer( & msg_s_ptr->cdma_mt_rec[i]->timer ) == 0 )
    {
      rec_ptr = msg_s_ptr->cdma_mt_rec[i];

      MSG_HIGH_2( "wms_msg_cdma_mt_ack_timer_proc(): MT ACK timer expired for tid=0x%lx, seq=%d",
                  rec_ptr->user_data, rec_ptr->seq_num );

      if( rec_ptr->state == WMS_MSG_STATE_WAIT_FOR_ACK )
      {
        MSG_HIGH_0( "Client didn't ack msg");

        //Change state to ACK in process to locked out other clients that ACK late 
        rec_ptr->state = WMS_MSG_STATE_ACK_IN_PROGRESS;

        /* Assign Client ID of Acking Client */
        rec_ptr->client_id = WMS_CLIENT_TYPE_INTERNAL;

        (void)wms_msg_cdma_ack_msg_i( rec_ptr, WMS_ERROR_TEMP, WMS_TL_NO_ACK_S );
      }
      else if( rec_ptr->state == WMS_MSG_STATE_LAYER2_IN_PROGRESS )
      {
        /* this timer sig is from the layer2 ack timer */
        /* sending ack msg failed; report error to user */
        MSG_HIGH_0( "MT ack layer2 timeout");
      }
      else if (( WMS_MSG_STATE_QUEUED == rec_ptr->state ) ||
               ( WMS_MSG_STATE_ACK_IN_PROGRESS == rec_ptr->state))
      {
        MSG_HIGH_0( "MT ack queued too long");
      }
      else
      {
        rec_ptr->state = WMS_MSG_STATE_IDLE;
        MSG_ERROR_1("Unexpected msg state %d", rec_ptr->state);
      }

      if ( (WMS_CLIENT_TYPE_INTERNAL != rec_ptr->client_id) &&
           (WMS_MSG_STATE_WAIT_FOR_ACK != rec_ptr->state) )
      {
        msg_event_info.ack_report_info.client_id = rec_ptr->client_id;
        msg_event_info.ack_report_info.transaction_id =
          (wms_transaction_id_type)rec_ptr->user_data;
        msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
        msg_event_info.ack_report_info.user_data = rec_ptr->mt_rec_user_data;
        msg_event_info.ack_report_info.is_success = FALSE;
        msg_event_info.ack_report_info.error_status = WMS_RPT_GENERAL_ERROR;
        (void)wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
      }

      wms_msg_cdma_free_mt_msg_rec( rec_ptr );

    } /* if timer expired */
  } /* for */

  /* done */
  return;

} /* wms_msg_cdma_mt_ack_timer_proc() */


/*=========================================================================
FUNCTION
  wms_msg_awi_ack_timer_proc

DESCRIPTION
  This function processes the AWI Ack Timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a Negative Ack to the Network

=========================================================================*/
void wms_msg_awi_ack_timer_proc
(
  void
)
{
#ifdef FEATURE_ACP
  MSG_HIGH_0( "got WMS_AWISMS_ACK_TIMER_SIG");

  (void) rex_clr_sigs( rex_self(), WMS_AWISMS_ACK_TIMER_SIG );

  if( msg_s_ptr->awi_info.is_valid == TRUE )
  {
    /* send the order with time out error
    */
    wms_msg_awi_ack( TRUE, FALSE, WMS_ERROR_TEMP, WMS_TL_NO_ACK_S );

    /* invalidate the awi info
    */
    msg_s_ptr->awi_info.is_valid = FALSE;
  }
  else
  {
    MSG_ERROR_0("unexpected AWISMS timer signal");
  }

  /* done */
  return;
#endif /* FEATURE_ACP */
} /* wms_msg_awi_ack_timer_proc() */



/*=========================================================================
FUNCTION
  wms_msg_mc_mo_status

DESCRIPTION
  This function processed the MO SMS Status recieved from MC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Posts a command to WMS Command Queue

=========================================================================*/
void wms_msg_mc_mo_status
(
  wms_error_class_e_type     error_class,
  wms_status_e_type          status,
  wms_bearer_e_type          bearer
)
{
  wms_cmd_type    * cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( msg_s_ptr->mc_msg.is_valid == FALSE )
  {
    return;  /* no msg being sent; no need to report */
  }

  /* got a layer2 status */
  /* report to client */
  /* send a command to sms task */

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL )
  {
    /* Set up command */
    cmd_ptr->hdr.cmd_id      = WMS_CMD_MSG_MC_MO_STATUS_E;
    cmd_ptr->hdr.client_id   =  (uint8)WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;
    cmd_ptr->hdr.as_id       = wms_get_appinfo_1x_asid();
    cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_1x();

    cmd_ptr->cmd.mc_mo_status.error_class   = error_class;
    cmd_ptr->cmd.mc_mo_status.status     = status;
    cmd_ptr->cmd.mc_mo_status.bearer     = bearer;

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    MSG_ERROR_0("Out of cmd buf");
    /* else, command buffer allocation failed. Nothing to do since
       error was reported/logged already; the command will be ignored.
    */
  }

  /* done */
  return;

} /* end of wms_msg_mc_mo_status() */


/*=========================================================================
FUNCTION
  wms_msg_cdma_mt_MWI

DESCRIPTION
  This function sends a command to the SMS task for processing the received
  message waiting indication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Add a command to the SMS command queue.

=========================================================================*/
void wms_msg_cdma_mt_MWI(
  byte  msg_count
)
{
  wms_cmd_type    *cmd_ptr;

  MSG_HIGH_0("wms_msg_cdma_mt_MWI() called");

  cmd_ptr = wms_get_cmd_buf();

  if (cmd_ptr != NULL)
  {
    /* Set up command */
    cmd_ptr->hdr.cmd_id      = WMS_CMD_MSG_MC_MWI_E;
    cmd_ptr->hdr.client_id   =  (uint8)WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;
    cmd_ptr->hdr.as_id       = wms_get_appinfo_1x_asid();
    cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_1x();

    cmd_ptr->cmd.mwi.msg_count   = msg_count;

    wms_put_cmd( cmd_ptr );
  }
  /* else, command buffer allocation failed. Nothing to do since
     error was reported/logged already; the command will be ignored.  */

  return;
} /* end of wms_msg_cdma_mt_MWI() */


/*===========================================================================
FUNCTION   wms_msg_awi_store_info

DESCRIPTION
  Parse and store a segment of AWI SMS message into the AWI info buffer.

DEPENDENCIES
  None

RETURNS
  Status of the storing.

SIDE EFFECTS
  Update AWI info buffer.
===========================================================================*/
wms_error_class_e_type wms_msg_awi_store_info
(
  byte      awi_cnt,     /* Current AWI SMS info data byte count */

  /* Pointer to received (mobile-terminated) analog IS-95 AWI data
     to be translated (word 1 not included) */
  byte      * awi_ptr,
  wms_cdma_tl_status_e_type  * tl_status_ptr /* OUT */
)
{
#ifdef FEATURE_ACP
  word      i, j;          /* Index for looping through char/digits */
  word      msg_pos;       /* Bit position in awi_ptr */
  word      chari_size;    /* Bit size of each INFO_DATA field */
  byte      num_of_octets; /* Number of octets per AWI 28-bit word */

  /* Number of AWI 28-bit data words
     (12-bit parity field excluded in these data words)
  */
  word      num_of_awi_words;

  wms_error_class_e_type  err = WMS_ERROR_NONE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Unpack the number of remaining AWI data words     */
  num_of_awi_words = b_unpackb( awi_ptr, 2, 7 );

  /* Set current bit position to info_data field */
  /* and char/digit size in first AWI word.      */
  msg_pos    = 0;
  chari_size = 8;

  /* Set AWI SMS info data byte count */
  msg_s_ptr->awi_info.cnt = awi_cnt;

  MSG_HIGH_1("AWI store: num_of_awi_words=%d", num_of_awi_words);
  MSG_HIGH_1("AWI store: awi_info.cnt=%d", msg_s_ptr->awi_info.cnt);

  /* Loop - unpack each info_data field from each AWI data word */
  /* and create a contiguous info_data field for bearer data    */
  /* translation.                                               */
  for (i=0; i <= num_of_awi_words; i++)
  {
    /* Skip over header data */
    msg_pos += 8;

    /* All AWI data words info_data except first word are 24 bits */
    num_of_octets = 3;

    /* First AWI data word has info data in different */
    /* bit position and size than the rest            */
    if (i == 0 )
    {
      msg_pos += 12;
      num_of_octets = 1;
    }

    /* Loop - copy each info_data field to contiguous array */
    for (j=0; j < num_of_octets; j++ )
    {
      msg_s_ptr->awi_info.data[ msg_s_ptr->awi_info.cnt ] =
            b_unpackb( awi_ptr, msg_pos, chari_size );
      msg_s_ptr->awi_info.cnt++;
      msg_pos += chari_size;

      if( msg_s_ptr->awi_info.cnt >= WMS_MAX_LEN )
      {
        err = WMS_ERROR_TEMP;
        *tl_status_ptr = WMS_TL_UNEXPECTED_PARM_SIZE_S;
        MSG_HIGH_1("max sms len exceeded! (#words=%d)", num_of_awi_words);
        msg_s_ptr->awi_info.cnt    = 0;
        break;
      }

    } /* for j<num_of_octets */

  } /* for i<num_of_awi_words */


  /* done */
  return err;
#else /* FEATURE_ACP */
  (void)awi_cnt; // parameter not used
  (void)awi_ptr; // parameter not used
  (void)tl_status_ptr; // parameter not used
#endif /* FEATURE_ACP */
  return WMS_ERROR_NONE;
} /* end of wms_msg_awi_store_info() */



/*===========================================================================
FUNCTION   wms_msg_awi_assemble_mt

DESCRIPTION
  Assemble a segment of AWI SMS message into the AWI info buffer.

DEPENDENCIES
  None

RETURNS
  Status of the assembling.

SIDE EFFECTS
  Update AWI info buffer.

===========================================================================*/
wms_error_class_e_type wms_msg_awi_assemble_mt
(
  byte           sms_len,
  byte           * sms_ptr,
  boolean        * msg_complete_ptr,   /* OUT: if the entire msg is received */
  wms_cdma_tl_status_e_type  * tl_status_ptr /* OUT */
)
{
  wms_error_class_e_type     err = WMS_ERROR_NONE;

#ifdef FEATURE_ACP
  /* Sequence Number field from AWI SMS message */
  byte                    seq_no;

  /* Begin/Final field from AWI SMS message */
  wms_awi_flag_e_type   bf;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_HIGH_0("Assembling AWI msg");

  /* by default msg is not complele
  */
  * msg_complete_ptr = FALSE;

  /* Retrieve Begin/Final and Sequence Number fields to */
  /* determine type of segmentation for AWI SMS message */
  bf     = (wms_awi_flag_e_type) b_unpackb( sms_ptr, 12, 2 );
  seq_no = b_unpackb( sms_ptr, 9, 3 );

  /*---------------------------------------*/
  /* bf = 0 : intermediate segment         */
  /* bf = 1 : final segment                */
  /* bf = 2 : first segment                */
  /* bf = 3 : unsegmented                  */
  /* Sequence Number will begin at 0 for   */
  /* new SMS message.                      */
  /*---------------------------------------*/

  MSG_HIGH_2( "AWI msg: bf=%d, seq_no=%d",
              (int)bf,
              seq_no );

  /* Do BF end seq_no error checking
  */
  if(
      ( bf == WMS_AWI_FIRST && msg_s_ptr->awi_info.cnt != 0 )
      ||
      ( bf == WMS_AWI_FIRST && msg_s_ptr->awi_info.seq_no != 0 )
      ||
      ( bf == WMS_AWI_FIRST && seq_no != 0 )
      ||
      ( bf == WMS_AWI_UNSEGMENTED && msg_s_ptr->awi_info.cnt != 0 )
      ||
      ( bf == WMS_AWI_UNSEGMENTED && msg_s_ptr->awi_info.seq_no != 0 )
      ||
      ( bf == WMS_AWI_INTERMEDIATE && msg_s_ptr->awi_info.cnt == 0 )
      ||
      ( bf == WMS_AWI_FINAL && msg_s_ptr->awi_info.cnt == 0 )
    )
  {
    MSG_HIGH_0("AWI assembly error: bf and seq_no error check");
    MSG_HIGH_2( "awi_info.seq_no=%d, awi_info.cnt=%d",
                (msg_s_ptr->awi_info.seq_no + 1) % 8,
                msg_s_ptr->awi_info.cnt );
    msg_s_ptr->awi_info.seq_no = 0;
    msg_s_ptr->awi_info.cnt    = 0;
    err = WMS_ERROR_TEMP;
    *tl_status_ptr = WMS_TL_UNEXPECTED_PARM_VALUE_S;
    return err; /*** SHORT RETURN ***/
  }


  /* Determine segmentation processing */
  if( (bf == WMS_AWI_UNSEGMENTED || bf == WMS_AWI_FIRST) && (seq_no == 0) )
  {
    /* Copy AWI SMS info data fields into contiguous array */
    /* for translation.                                    */
    err = wms_msg_awi_store_info( 0, sms_ptr, tl_status_ptr );

    if ( bf == WMS_AWI_UNSEGMENTED )
    {
      * msg_complete_ptr = TRUE;
    }
  }
  else if( bf == WMS_AWI_INTERMEDIATE || bf == WMS_AWI_FINAL )
  {
    /* If sequence number is equal to next sequence */
    /* expected, then save info data.               */
    if( seq_no == (msg_s_ptr->awi_info.seq_no + 1)%8 )
    {
      err = wms_msg_awi_store_info( msg_s_ptr->awi_info.cnt,
                                    sms_ptr,
                                    tl_status_ptr );

      /* If last segment received, then indicate entire */
      /* message has been received.                     */
      if( bf == WMS_AWI_FINAL )
      {
        * msg_complete_ptr = TRUE;
      }
    }
    else if( seq_no == msg_s_ptr->awi_info.seq_no )
    {
      MSG_HIGH_1("duplicate msg ignored: seq=%d", seq_no);
    }
    else
    {
      /* If not a duplicate or next expected sequence number,
         then discard entire message.
      */
      err = WMS_ERROR_TEMP;
      *tl_status_ptr = WMS_TL_UNEXPECTED_PARM_VALUE_S;
      MSG_HIGH_2( "expecting seq %d but got %d!",
                  (msg_s_ptr->awi_info.seq_no + 1) % 8,
                  seq_no );
    }
  }
  else
  {
    /* Message not expected or duplicate so
       discard entire message.
    */
    err = WMS_ERROR_TEMP;
    *tl_status_ptr = WMS_TL_UNEXPECTED_PARM_VALUE_S;
    MSG_HIGH_2( "unexpected msg. bf=%d, seq_no=%d",
                bf,
                seq_no );
  }


  /* Set sequence number */
  if( err != WMS_ERROR_NONE )
  {
    /* Clear the count in case of error
    */
    msg_s_ptr->awi_info.seq_no = 0;
    msg_s_ptr->awi_info.cnt    = 0;
  }
  else
  {
    msg_s_ptr->awi_info.seq_no = seq_no;
  }

  msg_s_ptr->awi_info.bf = bf;
#else /* FEATURE_ACP */
  (void)sms_len; // parameter not used
  (void)sms_ptr; // parameter not used
  (void)msg_complete_ptr; // parameter not used
  (void)tl_status_ptr; // parameter not used
#endif /* FEATURE_ACP */

  return err;

} /* end of wms_msg_awi_assemble_mt() */



/*=========================================================================
FUNCTION
  wms_msg_cdma_mt_msg

DESCRIPTION
  This function receives an SMS message and sends a command to the SMS task
  for processing.

  For Analog AWI SMS messages, segments are assembled into a complete
  message before being sent to the SMS task and an ack order is returned
  for intermediate segments.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Add a command to the command queue. AWI SMS info is updated for AWI SMS
  segments.

=========================================================================*/
void wms_msg_cdma_mt_msg
(
  wms_format_e_type      format,

  /* length of sms message in bytes */
  word                   sms_len,

  /* Pointer to received (mobile-terminated) SMS message */
  byte                 * sms_ptr,
  wms_bearer_e_type      bearer,
  boolean                ack_pending,
  sys_modem_as_id_e_type as_id
)
{
  wms_cmd_type            * cmd_ptr;
  wms_error_class_e_type  error_class  = WMS_ERROR_NONE;
  wms_cdma_tl_status_e_type    tl_status = WMS_TL_ADDRESS_VACANT_S;
  boolean                 msg_complete = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH_0("wms_msg_cdma_mt_msg() called");


  if( format == WMS_FORMAT_MWI )
  {
    wms_msg_cdma_mt_MWI( *sms_ptr );  // data is only one byte, i.e. msg_count
    return; /* SHORT-RETURN */
  }


  if( sms_len > WMS_MAX_LEN )
  {
    MSG_HIGH_2( "MT msg: msg len %d is larger than max %d",
                sms_len,
                WMS_MAX_LEN );
    error_class = WMS_ERROR_PERM;
    tl_status   = WMS_TL_UNRECOGNIZED_PARM_VALUE_S;
  }


  if( format >= WMS_FORMAT_MAX )
  {
    MSG_ERROR_1( "MT msg: msg format %d is invalid!", format);
  }
  else if( format == WMS_FORMAT_ANALOG_AWISMS )
  {
    /*----------------------------------------------------------------------
      For AWISMS msgs, L2 ack + L3 ack will be available if msg is not
      complete or there is an error. Ack will not be available immediately
      if msg is complete and has been sent to the SMS task.
    -----------------------------------------------------------------------*/

    /* assemble the segments
    */
    if( error_class == WMS_ERROR_NONE )
    {
      error_class = wms_msg_awi_assemble_mt( (byte)sms_len,
                                             sms_ptr,
                                             & msg_complete,
                                             & tl_status );
    }

    /* if msg is not complete or if there is an error, return status in
       order confirmation within the MC task context
    */
    if( error_class != WMS_ERROR_NONE )
    {
      /* msg failed */
      MSG_HIGH_2( "AWI ack: error_class=%d, tl_status=%d",
                  error_class,
                  tl_status );
      wms_msg_awi_ack( TRUE, FALSE, error_class, tl_status );
    }
    else if( msg_complete == FALSE )
    {
      /* msg Ok, but waiting for more segments */
      MSG_HIGH_0("AWI Ack: no errors");
      wms_msg_awi_ack( FALSE, FALSE, WMS_ERROR_NONE, tl_status );
    }
    /* else: no ack is sent immediately;
       msg will be delivered to the UI and the UI needs to ack it.
    */
  }
  /* else send msg to sms task for cdma and is91ep msgs */


  /* send the cmd; in case of AWISMS, the msg should be complete
  */
  if( error_class == WMS_ERROR_NONE
      &&
      ( format        != WMS_FORMAT_ANALOG_AWISMS ||
        msg_complete  == TRUE
      )
    )
  {
    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr == NULL )
    {
      /* Command buffer allocation failed.
      */
      error_class = WMS_ERROR_TEMP;
      tl_status   = WMS_TL_DESTINATION_RESOURCE_SHORTAGE_S;

      /* send tl ack in MC task context
      */
      if( format == WMS_FORMAT_CDMA )
      {
        /* no action taken; the base station will resend the msg */
      }
      else if( format == WMS_FORMAT_ANALOG_AWISMS )
      {
        /* msg failed */
        MSG_HIGH_2( "AWI ack: error_class=%d, tl_status=%d",
                    error_class,
                    tl_status );
        wms_msg_awi_ack( TRUE, FALSE, error_class, tl_status );
      }
      /* else there is no tl ack available for reporting errors for
         other formats
      */
    }
    else
    {

      wms_msg_transport_type_e_type transport_type =
                        (WMS_BEARER_IMS_3GPP2 == bearer)?
                         WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS: WMS_MSG_TRANSPORT_TYPE_CS;
      /* Set up command
      */
      cmd_ptr->hdr.cmd_id      = WMS_CMD_MSG_MC_MT_MSG_E;
      cmd_ptr->hdr.client_id   = (wms_client_id_type) WMS_CLIENT_TYPE_INTERNAL;
      cmd_ptr->hdr.cmd_cb      = NULL;
      cmd_ptr->hdr.user_data   = NULL;
      cmd_ptr->hdr.as_id       = as_id;

      if(WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == transport_type)
      {
        cmd_ptr->hdr.appInfo_ptr =  wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);
      }
      else
      {
        cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_1x();
      }

      cmd_ptr->cmd.mt_msg.ota.format    = format;
      cmd_ptr->cmd.mt_msg.bearer        = bearer;

      if( format == WMS_FORMAT_ANALOG_AWISMS )
      {
#ifdef FEATURE_ACP
        msg_s_ptr->awi_info.is_valid      = TRUE;
        cmd_ptr->cmd.mt_msg.ota.data_len  = msg_s_ptr->awi_info.cnt;

        (void)memscpy( (byte *)cmd_ptr->cmd.mt_msg.ota.data,
                       msg_s_ptr->awi_info.cnt,
                       msg_s_ptr->awi_info.data,
                       msg_s_ptr->awi_info.cnt );
#endif /* FEATURE_ACP */
      }
      else
      {
        /* CDMA or IS91EP */
        cmd_ptr->cmd.mt_msg.ota.data_len = sms_len;
        (void)memscpy( (byte *)cmd_ptr->cmd.mt_msg.ota.data,
                       MIN(sms_len, WMS_MAX_LEN),
                       sms_ptr,
                       MIN(sms_len, WMS_MAX_LEN) );
        cmd_ptr->cmd.mt_msg.ack_pending  =  ack_pending;
      }

      /* Send it to WMS task command queue
      */
      wms_put_cmd( cmd_ptr );
    }

  } /* if */


  /* done */
  return;

} /* wms_msg_cdma_mt_msg() */

#endif /* FEATURE_CDSMS */

/*=========================================================================
FUNCTION
  wms_msg_cdma_mt_busy

DESCRIPTION
  Check whether CDMA MT records are in use or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: busy
  FALSE: not busy

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_cdma_mt_busy
(
  void
)
{
  boolean busy = FALSE;
#ifdef FEATURE_CDSMS
  uint32  i;

  for( i=0; i < WMS_MAX_MT_MESSAGES; i++ )
  {
    if( (NULL != msg_s_ptr->cdma_mt_rec[i]) &&
        (WMS_MSG_STATE_IDLE != msg_s_ptr->cdma_mt_rec[i]->state) )
    {
      // This Record in Use
      busy = TRUE;
      break;
    }
  }
#endif /* FEATURE_CDSMS */

  return busy;

} /* wms_msg_cdma_mt_busy() */


/*=========================================================================
FUNCTION
  wms_msg_cdma_mo_busy

DESCRIPTION
  Check whether CDMA MO records are in use or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: busy
  FALSE: not busy

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_cdma_mo_busy
(
  cdma_mo_next_retry_rec_s_type *next_retry_rec_ptr
)
{
boolean busy = FALSE;

#ifdef FEATURE_CDSMS
  {
    wms_msg_cdma_message_record_type *rec_ptr =  msg_s_ptr->cdma_mo_rec.head_ptr;

    if ( NULL != next_retry_rec_ptr ) 
    {
      next_retry_rec_ptr->state           = WMS_MSG_STATE_IDLE;
      next_retry_rec_ptr->ch_pref         = WMS_CLIENT_CH_NONE;
      next_retry_rec_ptr->retry_time_left = WMS_MAX_RETRY_PERIOD;
      next_retry_rec_ptr->retry_wait_time = WMS_MAX_RETRY_PERIOD;
    }

      while ( NULL != rec_ptr )
      {
        if ( WMS_MSG_STATE_IDLE != rec_ptr->state )
        {
          // Queue is not empty
          busy = TRUE;

          /* Determine if this is the next record scheduled for retry. */
          if ( (NULL != next_retry_rec_ptr) &&
               (WMS_MSG_STATE_QUEUED == rec_ptr->state) &&
               (rec_ptr->retry_time_left >= rec_ptr->retry_wait_time) &&
               (next_retry_rec_ptr->retry_wait_time > rec_ptr->retry_wait_time) )
          {
            next_retry_rec_ptr->state = WMS_MSG_STATE_QUEUED;
            next_retry_rec_ptr->ch_pref = rec_ptr->ch_pref;
            next_retry_rec_ptr->retry_time_left = rec_ptr->retry_time_left;
            next_retry_rec_ptr->retry_wait_time = rec_ptr->retry_wait_time;
          }
        }
        rec_ptr = rec_ptr->next;
      }
    }
  #endif /* FEATURE_CDSMS */

  return busy;

} /* wms_msg_cdma_mo_busy() */



/*=========================================================================
FUNCTION
  wms_msg_gw_mt_busy

DESCRIPTION
  Check whether GW MT records are in use or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: busy
  FALSE: not busy

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_gw_mt_busy(
  sys_modem_as_id_e_type as_id
)
{
  boolean busy = FALSE;
#ifdef FEATURE_GWSMS
  uint32  i;

  for (i=0; i < WMS_MAX_MT_MESSAGES; i++)
  {
    if ((NULL != msg_s_ptr->gw_mt_rec[i]) && (msg_s_ptr->gw_mt_rec[i]->as_id == as_id))
    {
      /* This Record in Use */
       busy = TRUE;
      break;
    }
  }
#endif /* FEATURE_GWSMS */

  return busy;

} /* wms_msg_gw_mt_busy() */

/*=========================================================================
FUNCTION
  wms_msg_gw_mo_busy

DESCRIPTION
  Check whether GW MO (Non SMMA) records are in use or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: busy
  FALSE: not busy

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_gw_mo_busy(
  sys_modem_as_id_e_type as_id
)
{
  boolean busy = FALSE;
#ifdef FEATURE_GWSMS
  uint32  i;

  for (i=0; i < WMS_MAX_MO_MESSAGES; i++)
  {
    if ((NULL != msg_s_ptr->gw_mo_rec[i]) && (msg_s_ptr->gw_mo_rec[i]->as_id == as_id))
    {
      // Ignore the SMMA records while checking for MO busy.
      if (WMS_MSG_SMMA_REQUEST_ID != msg_s_ptr->gw_mo_rec[i]->smi)
      {
        /* This Record in Use */
        busy = TRUE;
        break;
      }
    }
  }
#endif /* FEATURE_GWSMS */

  return busy;

} /* wms_msg_gw_mo_busy() */

/*=========================================================================
FUNCTION: wms_msg_check_pending_mo_messages

DESCRIPTION
  Check whether any MO records (CDMA or GW) are in use or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  busy
  FALSE: not busy

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_check_pending_mo_messages(
  sys_modem_as_id_e_type as_id
)
{
  boolean pending_mo_messages = FALSE;

  // Check GW messages
  pending_mo_messages = wms_msg_gw_mo_busy(as_id);

  if (!pending_mo_messages)
  {
    // Check CDMA messages
    pending_mo_messages = wms_msg_cdma_mo_busy( NULL );
  }

  return pending_mo_messages;
} /* wms_msg_check_pending_mo_messages() */

/*=========================================================================
FUNCTION
  wms_msg_get_cache_info

DESCRIPTION
  Allow the client to retrieve the message cache info for a message stored in
  a memory.

  NOTE: This is a synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  None.

=========================================================================*/
wms_status_e_type wms_msg_get_cache_info
(
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index,
  wms_message_tag_e_type          tag,
  uint8                           * cache_ptr
)
{
  (void)mem_store; // parameter not used
  (void)index; // parameter not used
  (void)tag; // parameter not used
  (void)cache_ptr; // parameter not used

  MSG_ERROR_0("WMS no longer supports msg info cache!");

  return WMS_UNSUPPORTED_S;

} /* wms_msg_get_cache_info() */

/*=========================================================================
FUNCTION
  wms_msg_check_in_use_records

DESCRIPTION
  Checks the MO / MT records for any in use records

DEPENDENCIES
  None

RETURN VALUE
  TRUE: There are in use records
  FALSE: No in use records

SIDE EFFECTS
  None.

=========================================================================*/
boolean wms_msg_check_in_use_records
(
  sys_modem_as_id_e_type as_id
)
{
  /* Initialize to Indicate No records in Use */
  boolean ret_val = FALSE;

  ret_val = wms_msg_cdma_mo_busy( NULL );

  if (!ret_val)
  {
    ret_val = wms_msg_cdma_mt_busy();
  }

  if (!ret_val)
  {
    ret_val = wms_msg_gw_mo_busy(as_id);
  }

  if (!ret_val)
  {
    ret_val = wms_msg_gw_mt_busy(as_id);
  }

  return ret_val;
} /* wms_msg_check_in_use_records() */


/*=========================================================================
FUNCTION
  wms_msg_mo_sms_busy

DESCRIPTION
  Checks the MO  records for any in use records

DEPENDENCIES
  None

RETURN VALUE
  TRUE: There are in use records
  FALSE: No in use records

SIDE EFFECTS
  None.

=========================================================================*/
boolean wms_msg_mo_sms_busy(
  sys_modem_as_id_e_type              as_id
)
{
  /* Initialize to Indicate No records in Use */
  boolean ret_val = FALSE;

  if(as_id != SYS_MODEM_AS_ID_2)
  {
    ret_val = wms_msg_cdma_mo_busy( NULL );
  }

  if (!ret_val)
  {
    ret_val = wms_msg_gw_mo_busy(as_id);
  }

  MSG_HIGH_1("wms_msg_mo_sms_busy: %d",ret_val );
  return ret_val;
} /* wms_msg_mo_sms_busy */


#ifdef FEATURE_GWSMS_BROADCAST

boolean wms_msg_gw_cb_msg_is_cmas(
  wms_gw_cb_srv_id_type cb_srv_id
)
{
  if( WMS_GW_CB_SRV_ID_CMAS_MSGID1 <= cb_srv_id &&
      cb_srv_id <= WMS_GW_CB_SRV_ID_CMAS_MSGID13)
  {
    return TRUE;
  }
  return FALSE;
}

#ifdef FEATURE_SMS_PWS_OVER_LTE
wms_cell_info_s_type wms_msg_gw_cb_get_cell_info_by_appInfo(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  wms_cell_info_s_type cell_info;

  memset( &cell_info, 0, sizeof(wms_cell_info_s_type) );
  
  if (NULL != appInfo_ptr)
  {
    wms_ss_stack_info_s_type *stack_info_ptr = wms_msg_get_current_gw_domain_pref_stack( appInfo_ptr );

    if (NULL != stack_info_ptr)
    {
      MSG_HIGH_1( "Current SYS_MODE: %d", stack_info_ptr->sys_mode );
      if (SYS_SYS_MODE_GSM == stack_info_ptr->sys_mode)
      {
        
        /* PLMN ID */
        /* sys_plmn.c */
        /**
               ** Define a structure that identifies a Public Land Mobile Network (PLMN).
               ** A PLMN ID is defined within a GSM network to be the combination of a Mobile
               ** Country Code (MCC) and Mobile Network Code (MNC).  A PLMN ID is stored in 3
               ** octets as specified in 3G TS 24.008 Section 10.5.1.3 and shown below:
               **                                        Bit
               **                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
               **                 |===============================================|
               **         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
               **                 |-----------------------------------------------|
               **         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
               **                 |-----------------------------------------------|
               **         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
               **                 |===============================================|
               **
               ** The criteria for comparing one PLMN against another is specified in
               ** 3G TS 23.122 Annex A.
               **/
        cell_info.selected_plmn.mcc[0] =  stack_info_ptr->sys_id.id.plmn.identity[0] & 0x0F;
        cell_info.selected_plmn.mcc[1] = (stack_info_ptr->sys_id.id.plmn.identity[0] & 0xF0) >> 4;
        cell_info.selected_plmn.mcc[2] =  stack_info_ptr->sys_id.id.plmn.identity[1] & 0x0F;
        cell_info.selected_plmn.mnc[0] =  stack_info_ptr->sys_id.id.plmn.identity[2] & 0x0F;
        cell_info.selected_plmn.mnc[1] = (stack_info_ptr->sys_id.id.plmn.identity[2] & 0xF0) >> 4;
        cell_info.selected_plmn.mnc[2] = (stack_info_ptr->sys_id.id.plmn.identity[1] & 0xF0) >> 4;

        if(0x0F == cell_info.selected_plmn.mnc[2])
        {
          cell_info.selected_plmn.num_mnc_digits = 2;
        }
        else
        {
          cell_info.selected_plmn.num_mnc_digits = 3;
        }
      }
      else if (SYS_SYS_MODE_WCDMA == stack_info_ptr->sys_mode || 
               SYS_SYS_MODE_TDS   == stack_info_ptr->sys_mode)
      {
        wms_cfg_s_type  *cfg_s_ptr  = wms_cfg_s_ptr();
        /* PLMN ID */
        /* sys_plmn.c */
        /**
               ** Define a structure that identifies a Public Land Mobile Network (PLMN).
               ** A PLMN ID is defined within a GSM network to be the combination of a Mobile
               ** Country Code (MCC) and Mobile Network Code (MNC).  A PLMN ID is stored in 3
               ** octets as specified in 3G TS 24.008 Section 10.5.1.3 and shown below:
               **                                        Bit
               **                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
               **                 |===============================================|
               **         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
               **                 |-----------------------------------------------|
               **         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
               **                 |-----------------------------------------------|
               **         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
               **                 |===============================================|
               **
               ** The criteria for comparing one PLMN against another is specified in
               ** 3G TS 23.122 Annex A.
               **/
        cell_info.selected_plmn.mcc[0] =  cfg_s_ptr->umts_loc_info.mcc_and_mnc[0] & 0x0F;
        cell_info.selected_plmn.mcc[1] = (cfg_s_ptr->umts_loc_info.mcc_and_mnc[0] & 0xF0) >> 4;
        cell_info.selected_plmn.mcc[2] =  cfg_s_ptr->umts_loc_info.mcc_and_mnc[1] & 0x0F;
        cell_info.selected_plmn.mnc[0] =  cfg_s_ptr->umts_loc_info.mcc_and_mnc[2] & 0x0F;
        cell_info.selected_plmn.mnc[1] = (cfg_s_ptr->umts_loc_info.mcc_and_mnc[2] & 0xF0) >> 4;
        cell_info.selected_plmn.mnc[2] = (cfg_s_ptr->umts_loc_info.mcc_and_mnc[1] & 0xF0) >> 4;

        if(0x0F == cell_info.selected_plmn.mnc[2])
        {
          cell_info.selected_plmn.num_mnc_digits = 2;
        }
        else
        {
          cell_info.selected_plmn.num_mnc_digits = 3;
        }
      }
      else
      {
        cell_info.selected_plmn   = current_cell_info.selected_plmn;
      }
    }
  }
  else
  {
    MSG_ERROR_0("appInfo_ptr is NULL");
  }

  return cell_info;
}
#endif /* FEATURE_SMS_PWS_OVER_LTE */


void wms_msg_gw_cb_data_ind_proc(
  sys_modem_as_id_e_type as_id,
  uint8                  *cb_page,
  int                     page_length,
  boolean                 message_from_lte
)
{
  wms_client_message_s_type   *msg_ptr;
  wms_prov_app_info_s_type    *appInfo_ptr;

  appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  if ( !asid_is_valid(appInfo_ptr->asid) )
  {
    return;
  }


  msg_event_info.mt_message_info.client_id       = WMS_CLIENT_TYPE_MAX;
  msg_event_info.mt_message_info.route           = WMS_ROUTE_TRANSFER_AND_ACK;
  msg_event_info.mt_message_info.transaction_id  = WMS_DUMMY_TRANSACTION_ID;
  msg_event_info.mt_message_info.as_id           = appInfo_ptr->asid;
  msg_event_info.mt_message_info.transport_type  = WMS_MSG_TRANSPORT_TYPE_CS;
  
  msg_ptr = &msg_event_info.mt_message_info.message;
  
  msg_ptr->msg_hdr.message_mode                     = WMS_MESSAGE_MODE_GW;
  msg_ptr->msg_hdr.tag                              = WMS_TAG_MT_NOT_READ;
  msg_ptr->msg_hdr.mem_store                        = WMS_MEMORY_STORE_NONE;
  msg_ptr->msg_hdr.index                            = WMS_DUMMY_MESSAGE_INDEX;
  
  msg_ptr->u.gw_message.is_broadcast                = TRUE;
  msg_ptr->u.gw_message.sc_address.number_of_digits = 0;
  msg_ptr->u.gw_message.raw_ts_data.format          = WMS_FORMAT_GW_CB;
  msg_ptr->u.gw_message.raw_ts_data.tpdu_type       = WMS_TPDU_NONE;
  msg_ptr->u.gw_message.raw_ts_data.len             = (uint32)MIN(WMS_MAX_LEN, page_length);
  memset( msg_ptr->u.gw_message.raw_ts_data.data,
          0,
          sizeof(msg_ptr->u.gw_message.raw_ts_data.data) );
  
  (void)memscpy( msg_ptr->u.gw_message.raw_ts_data.data,
                 msg_ptr->u.gw_message.raw_ts_data.len,
                 cb_page,
                 msg_ptr->u.gw_message.raw_ts_data.len );
  
  MSG_HIGH_1("Received CB SMS with page length = %d", page_length);
  
  wms_msg_gw_cb_send_log(&msg_ptr->u.gw_message.raw_ts_data);
  

    if (TRUE == wms_msg_gw_cb_data_download_proc( appInfo_ptr,
                                                  &msg_ptr->u.gw_message.raw_ts_data.data[0]) )
    {
      MSG_HIGH_0("This is a CB Data Download Message");
    }
    else
    {
      (void)wms_msg_event_notify( WMS_MSG_EVENT_RECEIVED_MESSAGE, &msg_event_info );
    }

    if ( WMS_OK_S ==
         wms_ts_decode( &msg_event_info.mt_message_info.message.u.gw_message.raw_ts_data,
                        &wms_msg_Gclient_ts ) )
    {
      if ( (WMS_GW_CB_SRV_ID_CMAS_MSGID1 <= wms_msg_Gclient_ts.u.gw_cb.cb_page_hdr.cb_srv_id) &&
           (wms_msg_Gclient_ts.u.gw_cb.cb_page_hdr.cb_srv_id <= WMS_GW_CB_SRV_ID_CMAS_MSGID26) )
      {
        wms_evt_cbs_msg_s_type       cmas_evt;

        cmas_evt.message_id    = wms_msg_Gclient_ts.u.gw_cb.cb_page_hdr.cb_srv_id;
        cmas_evt.message_code  = wms_msg_Gclient_ts.u.gw_cb.cb_page_hdr.message_code;
        cmas_evt.geo_scope     = (uint8)wms_msg_Gclient_ts.u.gw_cb.cb_page_hdr.geo_scope;
        cmas_evt.update_number = wms_msg_Gclient_ts.u.gw_cb.cb_page_hdr.update_number;
        cmas_evt.dcs_data      = wms_msg_Gclient_ts.u.gw_cb.cb_page_hdr.cb_dcs.raw_dcs_data;
        cmas_evt.user_alert    = (wms_msg_Gclient_ts.u.gw_cb.cb_page_hdr.message_code & 0x0200) >> 9;
        cmas_evt.popup         = (wms_msg_Gclient_ts.u.gw_cb.cb_page_hdr.message_code & 0x0100) >> 8;
        cmas_evt.format        = (uint8)WMS_FORMAT_GW_CB;
        cmas_evt.data_len      = (uint16)wms_msg_Gclient_ts.u.gw_cb.user_data.sm_len;

        event_report_payload(EVENT_SMS_CBS_MSG_RECEIVED, sizeof(cmas_evt), (void*)&cmas_evt);      
    }
  }


  return;
} /* wms_msg_gw_cb_data_ind_proc() */

void wms_msg_gw_cb_duplicate_ind_proc
(
  cb_duplication_detection_ind_type  * gw_cb_duplicate_ptr
)
{
  msg_event_info.cb_page_header.cb_srv_id         = (uint16)gw_cb_duplicate_ptr->message_id;

  (void)wms_ts_decode_gw_cb_dcs((uint8)gw_cb_duplicate_ptr->data_coding_scheme, NULL, &msg_event_info.cb_page_header.cb_dcs);

  msg_event_info.cb_page_header.total_pages       = (uint8)(gw_cb_duplicate_ptr->number_of_pages & 0x0F); /* 4 bits */
  msg_event_info.cb_page_header.page_number       = (uint8)(gw_cb_duplicate_ptr->page_number & 0x0F); /* 4 bits */
  msg_event_info.cb_page_header.geo_scope         = (wms_gw_cb_geo_scope_e_type)(gw_cb_duplicate_ptr->gs_indicator & 0x03); /* 2 bits */
  msg_event_info.cb_page_header.message_code      = (uint16)(gw_cb_duplicate_ptr->message_code & 0x03FF); /* 10 bits */
  msg_event_info.cb_page_header.update_number     = (uint8)(gw_cb_duplicate_ptr->update_number & 0x0F); /* 4 bits */
  msg_event_info.cb_page_header.raw_serial_number = (uint16)(((uint32)msg_event_info.cb_page_header.geo_scope << 14)
                                                    + (msg_event_info.cb_page_header.message_code << 4)
                                                    + (msg_event_info.cb_page_header.update_number) );
  if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
  {
    msg_event_info.cb_page_header.as_id           = gw_cb_duplicate_ptr->as_id;
  }
  else
  {
    msg_event_info.cb_page_header.as_id           = SYS_MODEM_AS_ID_1;
  }

  (void)wms_msg_event_notify( WMS_MSG_EVENT_DUPLICATE_CB_PAGE, & msg_event_info );

  /* done */
  return;

} /* wms_msg_gw_cb_duplicate_ind_proc() */

#if defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS)
void wms_msg_etws_prim_notification_ind_proc
(
  cb_etws_prim_notification_ind_type *cb_etws_prim_ind_ptr
)
{
  uint8                pos = 0;
  boolean              msg_is_dup   = FALSE;
  wms_etws_msg_s_type *etws_msg_ptr = &(msg_event_info.etws_msg_info);

  MSG_HIGH_0("In wms_msg_etws_prim_notification_ind_proc()");

  etws_msg_ptr->type = WMS_ETWS_NOTIFICATION_PRIMARY;
  etws_msg_ptr->plmn_info_present = TRUE;

  /* PLMN ID */
  /* sys_plmn.c */
  /**
   ** Define a structure that identifies a Public Land Mobile Network (PLMN).
   ** A PLMN ID is defined within a GSM network to be the combination of a Mobile
   ** Country Code (MCC) and Mobile Network Code (MNC).  A PLMN ID is stored in 3
   ** octets as specified in 3G TS 24.008 Section 10.5.1.3 and shown below:
   **                                        Bit
   **                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
   **                 |===============================================|
   **         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
   **                 |-----------------------------------------------|
   **         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
   **                 |-----------------------------------------------|
   **         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
   **                 |===============================================|
   **
   ** The criteria for comparing one PLMN against another is specified in
   ** 3G TS 23.122 Annex A.
   **/

  /**
      Table 10.5.3/3GPP TS 24.008: Location Area Identification
      information element

      MNC, Mobile network code (octet 3 bits 5 to 8, octet 4) The
      coding of this field is the responsibility of each
      administration but BCD coding shall be used. The MNC shall
      consist of 2 or 3 digits. For PCS 1900 for NA, Federal
      regulation mandates that a 3-digit MNC shall be used.
      However a network operator may decide to use only two digits
      in the MNC in the LAI over the radio interface. In this
      case, bits 5 to 8 of octet 3 shall be coded as "1111".
    **/
  etws_msg_ptr->plmn_info.mcc[0] = cb_etws_prim_ind_ptr->plmn_id.identity[0] & 0x0F;
  etws_msg_ptr->plmn_info.mcc[1] = (cb_etws_prim_ind_ptr->plmn_id.identity[0] & 0xF0) >> 4;
  etws_msg_ptr->plmn_info.mcc[2] = cb_etws_prim_ind_ptr->plmn_id.identity[1] & 0x0F;
  etws_msg_ptr->plmn_info.mnc[0] = cb_etws_prim_ind_ptr->plmn_id.identity[2] & 0x0F;
  etws_msg_ptr->plmn_info.mnc[1] = (cb_etws_prim_ind_ptr->plmn_id.identity[2] & 0xF0) >> 4;
  etws_msg_ptr->plmn_info.mnc[2] = (cb_etws_prim_ind_ptr->plmn_id.identity[1] & 0xF0) >> 4;

  if(0x0F == etws_msg_ptr->plmn_info.mnc[2])
  {
    etws_msg_ptr->plmn_info.num_mnc_digits = 2;
  }
  else
  {
    etws_msg_ptr->plmn_info.num_mnc_digits = 3;
  }

  /* Serial Number */
  etws_msg_ptr->data[pos++] = (uint8)((cb_etws_prim_ind_ptr->serial_number & 0xFF00) >> 8);
  etws_msg_ptr->data[pos++] = (uint8)(cb_etws_prim_ind_ptr->serial_number & 0x00FF);

  /* Message Identifier */
  etws_msg_ptr->data[pos++] = (uint8)((cb_etws_prim_ind_ptr->message_id & 0xFF00) >> 8);
  etws_msg_ptr->data[pos++] = (uint8)(cb_etws_prim_ind_ptr->message_id & 0x00FF);

  /* Warning Type */
  etws_msg_ptr->data[pos++] = cb_etws_prim_ind_ptr->warning_type[0];
  etws_msg_ptr->data[pos++] = cb_etws_prim_ind_ptr->warning_type[1];

  etws_msg_ptr->data_len = pos;

  if(cb_etws_prim_ind_ptr->warning_security_info_present)
  {
    etws_msg_ptr->data_len += sizeof(cb_etws_prim_ind_ptr->warning_security_info);
    (void)memscpy( etws_msg_ptr->data + pos,
                   sizeof(cb_etws_prim_ind_ptr->warning_security_info),
                   cb_etws_prim_ind_ptr->warning_security_info,
                   sizeof(cb_etws_prim_ind_ptr->warning_security_info));
  }
  
#ifdef FEATURE_SMS_PWS_OVER_LTE
  // Do duplicate detection based on the NV - DISABLE_LTE_CB_DUP_DETECTION - value
  if (!cfg_s_ptr->lte_cb_dup_detection_is_disabled)
  {
    lte_rrc_etws_prim_ind_s     *lte_etws_prim_ptr = NULL;
    wms_gw_cb_geo_scope_e_type   geo_scope = cb_etws_prim_ind_ptr->serial_number >> 14; // Take the upper 2 bits 

    if (WMS_GW_CB_GEO_SCOPE_PLMN == geo_scope)
    {
      if ((lte_etws_prim_ptr = wms_mem_malloc(sizeof(lte_rrc_etws_prim_ind_s))) != NULL)
      {
        time_t                    search_time;
        wms_cell_info_s_type      curr_cell_info;

        memset( &curr_cell_info, 0, sizeof(wms_cell_info_s_type) );
        
        lte_etws_prim_ptr->msg_id      = cb_etws_prim_ind_ptr->message_id;
        lte_etws_prim_ptr->sr_num      = cb_etws_prim_ind_ptr->serial_number;
        lte_etws_prim_ptr->plmn.mcc[0] = etws_msg_ptr->plmn_info.mcc[0];
        lte_etws_prim_ptr->plmn.mcc[1] = etws_msg_ptr->plmn_info.mcc[1];
        lte_etws_prim_ptr->plmn.mcc[2] = etws_msg_ptr->plmn_info.mcc[2];
        lte_etws_prim_ptr->plmn.mnc[0] = etws_msg_ptr->plmn_info.mnc[0];
        lte_etws_prim_ptr->plmn.mnc[1] = etws_msg_ptr->plmn_info.mnc[1];
        lte_etws_prim_ptr->plmn.mnc[2] = etws_msg_ptr->plmn_info.mnc[2];
        lte_etws_prim_ptr->plmn.num_mnc_digits = etws_msg_ptr->plmn_info.num_mnc_digits;

        curr_cell_info.selected_plmn = lte_etws_prim_ptr->plmn;

        // Get timestamp of the duplicate search
        search_time = time_get_secs();

        // Check for duplicate message
        msg_is_dup = wms_bc_lte_message_is_duplicate( WMS_LTE_DUPLIST_PRIMARY,
                                                      (void *)lte_etws_prim_ptr,
                                                      search_time,
                                                      curr_cell_info );

        MSG_HIGH_3( "PriDupDet returns %d for msg_id 0x%04x with serial 0x%04x",
                    msg_is_dup,
                    lte_etws_prim_ptr->msg_id,
                    lte_etws_prim_ptr->sr_num );
        
        wms_mem_free( lte_etws_prim_ptr );
      }
    }
  } /* duplicate detection */
#endif /* FEATURE_SMS_PWS_OVER_LTE */

  if (FALSE == msg_is_dup)
  {
    (void)wms_msg_event_notify(WMS_MSG_EVENT_ETWS_NOTIFICATION, &msg_event_info);
  }

  /* done */
  return;
}

void wms_msg_etws_sec_notification_ind_proc
(
  cb_etws_sec_notification_ind_type *cb_etws_sec_ind_ptr
)
{
  uint8 pos = 0;
  boolean              msg_is_dup   = FALSE;
  wms_etws_msg_s_type *etws_msg_ptr = &(msg_event_info.etws_msg_info);

  MSG_HIGH_0("In wms_msg_etws_sec_notification_ind_proc()");

  etws_msg_ptr->type = WMS_ETWS_NOTIFICATION_SECONDARY_UMTS_FORMAT;
  etws_msg_ptr->plmn_info_present = TRUE;

  /* PLMN ID */
  /* sys_plmn.c */
  /**
   ** Define a structure that identifies a Public Land Mobile Network (PLMN).
   ** A PLMN ID is defined within a GSM network to be the combination of a Mobile
   ** Country Code (MCC) and Mobile Network Code (MNC).  A PLMN ID is stored in 3
   ** octets as specified in 3G TS 24.008 Section 10.5.1.3 and shown below:
   **                                        Bit
   **                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
   **                 |===============================================|
   **         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
   **                 |-----------------------------------------------|
   **         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
   **                 |-----------------------------------------------|
   **         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
   **                 |===============================================|
   **
   ** The criteria for comparing one PLMN against another is specified in
   ** 3G TS 23.122 Annex A.
   **/

  /**
      Table 10.5.3/3GPP TS 24.008: Location Area Identification
      information element

      MNC, Mobile network code (octet 3 bits 5 to 8, octet 4) The
      coding of this field is the responsibility of each
      administration but BCD coding shall be used. The MNC shall
      consist of 2 or 3 digits. For PCS 1900 for NA, Federal
      regulation mandates that a 3-digit MNC shall be used.
      However a network operator may decide to use only two digits
      in the MNC in the LAI over the radio interface. In this
      case, bits 5 to 8 of octet 3 shall be coded as "1111".
    **/
  etws_msg_ptr->plmn_info.mcc[0] = cb_etws_sec_ind_ptr->plmn_id.identity[0] & 0x0F;
  etws_msg_ptr->plmn_info.mcc[1] = (cb_etws_sec_ind_ptr->plmn_id.identity[0] & 0xF0) >> 4;
  etws_msg_ptr->plmn_info.mcc[2] = cb_etws_sec_ind_ptr->plmn_id.identity[1] & 0x0F;
  etws_msg_ptr->plmn_info.mnc[0] = cb_etws_sec_ind_ptr->plmn_id.identity[2] & 0x0F;
  etws_msg_ptr->plmn_info.mnc[1] = (cb_etws_sec_ind_ptr->plmn_id.identity[2] & 0xF0) >> 4;
  etws_msg_ptr->plmn_info.mnc[2] = (cb_etws_sec_ind_ptr->plmn_id.identity[1] & 0xF0) >> 4;

  if(0x0F == etws_msg_ptr->plmn_info.mnc[2])
  {
    etws_msg_ptr->plmn_info.num_mnc_digits = 2;
  }
  else
  {
    etws_msg_ptr->plmn_info.num_mnc_digits = 3;
  }

  /* Message Type */
  etws_msg_ptr->data[pos++] = 1; // CBS Message

  /* Messsage ID */
  etws_msg_ptr->data[pos++] = (uint8)((cb_etws_sec_ind_ptr->message_id & 0xFF00) >> 8);
  etws_msg_ptr->data[pos++] = (uint8)(cb_etws_sec_ind_ptr->message_id & 0x00FF);

  /* Serial Number */
  etws_msg_ptr->data[pos++] = (uint8)((cb_etws_sec_ind_ptr->serial_number & 0xFF00) >> 8);
  etws_msg_ptr->data[pos++] = (uint8)(cb_etws_sec_ind_ptr->serial_number & 0x00FF);

  /* Data Coding Scheme */
  etws_msg_ptr->data[pos++] = cb_etws_sec_ind_ptr->current_data_coding_scheme;

  /* CB Data */
  etws_msg_ptr->data[pos++] = cb_etws_sec_ind_ptr->number_of_pages;

  etws_msg_ptr->data_len = pos;

  if(0 != cb_etws_sec_ind_ptr->number_of_pages)
  {
    etws_msg_ptr->data_len += cb_etws_sec_ind_ptr->number_of_pages * 83; /* each page is 83 bytes long */
    etws_msg_ptr->data_len = MIN(etws_msg_ptr->data_len, WMS_ETWS_MAX_MSG_LEN);
    (void)memscpy( etws_msg_ptr->data + pos,
                   (etws_msg_ptr->data_len - 7),
                   cb_etws_sec_ind_ptr->page_data,
                   (etws_msg_ptr->data_len - 7) );
  }

#ifdef FEATURE_SMS_PWS_OVER_LTE
  // Do duplicate detection based on the NV - DISABLE_LTE_CB_DUP_DETECTION - value
  if (!cfg_s_ptr->lte_cb_dup_detection_is_disabled)
  { /* duplicate detection */
    lte_rrc_etws_sec_ind_s      *lte_etws_sec_ptr = NULL;
    wms_gw_cb_geo_scope_e_type   geo_scope = cb_etws_sec_ind_ptr->serial_number >> 14; // Take the upper 2 bits 

    if (WMS_GW_CB_GEO_SCOPE_PLMN == geo_scope)
    {
      if ((lte_etws_sec_ptr = wms_mem_malloc(sizeof(lte_rrc_etws_sec_ind_s))) != NULL)
      {
        time_t                    search_time;
        wms_cell_info_s_type      curr_cell_info;

        memset( &curr_cell_info, 0, sizeof(wms_cell_info_s_type) );
        
        lte_etws_sec_ptr->msg_id      = cb_etws_sec_ind_ptr->message_id;
        lte_etws_sec_ptr->sr_num      = cb_etws_sec_ind_ptr->serial_number;
        lte_etws_sec_ptr->plmn.mcc[0] = etws_msg_ptr->plmn_info.mcc[0];
        lte_etws_sec_ptr->plmn.mcc[1] = etws_msg_ptr->plmn_info.mcc[1];
        lte_etws_sec_ptr->plmn.mcc[2] = etws_msg_ptr->plmn_info.mcc[2];
        lte_etws_sec_ptr->plmn.mnc[0] = etws_msg_ptr->plmn_info.mnc[0];
        lte_etws_sec_ptr->plmn.mnc[1] = etws_msg_ptr->plmn_info.mnc[1];
        lte_etws_sec_ptr->plmn.mnc[2] = etws_msg_ptr->plmn_info.mnc[2];
        lte_etws_sec_ptr->plmn.num_mnc_digits = etws_msg_ptr->plmn_info.num_mnc_digits;

        curr_cell_info.selected_plmn = lte_etws_sec_ptr->plmn;
        
        // Get timestamp of the duplicate search
        search_time = time_get_secs();

        // Check for duplicate message
        msg_is_dup = wms_bc_lte_message_is_duplicate( WMS_LTE_DUPLIST_SECONDARY,
                                                      (void *)lte_etws_sec_ptr,
                                                      search_time,
                                                      curr_cell_info );

        MSG_HIGH_3( "SecDupDet returns %d for msg_id 0x%04x with serial 0x%04x",
                    msg_is_dup,
                    lte_etws_sec_ptr->msg_id,
                    lte_etws_sec_ptr->sr_num );
        
        wms_mem_free( lte_etws_sec_ptr );
      }
    }
  } /* duplicate detection */
#endif /* FEATURE_SMS_PWS_OVER_LTE */  

  if (FALSE == msg_is_dup)
  {
    (void)wms_msg_event_notify(WMS_MSG_EVENT_ETWS_NOTIFICATION, &msg_event_info);
  }

  /* done */
  return;
}
#endif /* defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS)*/
#endif /* FEATURE_GWSMS_BROADCAST */

/*=========================================================================
FUNCTION
  wms_msg_set_retry_period

DESCRIPTION
  Allow a client to specify the period of allowing retries of sms messages.
  NOTE: if period = 0 seconds, retry will not be attempted.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_set_retry_period
(
  wms_client_id_type              client_id,
  uint32                          period     /* in seconds */
)
{
  wms_status_e_type st = WMS_OK_S;

  wms_client_s_type *client_ptr;

  client_ptr = wms_client_ptr_fetch(client_id);

  if(client_ptr == NULL || client_ptr->activated == FALSE)
  {
    st = WMS_GENERAL_ERROR_S;
  }
  else if (period > WMS_MAX_RETRY_PERIOD)
  {
    st = WMS_INVALID_PARM_VALUE_S;
  }
  else if (NULL == msg_s_ptr)
  {
     MSG_ERROR_0("WMS Not Ready - msg_s_ptr is NULL");
     st = WMS_PHONE_NOT_READY_S;
  }
  else
  {

      nv_item_type                 nvi;
      msg_s_ptr->mo_retry_period = period;
      nvi.sms_mo_retry_period    = (uint8)period;

      if( NV_DONE_S != wms_nv_write_wait( NV_SMS_MO_RETRY_PERIOD_I, &nvi ) )
      {
        st = WMS_GENERAL_ERROR_S;
      }
      else
      {
        MSG_HIGH_2( "Set Retry Period = %d seconds by Client %d",
                    period,
                    client_id );
      }
  }

  WMS_CLIENT_PTR_RELEASEIF(client_ptr);

  return st;

} /* wms_msg_set_retry_period() */

/*=========================================================================
FUNCTION
  wms_msg_get_retry_period

DESCRIPTION
  Allow a client to retrieve the period of allowing retries of sms messages.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_get_retry_period
(
  wms_client_id_type              client_id,
  uint32                          * period     /* in seconds */
)
{
  wms_status_e_type st = WMS_OK_S;

  wms_client_s_type *client_ptr;

  client_ptr = wms_client_ptr_fetch(client_id);

  if (period == NULL)
  {
    MSG_HIGH_0("wms_msg_get_retry_period: NULL ptr");
    MSG_ERROR_0("period null pointer!");
    st = WMS_NULL_PTR_S;
  }
  else if(client_ptr == NULL || client_ptr->activated == FALSE)
  {
    st = WMS_GENERAL_ERROR_S;
  }
  else if (NULL == msg_s_ptr)
  {
     MSG_ERROR_0("WMS Not Ready - msg_s_ptr is NULL");
     st = WMS_PHONE_NOT_READY_S;
  }
  else
  {
    *period = msg_s_ptr->mo_retry_period;
  }

  WMS_CLIENT_PTR_RELEASEIF(client_ptr);

  return st;
}
/*=========================================================================
FUNCTION
  wms_set_retry_timer

DESCRIPTION
  This function Sets the Global Retry Timer to tick every second.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_set_retry_timer
(
  void
)
{
  if (rex_get_timer(&wms_retry_timer) == 0)
  {
    /* Set Timer for 1 second */
    (void) rex_set_timer(&wms_retry_timer, 1000);
  }
}

/*=========================================================================
FUNCTION
  wms_msg_process_retry_sig

DESCRIPTION
  This function processed the Retry Signal

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_process_retry_sig
(
  void
)
{
  int i;
  boolean set_timer = FALSE;

  MSG_HIGH_0( "got WMS_MO_RETRY_TIMER_SIG");
  (void) rex_clr_sigs( rex_self(), WMS_MO_RETRY_TIMER_SIG );

#ifdef FEATURE_CDSMS
  /* Go through all the pending CDMA MT Records
  ** and decrement retry_time_left and retry_wait_time
  ** Also check retry_wait_time becoming 0 to send message
  ** and retry_time_left becoming 0 to clear message
  */
  for(i=0; i< WMS_MAX_MT_MESSAGES; i++)
  {
    if ( NULL != msg_s_ptr->cdma_mt_rec[i] &&
         WMS_MSG_STATE_IDLE != msg_s_ptr->cdma_mt_rec[i]->state )
    {
      /* Decrement Timers */
      if (msg_s_ptr->cdma_mt_rec[i]->retry_time_left > 0)
      {
        if ( (WMS_MSG_STATE_QUEUED == msg_s_ptr->cdma_mt_rec[i]->state) &&
             ( 0 < msg_s_ptr->cdma_mt_rec[i]->retry_wait_time) )
        {
          msg_s_ptr->cdma_mt_rec[i]->retry_wait_time -= 1;

          if (msg_s_ptr->cdma_mt_rec[i]->retry_wait_time == 0)
          {
            wms_status_e_type     st;

            /* Send the Ack Again */
            st = wms_msg_cdma_ack_msg_i( msg_s_ptr->cdma_mt_rec[i],
                                         msg_s_ptr->cdma_mt_rec[i]->error_class,
                                         msg_s_ptr->cdma_mt_rec[i]->tl_status );

            if( st == WMS_OK_S )
            {
              MSG_HIGH_0("ack msg: L2 timer set");
              msg_s_ptr->cdma_mt_rec[i]->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
            }
            else if( st == WMS_NETWORK_NOT_READY_S )
            {
              msg_s_ptr->cdma_mt_rec[i]->retry_wait_time = 1; /* Retry after 1 second */
              msg_s_ptr->cdma_mt_rec[i]->state           = WMS_MSG_STATE_QUEUED;
            }
            else /* other error */
            {
              MSG_HIGH_0("ack msg error");

              if(msg_s_ptr->cdma_mt_rec[i]->client_id != WMS_CLIENT_TYPE_INTERNAL)
              {
                msg_event_info.ack_report_info.client_id = msg_s_ptr->cdma_mt_rec[i]->client_id;
                msg_event_info.ack_report_info.transaction_id =
                  (wms_transaction_id_type)msg_s_ptr->cdma_mt_rec[i]->user_data;
                msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
                msg_event_info.ack_report_info.user_data = msg_s_ptr->cdma_mt_rec[i]->mt_rec_user_data;
                msg_event_info.ack_report_info.is_success = FALSE;
                if ( st == WMS_1X_AND_VOICE_NOT_ALLOWED_S )
                {
                  msg_event_info.ack_report_info.error_status = WMS_RPT_1X_AND_VOICE_NOT_ALLOWED;
                }
                else
                {
                  msg_event_info.ack_report_info.error_status = WMS_RPT_GENERAL_ERROR;
                }

                (void)wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
              }

              wms_msg_cdma_free_mt_msg_rec( msg_s_ptr->cdma_mt_rec[i] );
              continue;
            } /* if st - else */
          }
        }

        msg_s_ptr->cdma_mt_rec[i]->retry_time_left -= 1;

        /* Check if retry_wait_time expires */
        if (msg_s_ptr->cdma_mt_rec[i]->retry_time_left == 0)
        {
          if(msg_s_ptr->cdma_mt_rec[i]->client_id != WMS_CLIENT_TYPE_INTERNAL)
          {
            msg_event_info.ack_report_info.client_id = msg_s_ptr->cdma_mt_rec[i]->client_id;
            msg_event_info.ack_report_info.transaction_id =
              (wms_transaction_id_type)msg_s_ptr->cdma_mt_rec[i]->user_data;
            msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
            msg_event_info.ack_report_info.user_data = msg_s_ptr->cdma_mt_rec[i]->mt_rec_user_data;
            msg_event_info.ack_report_info.is_success = FALSE;
            msg_event_info.ack_report_info.error_status = WMS_RPT_GENERAL_ERROR;

            (void)wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
          }

          wms_msg_cdma_free_mt_msg_rec(msg_s_ptr->cdma_mt_rec[i]);
          continue;
        }
      }
      /* Set timer in use */
      set_timer = TRUE;
    }
  }

  /* Go through all the pending CDMA MO Records
  ** and decrement retry_time_left and retry_wait_time
  ** Also check retry_wait_time becoming 0 to send message
  ** and retry_time_left becoming 0 to clear message
  */
  {
    wms_msg_cdma_message_record_type *cdma_mo_rec_ptr = msg_s_ptr->cdma_mo_rec.head_ptr;
    while ( NULL != cdma_mo_rec_ptr )
    {
      MSG_HIGH_3( "client_id = %d, ch_pref = %d, transport_type = %d",
                  cdma_mo_rec_ptr->client_id,
                  cdma_mo_rec_ptr->ch_pref,
                  cdma_mo_rec_ptr->transport_type );

      MSG_HIGH_3( "cdma_mo_rec = 0x%x, state = %d, retry_wait_time = %d",
                  cdma_mo_rec_ptr,
                  cdma_mo_rec_ptr->state,
                  cdma_mo_rec_ptr->retry_wait_time );

      MSG_HIGH_2( "retry_time_left = %d, seq_num = %d",
                  cdma_mo_rec_ptr->retry_time_left,
                  cdma_mo_rec_ptr->seq_num );

      if ( WMS_MSG_STATE_IDLE != cdma_mo_rec_ptr->state )
      {
        /* Decrement Timers */
        if (cdma_mo_rec_ptr->retry_time_left > 0)
        {
          if ((WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == cdma_mo_rec_ptr->transport_type) &&
              (WMS_MSG_STATE_QUEUED == cdma_mo_rec_ptr->state))
          {
            wms_cm_s_type             *cm_s_ptr     = wms_cm_s_ptr();
            sys_modem_as_id_e_type     wms_1x_as_id = wms_get_appinfo_1x_asid();

            if ((CM_CALL_TYPE_VOICE == cm_s_ptr->call_info.call_type)   &&
                (CM_CALL_STATE_IDLE != cm_s_ptr->call_info.call_state)  &&
                (cm_s_ptr->call_info.asubs_id == wms_1x_as_id)          &&
                wms_cfg_check_1xsrlte_or_ehrpd_oper_mode(wms_1x_as_id))
            {
              MSG_HIGH_1( "Can't send SMS over IMS while in 1xSRLTE mode, call_state = %d",
                          cm_s_ptr->call_info.call_state ); 
            
              cdma_mo_rec_ptr->retry_time_left = (sint31)1;
              cdma_mo_rec_ptr->retry_wait_time = (sint31)0;
              cdma_mo_rec_ptr->prev_report.report_status = WMS_RPT_IMS_NOT_ALLOWED_IN_SRLTE;
            }
          }

          /* Reject DAN SMS attempt from IMS - since IMS request to send DAN SMS on traffic channel will cause CM to
          abort LTE and move to 1X. */
          if ( ( WMS_CLIENT_CH_PREF_TCH_ONLY             == cdma_mo_rec_ptr->ch_pref )   &&
               ( (wms_client_id_type)WMS_CLIENT_TYPE_IMS == cdma_mo_rec_ptr->client_id ) &&
               ( WMS_MSG_STATE_QUEUED                    == cdma_mo_rec_ptr->state )     &&
               wms_cfg_check_hvolte_oper_mode() )
          {
             MSG_ERROR_0("Can't send DAN SMS over 1X in hVoLTE mode");
             cdma_mo_rec_ptr->retry_time_left = (sint31)1;
             cdma_mo_rec_ptr->retry_wait_time = (sint31)0;
             cdma_mo_rec_ptr->prev_report.report_status = WMS_RPT_TRANSPORT_NOT_READY;
          }
          
          if ( (cdma_mo_rec_ptr->state == WMS_MSG_STATE_QUEUED) &&
               (cdma_mo_rec_ptr->retry_wait_time > 0) )
          {
            cdma_mo_rec_ptr->retry_wait_time -= 1;

            if (cdma_mo_rec_ptr->retry_wait_time == 0)
            {
              wms_status_e_type     st;

              if ( (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102 == cdma_mo_rec_ptr->transport_type) &&
                   (!cfg_s_ptr->active_bearers[WMS_BEARER_LTE] || !cfg_s_ptr->sms_over_s102) )
              {
                cdma_mo_rec_ptr->transport_type = WMS_MSG_TRANSPORT_TYPE_CS;
              }

              if ( (WMS_MSG_TRANSPORT_TYPE_CS == cdma_mo_rec_ptr->transport_type) &&
                   (!dc_s_ptr->call_active && !dc_s_ptr->is_voice_data_call_conn) &&
                   (WMS_CLIENT_CH_PREF_TCH_ONLY == cdma_mo_rec_ptr->ch_pref) )
              {
                /* Send on Traffic Channel */
                MSG_HIGH_1("Retry cdma msg rec 0x%x on Traffic Channel", cdma_mo_rec_ptr);

                if (!dc_s_ptr->wms_initiated_call && WMS_DUMMY_CALL_ID == dc_s_ptr->dc_call_id)
                {
                   MSG_HIGH_0("WMS is initiating SMS call. Wait for the call to connect.");
                   st = wms_dc_connect( (wms_client_id_type) WMS_CLIENT_TYPE_INTERNAL,
                                        NULL,
                                        NULL,
                                        cdma_mo_rec_ptr->dc_so );
                   if (st == WMS_OK_S)
                   {
                     dc_s_ptr->wms_initiated_call = TRUE;
                   }
                }
                else
                {
                  MSG_HIGH_0("SMS call has been originated. Wait for the call to connect.");
                  st = WMS_OK_S;
                }

                if (st == WMS_OK_S)
                {
                  /* Wait for the remaining of RETRY Period to hear back from CM */
                  cdma_mo_rec_ptr->retry_wait_time = (sint31)0;
                  st = WMS_ACCESS_TOO_LARGE_S;
                }
              }
              else
              {
                // If the message is not being sent over IMS,
                // attempt retry over other transports (S102 is preferred)
                if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS != cdma_mo_rec_ptr->transport_type)
                {
                  if (cfg_s_ptr->active_bearers[WMS_BEARER_LTE] && cfg_s_ptr->sms_over_s102)
                  {
                    MSG_HIGH_1("Retrying cdma msg rec 0x%x over S102", cdma_mo_rec_ptr);
                    cdma_mo_rec_ptr->transport_type = WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102;
                  }
                  else
                  {
                    MSG_HIGH_2( "Retrying cdma msg rec 0x%x on %d channel",
                                 cdma_mo_rec_ptr, cdma_mo_rec_ptr->ch_pref );
                    cdma_mo_rec_ptr->transport_type = WMS_MSG_TRANSPORT_TYPE_CS;
                  }
                }

                /* Send the Message Again */
                st = wms_msg_cdma_send_sms_OTA( FALSE,   /* not a TL ack */
                                                cdma_mo_rec_ptr,
                                                cdma_mo_rec_ptr->ota,
                                                &(cdma_mo_rec_ptr->address) );

                // Send MO SMS Retry Attempt Event
                wms_msg_evt_mo_sms_retry_attempt( (uint32)cdma_mo_rec_ptr->user_data,
                                                  (uint32)cdma_mo_rec_ptr,
                                                  (uint8)cdma_mo_rec_ptr->prev_report.report_status,
                                                  (uint8)cdma_mo_rec_ptr->prev_report.cause_info.tl_status,
                                                  (uint32)cdma_mo_rec_ptr->retry_time_left );
              }

              if (st == WMS_OK_S)
              {
                /* update the msg state of the rec_ptr */
                cdma_mo_rec_ptr->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
              }
              else if( st == WMS_NETWORK_NOT_READY_S )
              {
                /* Set the retry_wait_time to 1 sec for none L2 status/error indication */
                cdma_mo_rec_ptr->retry_wait_time = (sint31)1;
                MSG_HIGH_1("L2/NW is busy. Try again in %d secs", cdma_mo_rec_ptr->retry_wait_time);
              }
              else if( st == WMS_PHONE_NOT_READY_S )
              {
                /* try again in one second for no bearer service */
                cdma_mo_rec_ptr->retry_wait_time = (sint31)1;
                cdma_mo_rec_ptr->state = WMS_MSG_STATE_QUEUED;

                MSG_HIGH_1("No bearer data service. Try again in %d secs", cdma_mo_rec_ptr->retry_wait_time);
              }
              else if( st == WMS_1X_AND_VOICE_NOT_ALLOWED_S )
              {
                MSG_HIGH_0("1X and voice not allowed. No more retry.");

                msg_event_info.submit_report_info = cdma_mo_rec_ptr->prev_report;
                msg_event_info.submit_report_info.report_status = WMS_RPT_1X_AND_VOICE_NOT_ALLOWED;

                if( cdma_mo_rec_ptr->send_status_to_clients == TRUE )
                {
                  msg_event_info.submit_report_info.delivery_failure_permanent = TRUE;
                 (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT,
                                            &msg_event_info);
                 {
                    if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == cdma_mo_rec_ptr->transport_type ||
                        WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102 == cdma_mo_rec_ptr->transport_type)
                    {		 
                      wms_prov_app_info_s_type *appInfo_ptr = NULL;

                      appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

                      if (NULL != appInfo_ptr)
                      {
                        appInfo_ptr->mo_sms_count--;
                        if (0 == appInfo_ptr->mo_sms_count)
                        {
                          wms_mm_sms_call_info_T  mm_status;

                          memset(&mm_status, 0x00, sizeof(wms_mm_sms_call_info_T));

                          mm_status.as_id = msg_event_info.submit_report_info.as_id;

                          (void) wms_cm_gw_send_sms_status_to_mm(mm_status);
                        }
                      }
                    }
                  }
                }

                /* Clear the MO record. */
                cdma_mo_rec_ptr = wms_msg_cdma_get_next_mo_rec_before_delete( cdma_mo_rec_ptr );

                if ( wms_msg_suspend_ps_call_is_supported() )
                {
                  wms_cm_resume_1x_pscall(wms_get_appinfo_1x_asid());
                }

                continue;
              }
              else if ( st == WMS_TRANSPORT_NOT_READY_S || st == WMS_TRANSPORT_NOT_ALLOWED_S )
              {
                MSG_HIGH_0("Transport not ready/allowed.");
                msg_event_info.submit_report_info = cdma_mo_rec_ptr->prev_report;
                if (WMS_TRANSPORT_NOT_READY_S == st)
                {
                  msg_event_info.submit_report_info.report_status = WMS_RPT_TRANSPORT_NOT_READY;
                }
                else if (WMS_TRANSPORT_NOT_ALLOWED_S == st)
                {
                  msg_event_info.submit_report_info.report_status = WMS_RPT_TRANSPORT_NOT_ALLOWED;
                }

                if( cdma_mo_rec_ptr->send_status_to_clients == TRUE )
                {
                  // Set delivery_failure_permanent as FALSE so that APP may trigger retry over 1X.                  
                  msg_event_info.submit_report_info.delivery_failure_permanent = FALSE;

                  /* Handling special case for hvolte: Send permanent error when
                     the MO SMS requested over IMS fails.
                     In HVOLTE, IMS is registered for both VoLTE and SMS. So, 1X bearer is not
                     available. App client shouldnt retry on 1X. */
                  if ( (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == cdma_mo_rec_ptr->transport_type) &&
                        wms_cfg_check_hvolte_oper_mode() )
                  {
                    MSG_HIGH_0("Reporting permanent failure for SMS over IMS in hVOLTE mode");
                    msg_event_info.submit_report_info.delivery_failure_permanent = TRUE;
                  }
    
                  (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT,
                                             &msg_event_info);

                  {
                    if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == cdma_mo_rec_ptr->transport_type ||
                        WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102 == cdma_mo_rec_ptr->transport_type)
                    {		 
                      wms_prov_app_info_s_type *appInfo_ptr = NULL;

                      appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

                      if (NULL != appInfo_ptr)
                      {
                        appInfo_ptr->mo_sms_count--;
                        if (0 == appInfo_ptr->mo_sms_count)
                        {
                          wms_mm_sms_call_info_T  mm_status;

                          memset(&mm_status, 0x00, sizeof(wms_mm_sms_call_info_T));

                          mm_status.as_id = msg_event_info.submit_report_info.as_id;

                          (void) wms_cm_gw_send_sms_status_to_mm(mm_status);
                        }
                      }
                    }
                  }
                }
                /* Clear the MO record. */
                cdma_mo_rec_ptr = wms_msg_cdma_get_next_mo_rec_before_delete( cdma_mo_rec_ptr );
                continue;
              }
            }
          }
		  else if ((cdma_mo_rec_ptr->state == WMS_MSG_STATE_QUEUED) &&
                   (cdma_mo_rec_ptr->retry_wait_time == 0))
		  {
              if ( (WMS_MSG_TRANSPORT_TYPE_CS == cdma_mo_rec_ptr->transport_type) &&
                   (!dc_s_ptr->call_active && !dc_s_ptr->is_voice_data_call_conn) &&
                   (WMS_CLIENT_CH_PREF_TCH_ONLY == cdma_mo_rec_ptr->ch_pref) &&
                   (!dc_s_ptr->wms_initiated_call))
              {
                msg_event_info.submit_report_info = cdma_mo_rec_ptr->prev_report;
				msg_event_info.submit_report_info.report_status = WMS_RPT_PHONE_NOT_READY;
				msg_event_info.submit_report_info.delivery_failure_permanent = FALSE; 
				(void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT,
                                       &msg_event_info);
                
				cdma_mo_rec_ptr = wms_msg_cdma_get_next_mo_rec_before_delete( cdma_mo_rec_ptr );
                
                if ( wms_msg_suspend_ps_call_is_supported() )
                {
                  wms_cm_resume_1x_pscall(wms_get_appinfo_1x_asid());
                }
                continue;
			  }
		  }
        }

        cdma_mo_rec_ptr->retry_time_left -= 1;
        /* Check if retry_wait_time expires */
        if (cdma_mo_rec_ptr->retry_time_left == 0)
        {
          MSG_HIGH_0("retry_time_left has expired.");
          msg_event_info.submit_report_info = cdma_mo_rec_ptr->prev_report;

          if ( cdma_mo_rec_ptr->state == WMS_MSG_STATE_WAIT_FOR_ACK )
          {
            msg_event_info.submit_report_info.report_status = WMS_RPT_NO_ACK;
          }

          if ( cdma_mo_rec_ptr->send_status_to_clients )
          {
            /* Handling special case for MO SMS going over IMS: Send permanent error when
               the MO SMS has been sent to IMS and IMS doesn't respond with status within
               the retry time period. */
            if ( (WMS_MSG_STATE_LAYER2_IN_PROGRESS == cdma_mo_rec_ptr->state) &&
                 (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == cdma_mo_rec_ptr->transport_type) )
            {
              msg_event_info.submit_report_info.delivery_failure_permanent = TRUE;
            }
            else
            {
              msg_event_info.submit_report_info.delivery_failure_permanent = FALSE; 
            }

            (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT,
                                       &msg_event_info);

            {
                if (WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == cdma_mo_rec_ptr->transport_type ||
                    WMS_MSG_TRANSPORT_TYPE_SMS_OVER_S102 == cdma_mo_rec_ptr->transport_type)
                {		 
                  wms_prov_app_info_s_type *appInfo_ptr = NULL;

                  appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

                  if (NULL != appInfo_ptr)
                  {
                    appInfo_ptr->mo_sms_count--;
                    if (0 == appInfo_ptr->mo_sms_count)
                    {
                      wms_mm_sms_call_info_T  mm_status;

                      memset(&mm_status, 0x00, sizeof(wms_mm_sms_call_info_T));

                      mm_status.as_id = msg_event_info.submit_report_info.as_id;

                      (void) wms_cm_gw_send_sms_status_to_mm(mm_status);
                    }
                  }
                }
              }
          }

          /* Clear the MO record. */
          cdma_mo_rec_ptr = wms_msg_cdma_get_next_mo_rec_before_delete( cdma_mo_rec_ptr );

          if ( wms_msg_suspend_ps_call_is_supported() )
          {
            wms_cm_resume_1x_pscall(wms_get_appinfo_1x_asid());
          }

          continue;
        }
      }

      cdma_mo_rec_ptr = cdma_mo_rec_ptr->next;
      /* Set timer in use */
      set_timer = TRUE;
    } /* end while() */
  }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
  /* Go through all the pending GW MO Records
  ** and decrement retry_time_left and retry_wait_time
  ** Also check retry_wait_time becoming 0 to send message
  ** and retry_time_left becoming 0 to clear message
  */
  for(i=0; i< WMS_MAX_MO_MESSAGES; i++)
  {
    if (NULL != msg_s_ptr->gw_mo_rec[i])
    {
      /* Decrement Timers */
      if (msg_s_ptr->gw_mo_rec[i]->retry_time_left > 0)
      {
        if (msg_s_ptr->gw_mo_rec[i]->retry_wait_time > 0)
        {
          msg_s_ptr->gw_mo_rec[i]->retry_wait_time -= 1;

          /* Check if retry_wait_time expires */
          if (msg_s_ptr->gw_mo_rec[i]->retry_wait_time == 0)
          {
            if (WMS_MSG_SMMA_REQUEST_ID == msg_s_ptr->gw_mo_rec[i]->smi)
            {
              wms_msg_notify_SMMA(msg_s_ptr->gw_mo_rec[i]->appInfo_ptr, TRUE);
            }
            else
            {
              wms_cmd_err_e_type  cmd_err = WMS_CMD_ERR_NONE;

              cmd_err = wms_msg_send_gw((uint32)i, NULL, TRUE);
              MSG_HIGH_1("wms_msg_sent_gw for Record %d", i);

              // Send MO SMS Retry Attempt Event
              wms_msg_evt_mo_sms_retry_attempt( (uint32)msg_s_ptr->gw_mo_rec[i]->user_data,
                                                (uint32)i,
                                                (uint8)msg_s_ptr->gw_mo_rec[i]->prev_report.report_status,
                                                (uint8)msg_s_ptr->gw_mo_rec[i]->prev_report.cause_info.cause_value,
                                                (uint32)msg_s_ptr->gw_mo_rec[i]->retry_time_left );

              if(cmd_err != WMS_CMD_ERR_NONE)
              {
                msg_event_info.submit_report_info = msg_s_ptr->gw_mo_rec[i]->prev_report;

                if (WMS_CMD_ERR_TRANSPORT_NOT_READY == cmd_err &&
                    WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == msg_s_ptr->gw_mo_rec[i]->transport_type)
                { 
                  // Send temporary error to APP so that they may retry the SMS over CS.                   
                  msg_event_info.submit_report_info.delivery_failure_permanent = FALSE;
                }
                else
                {
                  /* This should not happen for retry. The raw ts data must be incorrect
                  ** when trying to decode to get the message number.
                  ** This is a permanent error so send the submit report with the info
                  ** of the previous error.
                  */
                  msg_event_info.submit_report_info.delivery_failure_permanent = TRUE;
                }

                /* Send the event to clients
                */
                (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT,
                                           &msg_event_info);
                 
                {
                    wms_prov_app_info_s_type *appInfo_ptr = NULL;

                    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(msg_s_ptr->gw_mo_rec[i]->as_id, WMS_MESSAGE_MODE_GW);

                    if (NULL != appInfo_ptr)
                    {
                      appInfo_ptr->mo_sms_count--;
                      if (0 == appInfo_ptr->mo_sms_count)
                      {
                        wms_mm_sms_call_info_T  mm_status;

                        memset(&mm_status, 0x00, sizeof(wms_mm_sms_call_info_T));

                        mm_status.as_id = msg_event_info.submit_report_info.as_id;

                        (void) wms_cm_gw_send_sms_status_to_mm(mm_status);
                      }

                    }
                }

                {
                  sys_modem_as_id_e_type as_id = msg_s_ptr->gw_mo_rec[i]->as_id;

                  /* Clear the msg record
                  */
                  wms_msg_gw_clear_mo_msg_rec((uint32)i);

                  if ( wms_msg_suspend_ps_call_is_supported() )
                  {
                    wms_cm_resume_gw_pscall(as_id);
                  }
                }
                continue;
              }
            }
          }
        }

        if (NULL == msg_s_ptr->gw_mo_rec[i])
        {
          continue;
        }

        msg_s_ptr->gw_mo_rec[i]->retry_time_left -= 1;

        /* Check if retry_time_left expires */
        if (msg_s_ptr->gw_mo_rec[i]->retry_time_left == 0)
        {
          if (WMS_MSG_SMMA_REQUEST_ID == msg_s_ptr->gw_mo_rec[i]->smi)
          {
            /* if no SMMA report indication received by now, give it another chance */
            if ((0 == msg_s_ptr->gw_mo_rec[i]->retry_wait_time) &&
                (msg_s_ptr->gw_mo_rec[i]->retry_count < WMS_MAX_RETRY_COUNT))
            {
              msg_s_ptr->gw_mo_rec[i]->retry_count++;
              msg_s_ptr->gw_mo_rec[i]->retry_time_left = (sint31)msg_s_ptr->mo_retry_period;

              if ( 0 == msg_s_ptr->mo_retry_period ) 
              {
                msg_s_ptr->gw_mo_rec[i]->retry_time_left = WMS_MAX_MO_STATUS_WAIT_TIME;
              }
              
              set_timer = TRUE;
              MSG_HIGH_0("No SMMA report indication, retry time extended");
              continue;
            }
            else
            {
              MSG_ERROR_0("SMMA retry ended");
              // If these repeat procedures fail, the mobile shall unset the memory capacity exceeded notification flag 
              // in the (U)SIM and exit this procedure.
              wms_msg_clear_sim_mem_full_flag( msg_s_ptr->gw_mo_rec[i]->appInfo_ptr );
            }
          }
          else
          {
            boolean gcf_flag = FALSE;
            msg_event_info.submit_report_info = msg_s_ptr->gw_mo_rec[i]->prev_report;

            /* Send the event to clients
            */
            (void)wms_nv_read_gcf_flag(&gcf_flag);

            MSG_HIGH_1("Retry Period Expired, GCF Flag for Retry = %d", gcf_flag);

            if (TRUE == gcf_flag)
            {
              MSG_HIGH_0("GCF Flag is Set, set delivery failure as permanent");
              msg_event_info.submit_report_info.delivery_failure_permanent = TRUE;
            }
            else
            {
              msg_event_info.submit_report_info.delivery_failure_permanent = FALSE;
            }

            (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT,
                                       &msg_event_info);
            {

                wms_prov_app_info_s_type *appInfo_ptr = NULL;

                appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(msg_event_info.submit_report_info.as_id, WMS_MESSAGE_MODE_GW);

                if (NULL != appInfo_ptr)
                {
                  appInfo_ptr->mo_sms_count--;
                  if (0 == appInfo_ptr->mo_sms_count)
                  {
                    wms_mm_sms_call_info_T  mm_status;

                    memset(&mm_status, 0x00, sizeof(wms_mm_sms_call_info_T));

                    mm_status.as_id = msg_event_info.submit_report_info.as_id;

                    (void) wms_cm_gw_send_sms_status_to_mm(mm_status);
                  }
                }
            }
          }
          {
            sys_modem_as_id_e_type as_id = msg_s_ptr->gw_mo_rec[i]->as_id;

            /* Clear the msg record
            */
            wms_msg_gw_clear_mo_msg_rec((uint32)i);

            if ( wms_msg_suspend_ps_call_is_supported() )
            {
              wms_cm_resume_gw_pscall(as_id);
            }
          }
          continue;
        }
      }
      /* Set timer in use */
      set_timer = TRUE;
    }
  }
#endif /* FEATURE_GWSMS */

  if (set_timer == TRUE)
  {
    wms_set_retry_timer();
  }
}

void wms_msg_offline(
  void
)
{
  uint32    i;
  wms_cfg_s_type *l_cfg_s_ptr = NULL;

  // Simple function doing cleanup
  // Currently only called when phone goes into LPM
  // If you are changing this, check for any side effects
  // it might cause to LPM when phone wakes up and goes online!

  msg_s_ptr = wms_msg_s_ptr();

   l_cfg_s_ptr = wms_cfg_s_ptr();

  if (NULL != l_cfg_s_ptr)
  {

    sys_modem_as_id_e_type as_id_arr[MAX_AS_IDS+1];
    int count =0;

    memset(as_id_arr, SYS_MODEM_AS_ID_NONE, sizeof(as_id_arr));

    MSG_HIGH_0("Resetting MO SMS count in appInfo pointers");

    if (NULL != l_cfg_s_ptr->wms_app0_info)
    {
      if (0 != l_cfg_s_ptr->wms_app0_info->mo_sms_count)
      {
        as_id_arr[count++] = l_cfg_s_ptr->wms_app0_info->asid;		
      }

      l_cfg_s_ptr->wms_app0_info->mo_sms_count = 0;
    }
    if (NULL != l_cfg_s_ptr->wms_app1_info)
    {
      if (0 != l_cfg_s_ptr->wms_app1_info->mo_sms_count)
      {
        as_id_arr[count++] = l_cfg_s_ptr->wms_app1_info->asid;		
      }

      l_cfg_s_ptr->wms_app1_info->mo_sms_count = 0;
    }
    if (NULL != l_cfg_s_ptr->wms_app2_info)
    {
      if (0 != l_cfg_s_ptr->wms_app2_info->mo_sms_count)
      {
        as_id_arr[count++] = l_cfg_s_ptr->wms_app2_info->asid;		
      }

      l_cfg_s_ptr->wms_app2_info->mo_sms_count = 0;
    }

#if defined(FEATURE_TRIPLE_SIM)
    if (NULL != l_cfg_s_ptr->wms_app3_info)
    {
      if (0 != l_cfg_s_ptr->wms_app3_info->mo_sms_count)
      {
        as_id_arr[count++] = l_cfg_s_ptr->wms_app3_info->asid;		
      }

      l_cfg_s_ptr->wms_app3_info->mo_sms_count = 0;
    }
#endif


    for (i = 0; i < count; i++)
    {
      if (SYS_MODEM_AS_ID_NONE != as_id_arr[i])
      {
        wms_mm_sms_call_info_T  mm_status;

        memset(&mm_status, 0x00, sizeof(wms_mm_sms_call_info_T));

        mm_status.as_id = as_id_arr[i];

        (void) wms_cm_gw_send_sms_status_to_mm(mm_status);
      }
    }
  }

  (void) rex_clr_timer(&wms_retry_timer);

#ifdef FEATURE_CDSMS
  for ( i = 0; i < WMS_MAX_MT_MESSAGES; i++ )
  {
    if ( NULL != msg_s_ptr->cdma_mt_rec[ i ] )
    {
      if ( ( WMS_CLIENT_TYPE_INTERNAL != msg_s_ptr->cdma_mt_rec[ i ]->client_id ) &&
           ( WMS_MSG_STATE_WAIT_FOR_ACK != msg_s_ptr->cdma_mt_rec[ i ]->state ) )
      {
        msg_event_info.ack_report_info.client_id = msg_s_ptr->cdma_mt_rec[ i ]->client_id;
        msg_event_info.ack_report_info.transaction_id  = (wms_transaction_id_type)msg_s_ptr->cdma_mt_rec[ i ]->user_data;
        msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
        msg_event_info.ack_report_info.user_data = msg_s_ptr->cdma_mt_rec[ i ]->mt_rec_user_data;
        msg_event_info.ack_report_info.is_success = TRUE;
        msg_event_info.ack_report_info.error_status = WMS_RPT_OK;
        (void)wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
      }
      wms_msg_cdma_free_mt_msg_rec( msg_s_ptr->cdma_mt_rec[ i ] );
    }
  }

  {
    wms_msg_cdma_message_record_type *cdma_mo_rec_ptr = msg_s_ptr->cdma_mo_rec.head_ptr;

    while ( NULL != cdma_mo_rec_ptr )
    {
      /* Submit report to clients if needed */
      if ( cdma_mo_rec_ptr->send_status_to_clients )
      {
        msg_event_info.submit_report_info.report_status = WMS_RPT_NETWORK_NOT_READY;
        msg_event_info.submit_report_info.cause_info.error_class = WMS_ERROR_PERM;
        msg_event_info.submit_report_info.user_data = cdma_mo_rec_ptr->user_data;
        msg_event_info.submit_report_info.client_id = cdma_mo_rec_ptr->client_id;
        msg_event_info.submit_report_info.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
        msg_event_info.submit_report_info.alpha_id.len = 0;
        msg_event_info.submit_report_info.alpha_id.data = NULL;
        msg_event_info.submit_report_info.message_mode= WMS_MESSAGE_MODE_CDMA;
        msg_event_info.submit_report_info.delivery_failure_permanent = FALSE;
       (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT, &msg_event_info);
      }
      /* Clear the MO record. */
      cdma_mo_rec_ptr = wms_msg_cdma_get_next_mo_rec_before_delete( cdma_mo_rec_ptr );
    }
  }

  if ( wms_msg_suspend_ps_call_is_supported() )
  {
    wms_cm_resume_1x_pscall(wms_get_appinfo_1x_asid());
  }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
  for ( i = 0; i < WMS_MAX_MT_MESSAGES; i++ )
  {
    if ( NULL != msg_s_ptr->gw_mt_rec[ i ] )
    {
      if ( ( WMS_CLIENT_TYPE_INTERNAL != (wms_client_type_e_type)( msg_s_ptr->gw_mt_rec[i]->ack_client_id) ) &&
           ( WMS_MSG_STATE_WAIT_FOR_ACK != msg_s_ptr->gw_mt_rec[ i ]->state ) )
       {
          msg_event_info.ack_report_info.client_id = (wms_client_type_e_type)(msg_s_ptr->gw_mt_rec[i]->ack_client_id);
          msg_event_info.ack_report_info.transaction_id = (wms_transaction_id_type)msg_s_ptr->gw_mt_rec[ i ]->tid;
          msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_GW;
          msg_event_info.ack_report_info.user_data = msg_s_ptr->gw_mt_rec[ i ]->user_data;
          msg_event_info.ack_report_info.is_success = TRUE;
          msg_event_info.ack_report_info.as_id = msg_s_ptr->gw_mt_rec[ i ]->as_id;
          msg_event_info.ack_report_info.error_status = WMS_RPT_OK;
          (void)wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
      }
      wms_msg_release_mt_record(i);
    }
  }

  for ( i = 0; i < WMS_MAX_MO_MESSAGES; i++ )
  {
    if ( NULL != msg_s_ptr->gw_mo_rec[ i ] )
    {
      /* Submit report to client then clear msg */
      msg_event_info.submit_report_info.report_status = WMS_RPT_NETWORK_NOT_READY;
      msg_event_info.submit_report_info.cause_info.error_class = WMS_ERROR_PERM;
      msg_event_info.submit_report_info.user_data = msg_s_ptr->gw_mo_rec[i]->user_data;
      msg_event_info.submit_report_info.client_id = msg_s_ptr->gw_mo_rec[i]->client_id;
      msg_event_info.submit_report_info.mo_ctrl_status = WMS_MO_CTRL_STATUS_NO_MOD;
      msg_event_info.submit_report_info.alpha_id.len = 0;
      msg_event_info.submit_report_info.alpha_id.data = NULL;
      msg_event_info.submit_report_info.as_id = msg_s_ptr->gw_mo_rec[i]->as_id;
      msg_event_info.submit_report_info.message_mode= WMS_MESSAGE_MODE_GW;
      msg_event_info.submit_report_info.delivery_failure_permanent = FALSE;
      (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT, &msg_event_info);
      {
        sys_modem_as_id_e_type as_id = msg_s_ptr->gw_mo_rec[i]->as_id;

        wms_msg_gw_clear_mo_msg_rec( i );

        if ( wms_msg_suspend_ps_call_is_supported() )
        {
          wms_cm_resume_gw_pscall(as_id);
        }
      }
    }
  }
#endif /* FEATURE_GWSMS */

} /* wms_msg_offline() */

#ifdef FEATURE_GWSMS
/*===========================================================================

FUNCTION wms_msg_fill_sc_address

DESCRIPTION
  This local function fills in sc address from template in Card/NV.
  SC address is needed for storing/sending messages for GW network.

DEPENDENCIES
  None.

RETURN VALUE
  Succes or error.

SIDE EFFECTS

===========================================================================*/
static wms_cmd_err_e_type wms_msg_fill_sc_address (
  wms_prov_app_info_s_type *appInfo_ptr,
  wms_address_s_type       *sc_address_ptr
)
{
  if (NULL == appInfo_ptr) {
    return WMS_CMD_ERR_NULL_PTR;
  }

  if (0 == appInfo_ptr->gw_template.sc_addr.number_of_digits)
  {
    return WMS_CMD_ERR_MSG_NO_SC_ADDRESS;
  }

  (void)memscpy( sc_address_ptr,
                 sizeof(wms_address_s_type),
                 &appInfo_ptr->gw_template.sc_addr,
                 sizeof(wms_address_s_type) );

  return WMS_CMD_ERR_NONE;
} /* wms_msg_fill_sc_address () */


#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
/*===========================================================================

FUNCTION wms_msg_wms_address_to_cm_num

DESCRIPTION
  This local function converts the WMS Address to CM Number

DEPENDENCIES
  None.

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean wms_msg_wms_address_to_cm_num
(
  wms_address_s_type      *wms_addr,  /* INPUT */
  cm_num_s_type           *cm_num     /* OUTPUT */
)
{
  if (NULL == wms_addr || NULL == cm_num)
  {
    return FALSE;
  }

  /* Perform conversion */

  /* Direct Conversion due to Same Enum Values */
  cm_num->digit_mode  = (cm_digit_mode_e_type)wms_addr->digit_mode;
  cm_num->number_type = (uint8)wms_addr->number_type;
  cm_num->number_plan = (uint8)wms_addr->number_plan;

  /* Copy Digits over */
#ifdef FEATURE_CMI
  /* CM.h file has removed featurization during CMI effort */
  cm_num->len = MIN(wms_addr->number_of_digits, CM_MAX_NUMBER_CHARS_NON_UMTS);
#else /* FEATURE_CMI */
  cm_num->len = MIN(wms_addr->number_of_digits, CM_MAX_NUMBER_CHARS);
#endif /* FEATURE_CMI */

  cm_num->len = MIN(cm_num->len, WMS_ADDRESS_MAX);
  (void)memscpy( cm_num->buf,
                 cm_num->len,
                 wms_addr->digits,
                 cm_num->len);

  return TRUE;
}

/*===========================================================================

FUNCTION wms_msg_wms_address_from_cm_num

DESCRIPTION
  This local function converts the WMS Address from CM Number

DEPENDENCIES
  None.

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean wms_msg_wms_address_from_cm_num
(
  cm_num_s_type          *cm_num,    /* INPUT */
  wms_address_s_type     *wms_addr   /* OUTPUT */
)
{
  if (NULL == wms_addr || NULL == cm_num)
  {
    return FALSE;
  }

  /* Perform conversion */

  /* Direct Conversion due to Same Enum Values */
  wms_addr->digit_mode  = (wms_digit_mode_e_type)cm_num->digit_mode;
  wms_addr->number_type = (wms_number_type_e_type)cm_num->number_type;
  wms_addr->number_plan = (wms_number_plan_e_type)cm_num->number_plan;

  /* Copy Digits over */
  wms_addr->number_of_digits = MIN(cm_num->len, WMS_ADDRESS_MAX);
  (void)memscpy( wms_addr->digits,
                 wms_addr->number_of_digits,
                 cm_num->buf,
                 wms_addr->number_of_digits);

  return TRUE;
}

/*===========================================================================

FUNCTION wms_msg_convert_plus_address

DESCRIPTION
  This local function converts the plus to the appropriate
  code by calling CM utilities

DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE.

SIDE EFFECTS
  Input Number is modified on success

===========================================================================*/
static boolean wms_msg_convert_plus_address
(
  wms_address_s_type      *wms_addr
)
{
  cmutil_num_conv_req_e_type  num_conv_req = CMUTIL_NUM_CONV_REQ_NONE;

  cm_num_details_for_conversion_s_type num_details;
  cm_num_s_type                        converted_num;

  cmutil_conver_result_e_type          conver_result;

  /* Input Parameter Validation */
  if (NULL == wms_addr)
  {
    return FALSE;
  }

  memset (&num_details, 0, sizeof(num_details));
  memset (&converted_num, 0, sizeof(converted_num));

  /* Init num conversion params */
  cm_util_init_num_conversion_params ( &num_conv_req,
                                       &num_details );

  /* Operation to be performed on num */
  num_conv_req = CMUTIL_NUM_CONV_REQ_PLUS_TO_DIGITS;

  /* Fill the number details before calling conversion */
  num_details.hs_based_plus_dial_setting
     = cfg_s_ptr->hs_based_plus_dial_setting;
  if (CM_INVALID_MOBILE_COUNTRY_CODE == cfg_s_ptr->current_mobile_country_code)
  {
      num_details.current_mobile_country_code
      = cfg_s_ptr->home_mobile_country_code;
  }
  else
  {
      num_details.current_mobile_country_code
       = cfg_s_ptr->current_mobile_country_code;
  }
  num_details.home_mobile_country_code
     = cfg_s_ptr->home_mobile_country_code;

  /* Assuming Voice Call for outgoing SMS Messages */
  num_details.call_type = CM_CALL_TYPE_SMS;

  // Convert from WMS Address to CM Number
  if (FALSE == wms_msg_wms_address_to_cm_num(wms_addr, &num_details.num))
  {
    MSG_ERROR_0(" wms_msg_wms_address_to_cm_num Failed!");
    return FALSE;
  }

  /* Perform Conversion */
  conver_result  = cm_util_perform_num_conversion (  num_conv_req,
                                                     &num_details,
                                                     &converted_num );

  if (CMUTIL_CONVER_RESULT_NUM_MODIFIED == conver_result)
  {
    MSG_HIGH_0("CM num_conversion - Number Changed");

    /* Convert back to WMS Address */
    if (FALSE == wms_msg_wms_address_from_cm_num(&converted_num, wms_addr))
    {
      MSG_ERROR_0(" wms_msg_wms_address_from_cm_num Failed!");
      return FALSE;
    }
  }
  else if (CMUTIL_CONVER_RESULT_NUM_UNCHANGED == conver_result)
  {
    MSG_HIGH_0("CM num_conversion - Number Unchanged");
  }
  else
  {
    MSG_ERROR_1(" CM num_conversion Failed Status = %d", conver_result);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION wms_msg_convert_address_to_plus

DESCRIPTION
  This local function converts the code to the appropriate
  code by calling CM utilities

DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE.

SIDE EFFECTS
  Input Number is modified on success

===========================================================================*/
static boolean wms_msg_convert_address_to_plus
(
  wms_address_s_type      *wms_addr
)
{
  cmutil_num_conv_req_e_type  num_conv_req = CMUTIL_NUM_CONV_REQ_NONE;

  cm_num_details_for_conversion_s_type num_details;
  cm_num_s_type                        converted_num;

  cmutil_conver_result_e_type          conver_result;
  uint8                                modified_digits[ WMS_ADDRESS_MAX ];
  uint8                                modified_len = 0;
  /* Input Parameter Validation */
  if (NULL == wms_addr)
  {
    return FALSE;
  }

  memset (&num_details, 0, sizeof(num_details));
  memset (&converted_num, 0, sizeof(converted_num));

  /* Init num conversion params */
  cm_util_init_num_conversion_params ( &num_conv_req,
                                       &num_details );

  /* Operation to be performed on num */
  num_conv_req = CMUTIL_NUM_CONV_REQ_DIGITS_TO_PLUS;

  /* Fill the number details before calling conversion */
  num_details.hs_based_plus_dial_setting
     = cfg_s_ptr->hs_based_plus_dial_setting;
  num_details.current_mobile_country_code
     = cfg_s_ptr->current_mobile_country_code;
  num_details.home_mobile_country_code
     = cfg_s_ptr->home_mobile_country_code;

  /* Assuming Voice Call for incoming SMS Messages */
  num_details.call_type = CM_CALL_TYPE_SMS;

  // Convert from WMS Address to CM Number
  if (FALSE == wms_msg_wms_address_to_cm_num(wms_addr, &num_details.num))
  {
    MSG_ERROR_0(" wms_msg_wms_address_to_cm_num Failed!");
    return FALSE;
  }

  /* Perform Conversion
  *  cm_util_perform_num_conversion take as input, digits in DTMF or ASCII
  *  cm_util_perform_num_conversion returns DTMF digits if number type is International.
  *  cm_util_perform_num_conversion removes the IDD part of the number, sets
  *  Number_Type as International and returns the modified number.
  */
  conver_result  = cm_util_perform_num_conversion (  num_conv_req,
                                                     &num_details,
                                                     &converted_num );

  /*
  * CMUTIL_CONVER_RESULT_NUM_MODIFIED means number is International,
  * and IDD part has been removed from the digits returned
  * Also the digits returned will be in DTMF and Number Type will be set to International
  */
  if (CMUTIL_CONVER_RESULT_NUM_MODIFIED == conver_result)
  {
    MSG_HIGH_0("CM num_conversion - Number Changed");

    /* Convert back to WMS Address */
    if (FALSE == wms_msg_wms_address_from_cm_num(&converted_num, wms_addr))
    {
      MSG_ERROR_0(" wms_msg_wms_address_from_cm_num Failed!");
      return FALSE;
    }
    /*Convert DTMF digits to ASCII Digit since Digit_Mode DTMF don't allow
      Number_Type parameter in Address Parameter*/
    if (WMS_DIGIT_MODE_4_BIT == wms_addr->digit_mode)
    {
      wms_addr->number_of_digits = MIN(wms_addr->number_of_digits, WMS_ADDRESS_MAX);
      modified_len = wms_ts_dtmf2ascii(wms_addr->number_of_digits, wms_addr->digits, modified_digits);
      MSG_HIGH_2( "Converted DTMF digits to ASCII digits, orig_len: %d, converted_len: %d",
                  wms_addr->number_of_digits, modified_len );

      /* If conversion is successful, then copy the converted digits to wms_addr */
      if (modified_len == wms_addr->number_of_digits)
      {
        (void)memscpy( wms_addr->digits,
                       (size_t) WMS_ADDRESS_MAX,
                       modified_digits,
                       wms_addr->number_of_digits);
        wms_addr->digit_mode = WMS_DIGIT_MODE_8_BIT;
      }
    }
  }
  else if (CMUTIL_CONVER_RESULT_NUM_UNCHANGED == conver_result)
  {
    MSG_HIGH_0("CM num_conversion - Number Unchanged");
  }
  else
  {
    MSG_ERROR_1(" CM num_conversion Failed Status = %d", conver_result);
    return FALSE;
  }

  return TRUE;
}

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS

static pbm_provision_enum_type wms_sim_mmgsdi_session_type_to_pbm_prov_type(
  mmgsdi_session_type_enum_type mmgsdi_session_type
)
{
   pbm_provision_enum_type prov_type = PBM_PROVISION_NONE;

   switch (mmgsdi_session_type)
   {
   case MMGSDI_GW_PROV_PRI_SESSION:
      prov_type = PBM_PROVISION_GW_PRIMARY;
      break;

   case MMGSDI_GW_PROV_SEC_SESSION:
      prov_type = PBM_PROVISION_GW_SECONDARY;
      break;

   case MMGSDI_1X_PROV_PRI_SESSION:
      prov_type = PBM_PROVISION_1X_PRIMARY;
      break;

   case MMGSDI_1X_PROV_SEC_SESSION:
      prov_type = PBM_PROVISION_1X_SECONDARY;
      break;

   case MMGSDI_GW_PROV_TER_SESSION:
        prov_type = PBM_PROVISION_GW_TERTIARY;
        break;

   case MMGSDI_1X_PROV_TER_SESSION:
        prov_type = PBM_PROVISION_1X_TERTIARY;
        break;

   default:
      break;
   }

   return prov_type;
} /* wms_sim_mmgsdi_session_type_to_pbm_prov_type() */

/*===========================================================================

FUNCTION wms_msg_mo_sms_fdn_control_func

DESCRIPTION
  This function checks for MO SMS FDN Control

DEPENDENCIES
  None.

RETURN VALUE
  WMS_CMD_ERR_MSG_BLOCKED if blocked due to FDN
  WMS_CMD_ERR_NONE if Allowed

SIDE EFFECTS
  None.

===========================================================================*/
wms_cmd_err_e_type wms_msg_mo_sms_fdn_control_func(
  wms_prov_app_info_s_type  *appInfo_ptr,
  wms_message_mode_e_type    msg_mode,
  wms_address_s_type        *address
)
{
   wms_cmd_err_e_type         cmd_err     = WMS_CMD_ERR_NONE;
   uint8                     *pAddress    = NULL;
   int                        nAddressLen;
   pbm_phonebook_type         pb_id;

   (void)msg_mode; // parameter not used

   if (NULL == appInfo_ptr) {
      cmd_err = WMS_CMD_ERR_NULL_PTR;
      return cmd_err;
   }

   if (!appInfo_ptr->fdn_enabled)
   {
      return cmd_err;
   }

   if (WMS_ADDRESS_MAX < address->number_of_digits)
   {
     MSG_ERROR_0("Invalid address passed");
     return WMS_CMD_ERR_BAD_PARM;
   }

   /* Convert Address to Character String */

   /* One for + Sign and One for Null Character */
   pAddress = wms_mem_malloc(address->number_of_digits+2);
   if (NULL == pAddress)
   {
      MSG_ERROR_0("Null Pointer Allocation");
      return WMS_CMD_ERR_NULL_PTR;
   }

   memset(pAddress, 0, address->number_of_digits+2);
   if (WMS_NUMBER_INTERNATIONAL == address->number_type)
   {
      /* Append Plus Sign */
      pAddress[0] = '+';
      wms_ts_bcd_to_ascii(address->digits, address->number_of_digits, pAddress+1);
      nAddressLen = address->number_of_digits+1;
   }
   else
   {
      wms_ts_bcd_to_ascii(address->digits, address->number_of_digits, pAddress);
      nAddressLen = address->number_of_digits;
   }


   pb_id.pb_category = PBM_LPB;
   pb_id.prov_type   = wms_sim_mmgsdi_session_type_to_pbm_prov_type(appInfo_ptr->mmgsdi_appInfo.session_type);
   pb_id.slot_id     = (uint16) appInfo_ptr->mmgsdi_appInfo.slot_id;
   pb_id.device_type = PBM_FDN;

   if (PBM_SUCCESS !=  pbm_session_find_number( pb_id,
                                                pAddress,
                                                nAddressLen,
                                                NULL,
                                                NULL ) )
   {
      /* Number not present in FDN List*/
      MSG_ERROR_0("MO SMS Destination not in FDN List");
      cmd_err = WMS_CMD_ERR_MSG_BLOCKED;
   }

   wms_mem_free(pAddress);

   /* Allow SMS initialized as WMS_CMD_ERR_NONE */
   return cmd_err;
} /* wms_msg_mo_sms_fdn_control_func() */

/*===========================================================================

FUNCTION wms_msg_rp_error_permanent

DESCRIPTION
  This function checks if RP error is permanent or not

  DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean wms_msg_rp_error_permanent(wms_report_status_e_type status, uint8 cause)
{
   boolean err_perm = FALSE;

   if (WMS_RPT_RP_ERROR == status)
   {
      switch (cause)
      {
        case WMS_RP_CAUSE_UNASSIGNED_NUMBER:                 /*   = 1,  MO error */
        case WMS_RP_CAUSE_CALL_BARRED:                       /*   = 10, MO error */
        case WMS_RP_CAUSE_SM_REJECTED:                       /*   = 21, MO error */
        case WMS_RP_CAUSE_UNIDENTIFIED_SUBSCRIBER:           /*   = 28, MO error */
        case WMS_RP_CAUSE_UNKNOWN_SUBSCRIBER:                /*   = 30, MO & SMMA error */
        case WMS_RP_CAUSE_INVALID_MANDATORY_INFORMATION:     /*   = 96, MO/MT/SMMA/CP error */
           err_perm = TRUE;
           break;

        default:
           break;
      }
   }

   return err_perm;
}

#endif /* FEATURE_GWSMS */

uint32 wms_msg_get_pending_1x_mo_sms_count(
  void
)
{
  return (uint32)msg_s_ptr->cdma_mo_rec.count;
}

uint32 wms_msg_get_pending_gw_mo_sms_count(
  sys_modem_as_id_e_type             as_id
)
{
  uint32  i;
  uint32 cnt = 0;

  for ( i = 0; i < WMS_MAX_MO_MESSAGES; i++ )
  {
    if (NULL == msg_s_ptr->gw_mo_rec[i])
    {
       continue; /*skip empty slot*/
    }
    else if (as_id == msg_s_ptr->gw_mo_rec[i]->as_id)
    {
      cnt++;
    }
  }
  return cnt;
}

#ifdef FEATURE_SMS_PWS_OVER_LTE

/*===========================================================================
FUNCTION
  wms_msg_lte_etws_prim_proc()

DESCRIPTION
  Processes a primary ETWS notification.

  If the message is not detected as a duplicate, the message is sent
  to clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_msg_lte_etws_prim_proc(
  lte_rrc_etws_prim_ind_s  *lte_etws_prim_ptr
)
{
  boolean   msg_is_dup = FALSE;

  if (NULL == lte_etws_prim_ptr)
  {
    MSG_ERROR_0("lte_etws_prim_ptr is NULL!");
    return;
  }

  MSG_HIGH_2( "In wms_msg_lte_etws_prim_proc(): msg_id 0x%04x with serial 0x%04x",
              lte_etws_prim_ptr->msg_id,
              lte_etws_prim_ptr->sr_num );

  // Do duplicate detection based on the NV - DISABLE_LTE_CB_DUP_DETECTION - value
  if (!cfg_s_ptr->lte_cb_dup_detection_is_disabled)
  { /* duplicate detection */
    time_t                   search_time;

    // Get timestamp of the duplicate search
    search_time = time_get_secs();

    // Check for duplicate message
    msg_is_dup = wms_bc_lte_message_is_duplicate( WMS_LTE_DUPLIST_PRIMARY,
                                                  (void *)lte_etws_prim_ptr,
                                                  search_time,
                                                  current_cell_info );

    MSG_HIGH_3( "PriDupDet returns %d for msg_id 0x%04x with serial 0x%04x",
                msg_is_dup,
                lte_etws_prim_ptr->msg_id,
                lte_etws_prim_ptr->sr_num );
  } /* duplicate detection */

  // Send notification to clients if not duplicate
  if (!msg_is_dup)
  {
    uint8                pos          = 0;
    wms_etws_msg_s_type *etws_msg_ptr = &(msg_event_info.etws_msg_info);

    etws_msg_ptr->type = WMS_ETWS_NOTIFICATION_PRIMARY;
    etws_msg_ptr->plmn_info_present = TRUE;
    etws_msg_ptr->plmn_info.mcc[0] = (uint8) lte_etws_prim_ptr->plmn.mcc[0];
    etws_msg_ptr->plmn_info.mcc[1] = (uint8) lte_etws_prim_ptr->plmn.mcc[1];
    etws_msg_ptr->plmn_info.mcc[2] = (uint8) lte_etws_prim_ptr->plmn.mcc[2];
    etws_msg_ptr->plmn_info.mnc[0] = (uint8) lte_etws_prim_ptr->plmn.mnc[0];
    etws_msg_ptr->plmn_info.mnc[1] = (uint8) lte_etws_prim_ptr->plmn.mnc[1];
    etws_msg_ptr->plmn_info.mnc[2] = (uint8) lte_etws_prim_ptr->plmn.mnc[2];
    etws_msg_ptr->plmn_info.num_mnc_digits = (uint8) lte_etws_prim_ptr->plmn.num_mnc_digits;

    /* Serial Number */
    etws_msg_ptr->data[pos++] = (uint8)((lte_etws_prim_ptr->sr_num & 0xFF00)>>8);
    etws_msg_ptr->data[pos++] = (uint8)(lte_etws_prim_ptr->sr_num & 0x00FF);

    /* Message Identifier */
    etws_msg_ptr->data[pos++] = (uint8)((lte_etws_prim_ptr->msg_id & 0xFF00)>>8);
    etws_msg_ptr->data[pos++] = (uint8)(lte_etws_prim_ptr->msg_id & 0x00FF);

    /* Warning Type */
    etws_msg_ptr->data[pos++] = lte_etws_prim_ptr->warning_type[0];
    etws_msg_ptr->data[pos++] = lte_etws_prim_ptr->warning_type[1];

    etws_msg_ptr->data_len = pos;

    if (lte_etws_prim_ptr->warning_sec_info_present)
    {
      etws_msg_ptr->data_len += sizeof(lte_etws_prim_ptr->warning_sec_info);

      (void)memscpy( etws_msg_ptr->data + pos,
                     sizeof(lte_etws_prim_ptr->warning_sec_info),
                     lte_etws_prim_ptr->warning_sec_info,
                     sizeof(lte_etws_prim_ptr->warning_sec_info));
    }

    (void)wms_msg_event_notify( WMS_MSG_EVENT_ETWS_NOTIFICATION, &msg_event_info);
  }

  return;
} /* wms_msg_lte_etws_prim_proc() */

/*===========================================================================
FUNCTION
  wms_msg_lte_etws_sec_proc()

DESCRIPTION
  Processes a secondary ETWS notification.

  If the message is not detected as a duplicate, the message is sent
  to clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_msg_lte_etws_sec_proc(
  lte_rrc_etws_sec_ind_s   *lte_etws_sec_ptr
)
{
  boolean                  msg_is_dup     = FALSE;
  msgr_attach_struct_type *att_ptr        = NULL;
  dsm_item_type           *rx_dsm_ptr     = NULL;

  if (NULL == lte_etws_sec_ptr)
  {
    MSG_ERROR_0("lte_etws_sec_ptr is NULL!");
    return;
  }

  MSG_HIGH_2( "In wms_msg_lte_etws_sec_proc(): msg_id 0x%04x with serial 0x%04x",
              lte_etws_sec_ptr->msg_id,
              lte_etws_sec_ptr->sr_num );

  // Do duplicate detection based on the NV - DISABLE_LTE_CB_DUP_DETECTION - value
  if (!cfg_s_ptr->lte_cb_dup_detection_is_disabled)
  { /* duplicate detection */
    time_t                   search_time;

    // Get timestamp of the duplicate search
    search_time = time_get_secs();

    // Check for duplicate message
    msg_is_dup = wms_bc_lte_message_is_duplicate( WMS_LTE_DUPLIST_SECONDARY,
                                                  (void *)lte_etws_sec_ptr,
                                                  search_time,
                                                  current_cell_info );

    MSG_HIGH_3( "SecDupDet returns %d for msg_id 0x%04x with serial 0x%04x",
                msg_is_dup,
                lte_etws_sec_ptr->msg_id,
                lte_etws_sec_ptr->sr_num );
  } /* duplicate detection */

  /*
   * Get the attachment to the msgr message.
   * The attachment must be fetched whether the message is a duplicate or not.
   * Leaving the attachment unfetched will eventually exhaust the buffer pool.
   *
   * 1) Get the number of attachments and verify attachment count==1
   * 2) Get the reference to the attachment
   * 3) Use the attachment reference to pull in the attachment data
   */
  if (msgr_get_num_attach((msgr_hdr_struct_type *)lte_etws_sec_ptr) != 1)
  {
    MSG_ERROR_0("Number of attachments is not 1");
    return;
  }

  //lint -save -e826
  att_ptr = msgr_get_attach((msgr_hdr_struct_type *)lte_etws_sec_ptr, 0); // lint warning 826 from msgr.h
  //lint -restore

  if (NULL == att_ptr)
  {
    MSG_ERROR_0("msgr_get_attach() returns NULL!");
    return;
  }

  msgr_get_dsm_attach(att_ptr, &rx_dsm_ptr);

  if (NULL == rx_dsm_ptr)
  {
    MSG_ERROR_0("msgr_get_dsm_attach returns NULL pointer!");
    return;
  }

  // If the message is not a duplicate, send the message notification to clients
  if (!msg_is_dup)
  {
    uint8                    pos               = 0;
    wms_etws_msg_s_type     *etws_msg_ptr      = NULL;
    uint32                   dsm_packet_length = 0;

    etws_msg_ptr      = &(msg_event_info.etws_msg_info);
    etws_msg_ptr->type  = WMS_ETWS_NOTIFICATION_SECONDARY_UMTS_FORMAT;
    etws_msg_ptr->plmn_info_present = TRUE;

    etws_msg_ptr->plmn_info.mcc[0] = (uint8)lte_etws_sec_ptr->plmn.mcc[0];
    etws_msg_ptr->plmn_info.mcc[1] = (uint8)lte_etws_sec_ptr->plmn.mcc[1];
    etws_msg_ptr->plmn_info.mcc[2] = (uint8)lte_etws_sec_ptr->plmn.mcc[2];
    etws_msg_ptr->plmn_info.mnc[0] = (uint8)lte_etws_sec_ptr->plmn.mnc[0];
    etws_msg_ptr->plmn_info.mnc[1] = (uint8)lte_etws_sec_ptr->plmn.mnc[1];
    etws_msg_ptr->plmn_info.mnc[2] = (uint8)lte_etws_sec_ptr->plmn.mnc[2];

    etws_msg_ptr->plmn_info.num_mnc_digits = (uint8) lte_etws_sec_ptr->plmn.num_mnc_digits;

    etws_msg_ptr->data[pos++] = 1; // CBS Message

    /* Message Identifier */
    etws_msg_ptr->data[pos++] = (uint8)( (lte_etws_sec_ptr->msg_id & 0xFF00) >> 8);
    etws_msg_ptr->data[pos++] = (uint8)( (lte_etws_sec_ptr->msg_id & 0x00FF) );

    /* Serial Number */
    etws_msg_ptr->data[pos++] = (uint8)( (lte_etws_sec_ptr->sr_num & 0xFF00) >> 8);
    etws_msg_ptr->data[pos++] = (uint8)( (lte_etws_sec_ptr->sr_num & 0x00FF) );

    /* DCS */
    etws_msg_ptr->data[pos++] = (uint8)lte_etws_sec_ptr->data_coding_scheme;

    etws_msg_ptr->data_len = pos;

    // Get the length of CB data for the secondary notification
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);

    // Verify packet is not empty and fits into the buffer
    if ( (0 == dsm_packet_length) ||
         (dsm_packet_length > (WMS_ETWS_MAX_MSG_LEN - etws_msg_ptr->data_len)) )
    {
      // Free the attachment buffer -- unfetched data is thrown away
      dsm_free_packet(&rx_dsm_ptr);

      MSG_ERROR_2( "ETWS sec packet len=%d: empty or exceeds %d",
                   dsm_packet_length,
                   (WMS_ETWS_MAX_MSG_LEN - etws_msg_ptr->data_len) );
      return;
    }

    // Copy the secondary notification data start at 7 to N
    (void) dsm_pullup(&rx_dsm_ptr, etws_msg_ptr->data + etws_msg_ptr->data_len, (uint16)dsm_packet_length);

    if (NULL == rx_dsm_ptr)
    {
      MSG_HIGH_0("Secondary notification has been successfully retrieved");
      etws_msg_ptr->data_len += dsm_packet_length;
    }
    else
    {
      MSG_HIGH_0("Can't get the entire secondary notification from mgsr attachment");
    }

    // Send notification to clients
    (void)wms_msg_event_notify( WMS_MSG_EVENT_ETWS_NOTIFICATION, &msg_event_info );
  } /* if (!msg_is_dup) */

  if (NULL != rx_dsm_ptr)
  {
    // Free the attachment buffer if any data remains -- unfetched data is thrown away
    // The message was either a duplicate or fetching attachment data failed
    dsm_free_packet(&rx_dsm_ptr);
  }

  return;
} /* wms_msg_lte_etws_sec_proc() */

void wms_msg_lte_cmas_proc(
  lte_rrc_cmas_ind_s       *lte_cmas_ptr
)
{
  boolean                   msg_is_dup     = FALSE;
  msgr_attach_struct_type  *att_ptr        = NULL;
  dsm_item_type            *rx_dsm_ptr     = NULL;
  wms_prov_app_info_s_type *appInfo_ptr    = NULL;


  if (NULL == lte_cmas_ptr)
  {
    MSG_ERROR_0("lte_cmas_ptr is NULL!");
    return;
  }

  MSG_HIGH_2( "In wms_msg_lte_cmas_proc(): msg_id 0x%04x with serial 0x%04x",
              lte_cmas_ptr->msg_id,
              lte_cmas_ptr->sr_num );

  
  /*
   * Get the attachment to the msgr message.
   * The attachment must be fetched whether the message is a duplicate or not.
   * Leaving the attachment unfetched will eventually exhaust the buffer pool.
   *
   * 1) Get the number of attachments and verify attachment count==1
   * 2) Get the reference to the attachment
   * 3) Use the attachment reference to pull in the attachment data
   */
  if (msgr_get_num_attach((msgr_hdr_struct_type *)lte_cmas_ptr) != 1)
  {
    MSG_ERROR_0("Number of attachments is not 1");
    return;
  }

  //lint -save -e826
  att_ptr = msgr_get_attach((msgr_hdr_struct_type *)lte_cmas_ptr, 0); // lint warning 826 from msgr.h
  //lint -restore

  if (NULL == att_ptr)
  {
    MSG_ERROR_0("msgr_get_attach() returns NULL!");
    return;
  }

  msgr_get_dsm_attach(att_ptr, &rx_dsm_ptr);

  if (NULL == rx_dsm_ptr)
  {
    MSG_ERROR_0("msgr_get_dsm_attach returns NULL pointer!");
    return;
  }

  // Reject the CMAS message if No LTE subscription is present
  appInfo_ptr = wms_get_lte_appinfo();

  if (NULL == appInfo_ptr)
  {
    MSG_HIGH_0("No LTE service found on any subs");
	dsm_free_packet(&rx_dsm_ptr);
    return;
  }

  // Do duplicate detection based on the NV - DISABLE_LTE_CB_DUP_DETECTION - value
  if (!cfg_s_ptr->lte_cb_dup_detection_is_disabled)
  { /* duplicate detection */
    time_t                   search_time;

    // Get timestamp of the duplicate search
    search_time = time_get_secs();

    // Check for duplicate message
    msg_is_dup = wms_bc_lte_message_is_duplicate( WMS_LTE_DUPLIST_CMAS,
                                                  (void *)lte_cmas_ptr,
                                                  search_time,
                                                  current_cell_info );

    MSG_HIGH_3( "CMASDupDet returns %d for msg_id 0x%04x with serial 0x%04x",
                msg_is_dup,
                lte_cmas_ptr->msg_id,
                lte_cmas_ptr->sr_num );
  } /* duplicate detection */


  // If the message is not a duplicate, send the message notification to clients
  if (!msg_is_dup)
  {
    uint8  cmas_page_data[WMS_GW_CB_PAGE_SIZE];
    uint8  pos                                  = 0;
    uint8  page_param_pos                       = 0;
    uint32 dsm_packet_length                    = 0;
    uint8  cmas_num_pages                       = 0;
    int    byte_count;
    int    page_idx;

    /* Serial Number */
    cmas_page_data[pos++] = (uint8)((lte_cmas_ptr->sr_num & 0xFF00)>>8);
    cmas_page_data[pos++] = (uint8)(lte_cmas_ptr->sr_num & 0x00FF);

    /* Message Identifier */
    cmas_page_data[pos++] = (uint8)((lte_cmas_ptr->msg_id & 0xFF00)>>8);
    cmas_page_data[pos++] = (uint8)(lte_cmas_ptr->msg_id & 0x00FF);

    /* DCS*/
    cmas_page_data[pos++] = (uint8)lte_cmas_ptr->data_coding_scheme;
    page_param_pos = pos;
    cmas_page_data[pos++] = 0x11; // Single page 0001 0001.  Will get fixed below

    // Get the length of CB data for the secondary notification
    dsm_packet_length = dsm_length_packet(rx_dsm_ptr);

    // Verify packet is not empty and is at least 84 bytes
    if ( (0 == dsm_packet_length) || (dsm_packet_length < 84) )
    {
      // Free the attachment buffer -- unfetched data is thrown away
      dsm_free_packet(&rx_dsm_ptr);

      MSG_ERROR_1( "CMAS packet len=%d: empty or not at least 84", dsm_packet_length);

      return;
    }

    /*
    - Number-of-Pages;
    - CBS Message Information Page;
    - CBS-Message-Information-Length.
    =======================================================
    |  Octet Number(s) | Parameter                        |
    =======================================================
    |       1          | Number-of-Pages                  |
    |     2 – 83       | CBS Message Information Page 1   |
    |       84         | CBS-Message-Information-Length 1 |
    |                  |    ...  ...                      |
    |                  | CBS Message Information Page n   |
    |                  | CBS-Message-Information-Length n |
    =======================================================
    | NOTE:       n equal to or less than 15              |
    =======================================================
    */

    // Get the number of pages in this buffer
    byte_count = dsm_pullup(&rx_dsm_ptr, &cmas_num_pages, 1);
    dsm_packet_length -= (uint32)byte_count;

    MSG_HIGH_1("CMAS notification has %d page(s)", cmas_num_pages);

    if ( (cmas_num_pages < 1) || (cmas_num_pages > 15) )
    {
      MSG_ERROR_1("CMAS notification page count out of range (1-15): %d", cmas_num_pages);
      cmas_num_pages = 0;
    }

    for ( page_idx = 1; page_idx <= cmas_num_pages; page_idx++)
    {
      if (dsm_packet_length >= 83)
      {
        // There are at least 83 bytes in the buffer at this point

        // Fill in page_parameter (cur_page, num_pages)
        cmas_page_data[page_param_pos] = ((page_idx & 0x0F) << 4) + (cmas_num_pages & 0x0F);

        // Get the CMAS notification data for this page
        byte_count = dsm_pullup(&rx_dsm_ptr, cmas_page_data + pos, 82);
        dsm_packet_length -= (uint32)byte_count;

        if (82 == byte_count)
        {
          uint8  cmas_page_len = 0;

          // Get the CMAS notification data length for this page
          byte_count = dsm_pullup(&rx_dsm_ptr, &cmas_page_len, 1);
          dsm_packet_length -= (uint32)byte_count;

          // Add the header size to the length
          cmas_page_len += pos;

          MSG_HIGH_3( "Successfully retrieved CMAS notification page %d of %d, len=%d",
                      page_idx,
                      cmas_num_pages,
                      cmas_page_len );

          if (cmas_page_len <= WMS_GW_CB_PAGE_SIZE)
          {
            wms_msg_gw_cb_data_ind_proc(appInfo_ptr->asid, cmas_page_data, (int)cmas_page_len, TRUE);
          }
        }
        else
        {
          MSG_HIGH_0("Could not get the CMAS notification body from mgsr attachment");
        }
      } // if (dsm_packet_length >= 83)
    } // for ( page_idx = 1; page_idx <= cmas_num_pages; page_idx++)
  } // if (!msg_is_dup)

  if (NULL != rx_dsm_ptr)
  {
    // Free the attachment buffer if any data remains -- unfetched data is thrown away
    // The message was either a duplicate or fetching attachment data failed
    dsm_free_packet(&rx_dsm_ptr);
  }

  return;
} /* wms_msg_lte_cmas_proc() */


/*===========================================================================
FUNCTION wms_msg_lte_srv_ind_proc

DESCRIPTION
  Initialize the Current cell information in WMS cache
  And clear the CB Dup Lists if PLMN changes
  Send Cell Changed event to WMS clients

DEPENDENCIES
  LTE_RRC_SERVICE_IND indication from LTE RRC

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_msg_lte_srv_ind_proc(
  lte_rrc_service_ind_s       *lte_srv_ind_ptr
)
{
  MSG_HIGH_2( "wms_msg_lte_srv_ind_proc svc_status = %d, global_cell_id = %d",
              lte_srv_ind_ptr->svc_status,
              lte_srv_ind_ptr->camped_svc_info.cell_identity );

  // SVC_STATUS should be AVAILABLE for any valid entry to be read. Else ignore this event
  if (LTE_RRC_SVC_STATUS_AVAILABLE == lte_srv_ind_ptr->svc_status)
  {
    wms_cell_info_s_type cell_info;
    int i;
    boolean cell_changed = FALSE;
    boolean tac_changed = FALSE;
    boolean plmn_changed = FALSE;

    // Fill in the temporary structure with the new cell information.
    // It will be compared against WMS Cached value to detect cell/ PLMN/ TAC change
    cell_info.cell_identity = lte_srv_ind_ptr->camped_svc_info.cell_identity;
    cell_info.phy_cell_id = lte_srv_ind_ptr->camped_svc_info.phy_cell_id;
    cell_info.selected_plmn.num_mnc_digits = lte_srv_ind_ptr->camped_svc_info.selected_plmn.num_mnc_digits;

    for (i = 0; i < LTE_RRC_MAX_MNC_DIGITS; ++i)
    {
      cell_info.selected_plmn.mnc[i] = lte_srv_ind_ptr->camped_svc_info.selected_plmn.mnc[i];
    }

    for (i = 0; i < LTE_RRC_MAX_MCC_DIGITS; ++i)
    {
      cell_info.selected_plmn.mcc[i] = lte_srv_ind_ptr->camped_svc_info.selected_plmn.mcc[i];
    }

    cell_info.tac = lte_srv_ind_ptr->camped_svc_info.tac;

    // Compare with cached cell information with WMS to check if PLMN/ Cell/ TAC changed
    plmn_changed = !wms_bc_plmns_are_equal(&cell_info.selected_plmn, &current_cell_info.selected_plmn);
    tac_changed  = (cell_info.tac== current_cell_info.tac) ? FALSE : TRUE;
    cell_changed = (cell_info.phy_cell_id == current_cell_info.phy_cell_id) ? FALSE : TRUE;

    MSG_HIGH_3( "PLMN Changed: %d, TAC Changed: %d, CELL Changed: %d",
                plmn_changed,
                tac_changed,
                cell_changed );

    // If PLMN/ TAC/ Cell has changed, then update the WMS Cell infornation cache
    // If PLMN has changed, then update the WMS Dup List to remove invalid entries
    // Send Cell Change event to upper layers
    // Else ignore the event
    if (plmn_changed || tac_changed || cell_changed)
    {
      cell_change_type_T cb_cell_change = CB_WMS_SAME_CELL;

      // Update WMS Cell Info cache with latest information
      current_cell_info = cell_info;

      MSG_HIGH_6( "LTE PLMN INFO MCC: %x%x%x, MNC: %x%x%x", 
                  current_cell_info.selected_plmn.mcc[0],
                  current_cell_info.selected_plmn.mcc[1],
                  current_cell_info.selected_plmn.mcc[2],
                  current_cell_info.selected_plmn.mnc[0],
                  current_cell_info.selected_plmn.mnc[1],
                  current_cell_info.selected_plmn.mnc[2]);

      MSG_HIGH_3( "LTE PLMN INFO PHY_CELL_ID: %x, TAC: %x, CELL_ID: %x", current_cell_info.phy_cell_id,
                  current_cell_info.tac,
                  current_cell_info.cell_identity);

      // Send the cell changed indication to the WMS Clients
      if (plmn_changed)
      {
        cb_cell_change = CB_WMS_PLMN_CHANGE;
      }
      else if (tac_changed)
      {
        cb_cell_change = CB_WMS_LAC_CHANGE;
      }
      else if (cell_changed)
      {
        cb_cell_change = CB_WMS_CELL_CHANGE;
      }
      wms_cfg_cell_change_ind_proc(SYS_MODEM_AS_ID_1, cb_cell_change);
    }
  }
}

#endif /* FEATURE_SMS_PWS_OVER_LTE*/

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

/* END of wmsmsg.c */
