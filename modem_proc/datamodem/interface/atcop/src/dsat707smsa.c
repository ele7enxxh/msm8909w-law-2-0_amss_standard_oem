/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific sms related
  AT commands.

  The cmd is not specified in 707 spec.  We (qcom) add them to automate sms
  testing.

EXTERNALIZED FUNCTIONS

  None

 Copyright (c) 2002 - 2014 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707smsa.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk      Added support for Dynamic ATCoP.
05/06/14   pg      Fixed LATIN encoding issue.
03/11/14   tk      Optimized debug macros usage in ATCoP.
11/14/13   tk      Fixed issues in online command state.
04/19/13   tk      Fixed issue with both DSDS and thin UI enabled.
03/28/13   tk      Mainlined FEATURE_THIN_UI using new NV-EFS item.
11/30/12   tk      ATCoP changes for Triton DSDA.
08/22/12   sk      Mainlining Nikel Changes.
07/13/12   tk      Added graceful handling of unsupported WMS formats.
05/18/12   tk      Migrated to MSG 2.0 macros
04/11/12   nc      Fixed operating mode checks for SMS related commands for NikeL targets. 
01/19/12   sk      Feature cleanup.
09/27/11   ht      Included dsut header to support offtarget stubs.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
05/10/10   kk      Fix to hold back SMS notifications based on CNMI MT value.
07/05/10   nc      Changed the MT SMS Queue Initialisation independent of 
                   the current operating mode.
05/10/10   kk      Added support for ^HC SMS commands and UNICODE SMS.
12/15/09   nc      Mainlining FEATURE_DATA_SERIALIZER.
06/18/09   vg      Fixed Lint errors.
01/27/09   nc      Fixed crashes in QCMGR and QCMGS. Also adding check
                   from user setting "SM" for a RUIM build in NV only mode.
                   Also added support for *,# in <oa>/<da> in QCMGR and $QCMGS
11/13/08   nc      Added Support for QCMGW,QCMSS,QCMGL Commands
06/01/07   sa      Mainlining SMS C2K commands
11/28/03   sb      created the module. CDMA SMS additions

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#ifdef FEATURE_CDMA_SMS

#include "dsati.h"
#include "dsatsmsi.h"
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#include "dsatparm.h"
#include "err.h"
#include "wms.h"

#include <stringl/stringl.h>
#include "stdio.h"
#include "dsm.h"

/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains reginal definitions for variables that are used
  in ATCOP SMS SUB SYSTEM
===========================================================================*/

/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/
#define DEBUG( x )

/*===========================================================================
            Data types
===========================================================================*/

/* import variable from other files of sms module */
/* Variable to find out what command is being processed currently */
extern dsat_sms_cmd_s_type  dsatsmsi_pres_cmd;

/* WMS client (ATCOP) ID */
extern wms_client_id_type dsatsmsi_client_id;

/* Variable used in QCMGL SM */
extern sms_cmd_sm_type  dsatsmsi_cmd_sm;

/* Variable used QCMSS Command */
extern wms_address_s_type  send_addr;

#ifdef FEATURE_DSAT_EXTENDED_SMS
boolean dsat_unicode_response = FALSE;
/* Used to store the <mode> param in HCMGR command */
extern dsat_num_item_type hcmgr_mode;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

/* Variable to retain  message reference number for send command */
LOCAL dsat_num_item_type  is707_send_mr;

extern wms_client_ts_data_s_type * temp707_ts_data;

/* LOCAL function prototypes */
LOCAL dsat_result_enum_type is707_send_msg_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

LOCAL dsat_result_enum_type is707_submit_msg_report_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

extern dsat_result_enum_type is707_received_msg_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

LOCAL dsat_result_enum_type is707_read_msg_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

LOCAL dsat_result_enum_type is707_delete_msg_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

LOCAL dsat_result_enum_type is707_write_msg_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

#ifdef FEATURE_DSAT_EXTENDED_SMS
LOCAL dsat_num_item_type teleservice707_lookup_ext  
(
  wms_teleservice_e_type tele_type
);
LOCAL dsat_num_item_type encoding707_lookup_ext
( 
  wms_user_data_encoding_e_type encod_arg
);
#endif /* FEATURE_DSAT_EXTENDED_SMS */

/* the Message Event Handler lookup table */
/* note that the order of the functions has to match wms_msg_event_e_type */
const msg_handler_tab_type dsat707sms_msg_handler_table_entries[] = 
{
  { SMS_CFG_EVENT_CDMA_READY,       dsatsmsi_cdma_ready_cfg_event_handler       },
  { SMS_CFG_EVENT_ROUTES,           dsatsmsi_route_cfg_event_handler            },
  { SMS_CFG_EVENT_MEMORY_STATUS,    dsatsmsi_memory_status_cfg_event_handler_ex },
  { SMS_CFG_EVENT_MESSAGE_LIST,     dsatsmsi_message_list_cfg_event_handler     },
  { SMS_MSG_EVENT_SEND,             is707_send_msg_event_handler                },
  { SMS_MSG_EVENT_READ,             is707_read_msg_event_handler                },
  { SMS_MSG_EVENT_DELETE,           is707_delete_msg_event_handler              },
  { SMS_MSG_EVENT_RECEIVED_MESSAGE, is707_received_msg_event_handler            },
  { SMS_MSG_EVENT_SUBMIT_REPORT,    is707_submit_msg_report_event_handler       },
  { SMS_MSG_EVENT_WRITE,            is707_write_msg_event_handler               }
#ifdef FEATURE_DSAT_EXTENDED_SMS
  ,{ SMS_CFG_EVENT_MEMORY_FULL,      dsatsmsi_memory_full_handler               }
#endif /* FEATURE_DSAT_EXTENDED_SMS */

};

const dsatsms_msg_handler_tab_type dsat707sms_msg_handler_table =
{
  dsat707sms_msg_handler_table_entries,
  ARR_SIZE(dsat707sms_msg_handler_table_entries)
};

/*===========================================================================

FUNCTION DSAT707SMSA_SEND_UNSOL_MT_MSG

DESCRIPTION
  This Function sends a unsolicited Indication to TE on a reception of MT SMS
  without storing it in the TA.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/

void dsat707smsa_send_unsol_mt_msg 
(
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  dsm_item_type * res_buff;
  dsm_item_type * formatted_res_buff;
  wms_status_e_type msg_status;
  byte *data_ptr;
  uint32 addr_len = 0;
  uint32 len = 0;
  dsat_err_e_type   err_info = DSAT_ERR_NO_ERROR;
  wms_client_bd_s_type           *cl_bd_data = NULL;

  if (event_info == NULL)
  {
    err_info = DSAT_ERR_INVALID_OPERATION;
    goto send_result;
  }

  temp707_ts_data = (wms_client_ts_data_s_type *) 
                         dsat_alloc_memory(sizeof(wms_client_ts_data_s_type),FALSE);
  cl_bd_data      = (wms_client_bd_s_type *) 
                         dsat_alloc_memory(sizeof(wms_client_bd_s_type), FALSE);
  /*
      In 4 Digit Mode addressing as follows:
      Number   |    4 BIT DTMF       |    ASCII conversion  
               |  conversion by WMS  |
      -------- |-----------------    |------------------- 
       *1204#  |   11 01 02 10 04 12 |   66 49 50 65 52 35
  */
  len = 
    MIN( event_info->u.mt_msg.u.cdma_message.address.number_of_digits, 
         WMS_ADDRESS_MAX - 1 );
  
  if(event_info->u.mt_msg.u.cdma_message.address.digit_mode 
                                                   == WMS_DIGIT_MODE_4_BIT)
  {
    addr_len = wms_ts_dtmf2ascii(
         (uint8)len,
         (const uint8 *) &event_info->u.mt_msg.u.cdma_message.address.digits[0],
         (uint8 *) &event_info->u.mt_msg.u.cdma_message.address.digits[0]);
  }
  if( addr_len >= WMS_ADDRESS_MAX )
  {
    err_info = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
    goto send_result;
  }
  event_info->u.mt_msg.u.cdma_message.address.digits[addr_len] = '\0';
  event_info->u.mt_msg.u.cdma_message.address.number_of_digits =
                                                             (uint8)addr_len;

  msg_status = wms_ts_decode (&event_info->u.mt_msg.u.cdma_message.raw_ts,
                              temp707_ts_data );
  if ( msg_status != WMS_OK_S )
  {
    err_info = DSAT_ERR_INVALID_SYNTAX;
    goto send_result;
  }

  if((temp707_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_ASCII) ||
               (temp707_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_IA5))
  {
    cl_bd_data->user_data.data_len =
          wms_ts_unpack_ascii((wms_cdma_user_data_s_type*)&temp707_ts_data->u.cdma.user_data,
                              temp707_ts_data->u.cdma.user_data.number_of_digits+1,
                              cl_bd_data->user_data.data);
  }
#ifdef FEATURE_DSAT_EXTENDED_SMS
  else if ( (temp707_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE) || 
            (temp707_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_OCTET)   || 
            (temp707_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_GSM_7_BIT_DEFAULT) )
  {
    cl_bd_data->user_data.data_len = MIN(WMS_CDMA_USER_DATA_MAX,
                         temp707_ts_data->u.cdma.user_data.data_len);
    
   (void) dsatutil_memscpy((void*)cl_bd_data->user_data.data,WMS_CDMA_USER_DATA_MAX,
                           (void*)temp707_ts_data->u.cdma.user_data.data,
                           MIN(WMS_CDMA_USER_DATA_MAX,
                           temp707_ts_data->u.cdma.user_data.data_len));
  }
#endif /* FEATURE_DSAT_EXTENDED_SMS */

  res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
  if (NULL != res_buff)
  {
    data_ptr = res_buff->data_ptr;
#ifdef FEATURE_DSAT_EXTENDED_SMS
    if (temp707_ts_data->u.cdma.message_id.type == WMS_BD_TYPE_DELIVERY_ACK) 
    {
      data_ptr += (word)snprintf((char*)data_ptr,
                                       res_buff->size - res_buff->used,
                                       "^HCDS: ");
    }
    else
    {
      data_ptr += (word)snprintf((char*)data_ptr,
                                       res_buff->size - res_buff->used,
                                       "^HCMT: ");
    }
#else
    data_ptr += (word)snprintf((char*)data_ptr,
                                       res_buff->size - res_buff->used,
                                       "$QCMT: ");
    /* Update the Result with <stat> */
    data_ptr = dsatsmsi_tag_lookup ( 
                       event_info->u.mt_msg.msg_hdr.tag, 0, data_ptr );
    *data_ptr++ = ',';
#endif /* #ifndef FEATURE_DSAT_EXTENDED_SMS */

    res_buff->used = (uint16) ( data_ptr - res_buff->data_ptr );

    /* Update the Result with <oa> */
    data_ptr += (word)snprintf(
                                (char*)data_ptr, 
                                (res_buff->size - res_buff->used),
#ifdef FEATURE_DSAT_EXTENDED_SMS
                                "%s",
#else
                                "\"%s\"",
#endif /* FEATURE_DSAT_EXTENDED_SMS */
                                &event_info->u.mt_msg.u.cdma_message.address.digits[0]);

    *data_ptr++ = ',';
    /* Update the Result with <scts> */
#ifdef FEATURE_DSAT_EXTENDED_SMS
    /* Always use the HC format of time for URC */
    data_ptr = dsatsmsi_timestamp( temp707_ts_data->u.cdma.mc_time , data_ptr, TRUE );
#else
    data_ptr = dsatsmsi_timestamp( temp707_ts_data->u.cdma.mc_time ,data_ptr, FALSE );
#endif /* FEATURE_DSAT_EXTENDED_SMS */

    *data_ptr++ = ',';
#ifdef FEATURE_DSAT_EXTENDED_SMS
    /* Update the Result with <lang> */
    data_ptr = dsatutil_itoa((uint32) temp707_ts_data->u.cdma.language, 
                              data_ptr, 10);
    *data_ptr++ = ',';
    
    /* Update the Result with <format> */
    data_ptr = dsatutil_itoa(
                  (uint32) encoding707_lookup_ext(
                              temp707_ts_data->u.cdma.user_data.encoding),
                  data_ptr, 10);
    *data_ptr++ = ',';
    
    /* Update the Result with <length> */
    data_ptr = dsatutil_itoa((uint32) cl_bd_data->user_data.data_len, 
                              data_ptr,
                             10);
    *data_ptr++ = ',';
    
    /* Update the Result with <priority> */
    data_ptr = dsatutil_itoa((uint32) temp707_ts_data->u.cdma.priority, 
                             data_ptr, 10);
    *data_ptr++ = ',';
    
    /* Update the Result with <privacy> */
    data_ptr = dsatutil_itoa((uint32) temp707_ts_data->u.cdma.privacy,
                              data_ptr, 10);
    *data_ptr++ = ',';
    
    /* Update the Result with <type>  */
    if (temp707_ts_data->u.cdma.message_id.type == WMS_BD_TYPE_DELIVERY_ACK) 
    {
      /* SMS ACKs have type = 3 */
      data_ptr = dsatutil_itoa(3, data_ptr, 10);
    }
    else
    {
      data_ptr = dsatutil_itoa(
      (uint32) teleservice707_lookup_ext(
                  event_info->u.mt_msg.u.cdma_message.teleservice),
                  data_ptr, 10);
    }
    *data_ptr++ = ',';
  
    /* Update the Result with <stat> */
    data_ptr += (word)snprintf((char*)data_ptr, res_buff->size,
                                     "%d", 
                                     dsatsmsi_tag_lookup_ext(
                                          event_info->u.mt_msg.msg_hdr.tag)
                                     );
    /* when UNICODE encoding is used, the dsat_fmt_response routine bypasses */
    if (temp707_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE) 
    {
      *data_ptr++ = '\r';
    }
#else
    /* Update the Result with <length> */
    data_ptr = dsatutil_itoa((uint32) cl_bd_data->user_data.data_len, data_ptr, 10);
#endif /* FEATURE_DSAT_EXTENDED_SMS */

    *data_ptr++ = '\n';

    res_buff->used = (uint16) ( data_ptr - res_buff->data_ptr );
    /* Update the Result with <data> */
    
    if(temp707_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_OCTET ||
       temp707_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_ASCII ||
       temp707_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_GSM_7_BIT_DEFAULT
#ifdef FEATURE_DSAT_EXTENDED_SMS
       || temp707_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE
#endif /* FEATURE_DSAT_EXTENDED_SMS */
      )
    {
      dsat_dsm_pushdown_tail(&res_buff, (void*)cl_bd_data->user_data.data,
                             cl_bd_data->user_data.data_len,
                             DSM_ITEM_POOL(res_buff), 
                             TRUE);

      len =  cl_bd_data->user_data.data_len;
      DS_AT_MSG1_MED("Received SMS length is %d",len);
    }

    if (NULL != res_buff)
    {
#ifdef FEATURE_DSAT_EXTENDED_SMS
      data_ptr =  res_buff->data_ptr + res_buff->used;
      if (len > 0) 
      {
        if (temp707_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE) 
        {
          *data_ptr++ = 0x00;
        }
        *data_ptr++ = 0x1a;
      }
      res_buff->used = (uint16) ( data_ptr - res_buff->data_ptr );
#endif /* FEATURE_DSAT_EXTENDED_SMS */

      /* We are done with filling up raw response buffer */
      /* Format it and send it to TE                     */
      formatted_res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
#ifdef FEATURE_DSAT_EXTENDED_SMS
      dsat_unicode_response = (temp707_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE);
#endif /* FEATURE_DSAT_EXTENDED_SMS */

      if (NULL != formatted_res_buff)
      {
        /* send the response to the user */
        dsat_fmt_response( &res_buff, DSAT_COMPLETE_RSP, FALSE,
                           &formatted_res_buff );

        if (NULL != formatted_res_buff)
        {
          /* Flush the queue to the Rm interface. */
          dsatcmdp_send_queued_cmd_results( );
      
          if ( formatted_res_buff->used != 0 )
          {
            /* Tell SIOLIB that this is SMS and should be sent to Modem port */
            ds3g_siolib_set_tx_type 
              ( DS3G_SIOLIB_UNSOLICTED_SMS, DS3G_SIOLIB_DATA_PORT );
            dsatrsp_send_response( formatted_res_buff, TRUE );
          }
          else
          {
            dsm_free_packet( &formatted_res_buff );
          }
        }
      }
    }
  }

send_result:

  if (DSAT_ERR_NO_ERROR != err_info)
  {
    DS_AT_MSG1_HIGH("dsat707smsa_send_unsol_mt_msg() Error_Info %d" , err_info);
  }
  dsatutil_free_memory_ext((void **) &temp707_ts_data);
  dsatutil_free_memory_ext((void **) &cl_bd_data);
  return;
} /* dsat707smsa_send_unsol_mt_msg */

/*===========================================================================

FUNCTION  IS707_SEND_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for send message event from WMS.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type is707_send_msg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  wms_client_ts_data_s_type     *msg_ptr=NULL;
  dsat_result_enum_type         result = DSAT_ASYNC_EVENT;
  wms_status_e_type             decode_status;
  dsm_item_type                 * res_buff;
  
/*-------------------------------------------------------------------------*/

  DS_AT_MSG2_HIGH("is707_send_msg_event_handler, pres_cmd: %d HC cmd %d",
                dsatsmsi_pres_cmd.cmd_name,dsatsmsi_pres_cmd.is_hc_cmd);

  ASSERT( event_info != NULL );
  
  if ( ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGS ) ||
       ( dsatsmsi_pres_cmd.cmd_name == SMS_CMSS ) )
  {

    event_info->u.msg_status.u.cdma_message.raw_ts.format = WMS_FORMAT_CDMA;
    msg_ptr = (wms_client_ts_data_s_type *)dsat_alloc_memory(sizeof(wms_client_ts_data_s_type),FALSE);
    decode_status = wms_ts_decode(
                        &event_info->u.msg_status.u.cdma_message.raw_ts, 
                         msg_ptr);

    if ( decode_status == WMS_OK_S )
    {
      /* Retain Useful information from thie event            */
      /* Wait for REPORT from sms server and then send it to the user */

      /* for submit status */
      is707_send_mr = msg_ptr->u.cdma.message_id.id_number;

#ifdef FEATURE_DSAT_EXTENDED_SMS
      if ( TRUE == dsatsmsi_pres_cmd.is_hc_cmd )
      {
        res_buff = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
        res_buff->used = 
          (uint16) snprintf((char*)res_buff->data_ptr,
                                  res_buff->size,
                                  "^HCMGS: %d", is707_send_mr);
        /* The next URC would be basd on success of Failure. Send either
           HCMGSS or HCMGSF based on the status in submit msg handler */
        (void)strlcpy( dsatsmsi_pres_cmd.cmd_hdr,
                           "^HCMGSS",
                           sizeof(dsatsmsi_pres_cmd.cmd_hdr));
        dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
        result = DSAT_OK;
      }
      else
#endif /* FEATURE_DSAT_EXTENDED_SMS */
      {
        result = DSAT_ASYNC_CMD;
      }
    }
    else
    {
      res_buff = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
      /* We got a error in decoding message */
      res_buff->used = (uint16)dsatsmsi_cms_error(res_buff,
                                                  (int)DSAT_CMS_UNKNOWN_ERR);
      /* send the response to the user */
      dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);

      result = DSAT_CMD_ERR_RSP;
      /* We are done with the commands, clean up */
      DSAT_SMS_STATE_CLEAN_ALL();
    }
  } /* end if (dsatsmsi_pres_cmd.cmd_name == SMS_CMGS) */
  dsatutil_free_memory((void *)msg_ptr);
  return result;
}/* is707_send_msg_event_handler */


/*===========================================================================

FUNCTION  IS707_SUBMIT_MSG_REPORT_EVENT_HANDLER

DESCRIPTION
  Handler function for submit message report event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type is707_submit_msg_report_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  dsm_item_type * res_buff;
  byte * data_ptr;
  int error_cause;
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;

/*-------------------------------------------------------------------------*/

  DS_AT_MSG2_HIGH("is707_submit_msg_report_event_handler, pres_cmd: %d HC cmd %d",
                dsatsmsi_pres_cmd.cmd_name,dsatsmsi_pres_cmd.is_hc_cmd);

  ASSERT( event_info != NULL );
  
  if ( (dsatsmsi_pres_cmd.cmd_name == SMS_CMGS ) ||
       (dsatsmsi_pres_cmd.cmd_name == SMS_CMSS )  )
  {
    res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    if ( event_info->u.submit_report.report_status == WMS_RPT_OK )
    {
      /* We have successfully sent the message */
      /* Just return the message reference number to the user */

      res_buff->used = (word )snprintf((char *)res_buff->data_ptr,
                                            res_buff->size,
                                            "%s: ",
                                            dsatsmsi_pres_cmd.cmd_hdr);
      data_ptr =  &(res_buff->data_ptr[ res_buff->used ]);

      data_ptr = dsatsmsi_itoa (is707_send_mr, data_ptr, 10);

      res_buff->used =  (byte)(data_ptr - res_buff->data_ptr);
      result = DSAT_OK;
      DSAT_SMS_STATE_CLEAN_ALL();
      is707_send_mr = 0;
    }/* if(success) */
    else
    {
      /* We got a error */
      if ( (event_info->u.submit_report.report_status == WMS_RPT_RP_ERROR) ||
           (event_info->u.submit_report.report_status == WMS_RPT_CP_ERROR) ||
           (event_info->u.submit_report.report_status == WMS_RPT_PHONE_NOT_READY) || 
           (event_info->u.submit_report.report_status == WMS_RPT_CDMA_TL_ERROR) || 
           (event_info->u.submit_report.report_status == WMS_RPT_NO_ACK) )
      {
        if ((event_info->u.submit_report.tl_status >= 
                                 WMS_TL_OTHER_NETWORK_PROBLEM_MORE_FIRST_S) 
            && (event_info->u.submit_report.tl_status <= 
                                 WMS_TL_OTHER_NETWORK_PROBLEM_MORE_LAST_S)) 
        {
          error_cause = (int)DSAT_CMS_OTHER_GENERAL_PROBLEMS;
        }
        else
        {
          error_cause = 
            (int)event_info->u.submit_report.tl_status;
        }
      }
      else
      {
        /* Internal error                */
        /* Tell TE that it is an unknown */
        if ( TRUE == dsatsmsi_pres_cmd.is_hc_cmd )
        {
          error_cause = (int)DSAT_CMS_OTHER_GENERAL_PROBLEMS;
        }
        else
        {
          error_cause = (int)DSAT_CMS_UNKNOWN_ERR;
        }
      }
#ifdef FEATURE_DSAT_EXTENDED_SMS
      if ( TRUE == dsatsmsi_pres_cmd.is_hc_cmd )
      {
        res_buff->used = (word)snprintf((char *)res_buff->data_ptr,
                                              res_buff->size,
                                              "^HCMGSF");
        data_ptr = &(res_buff->data_ptr[res_buff->used]);
        data_ptr = dsatsmsi_itoa (error_cause, data_ptr, 10);
        res_buff->used += (byte)(data_ptr - res_buff->data_ptr);
      }
      else
#endif /* FEATURE_DSAT_EXTENDED_SMS */
      {
        res_buff->used += 
          (byte)dsatsmsi_cms_error(res_buff, error_cause);
      }

      result = DSAT_CMD_ERR_RSP;
      DSAT_SMS_STATE_CLEAN_ALL();
      is707_send_mr = 0;
    }
  
    /* send the response to the user */
    DSAT_SEND_RESPONSE(res_buff,DSAT_COMPLETE_RSP);
  }

  return result;
} /* is707_submit_msg_report_event_handler */


/*===========================================================================

FUNCTION  IS707_READ_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for read message (incoming) event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type is707_read_msg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  dsm_item_type                * di_ptr;
  dsat_result_enum_type          result = DSAT_ASYNC_EVENT;
  byte                         * data_ptr = NULL;
  unsigned int                   used_val = 0;
  dsat_rsp_enum_type             resp_state;
  uint32                         addr_len = 0;
  uint32                         len = 0;

/* Data structure used primarily to replace the <da> */
  wms_client_message_s_type  * replace707_da_msg = NULL;
/* Data structure for CDMA Bearer and Transport data */
  wms_client_bd_s_type       * cl_bd_data         = NULL;
  wms_client_ts_data_s_type  * cl_ts_data        = NULL;
/*-------------------------------------------------------------------------*/

  DS_AT_MSG2_HIGH("is707_read_msg_event_handler, pres_cmd: %d HC cmd %d",
                dsatsmsi_pres_cmd.cmd_name,dsatsmsi_pres_cmd.is_hc_cmd);

  ASSERT( event_info != NULL );
  if(( dsatsmsi_pres_cmd.cmd_name == SMS_CMGR )||
     ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL) )
  {
    if((dsat_num_item_type)dsatutil_get_val(
                            DSAT707_SMS_QCMGF_IDX,0,0,NUM_TYPE)== 0)
    {
      DS_AT_MSG0_HIGH("PDU Mode Not supported in CDMA");

      result = DSAT_CMD_ERR_RSP;
      DSAT_SMS_STATE_CLEAN_ALL();
      goto send_result;
    }

    /* Check whether the received WMS format is unsupported and if yes handle gracefully */
    if ( event_info->u.msg_status.u.cdma_message.raw_ts.format != WMS_FORMAT_CDMA )
    {
      DS_AT_MSG0_HIGH("Unsupported WMS format");

      result = DSAT_OK;

      /* CMGL specific Processing */
      if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL )
      {
        /* check to see if we keep going or dump the result */
        if(dsatsmsi_cmd_sm.present_index >= dsatsmsi_cmd_sm.num_of_indices)
        {
          if (dsatsmsi_cmd_sm.num_of_stat <= dsatsmsi_cmd_sm.present_stat)
          {
            if ( dsatsmsi_cmd_sm.list_begin == FALSE )
            {
              di_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
              /* Add trailer at the end of response */
              di_ptr->used = 0;
              dsatcmdp_handle_async_cmd_rsp(di_ptr, DSAT_END_OF_RSP);
            }
          }
          else
          {
            /* We have more stat to read */
            (void)dsatsmsi_get_msg_list(event_info->subs_id);
            result =  DSAT_ASYNC_CMD;
            goto send_result;
          }
        }
        else
        {
          /* We are not done with reading the list */
          (void)dsatsmsi_read_msg_list(event_info->subs_id);
          result = DSAT_ASYNC_CMD;
          goto send_result;
        }
      }

      /* We are done with the commands, clean up */
      DSAT_SMS_STATE_CLEAN_ALL();
      goto send_result;
    }

     /*
        In 4 Digit Mode addressing as follows:
        Number   |    4 BIT DTMF       |    ASCII conversion  
                 |  conversion by WMS  |
        -------- |-----------------    |------------------- 
         *1204#  |   11 01 02 10 04 12 |   66 49 50 65 52 35
     */
    len = 
      MIN( event_info->u.msg_status.u.cdma_message.address.number_of_digits, 
           WMS_ADDRESS_MAX - 1 );

    if(event_info->u.msg_status.u.cdma_message.address.digit_mode 
                                                     == WMS_DIGIT_MODE_4_BIT)
    {
      addr_len = wms_ts_dtmf2ascii(
           (uint8)len,
           (const uint8 *) &event_info->u.msg_status.u.cdma_message.address.digits[0],
           (uint8 *) &event_info->u.msg_status.u.cdma_message.address.digits[0]);
    }
    if( addr_len >= WMS_ADDRESS_MAX )
    {
      DS_AT_MSG0_HIGH("ERROR in message conversion");
      result = DSAT_CMD_ERR_RSP;
      DSAT_SMS_STATE_CLEAN_ALL();
      return result;
    }
    event_info->u.msg_status.u.cdma_message.address.digits[addr_len] = '\0';
    event_info->u.msg_status.u.cdma_message.address.number_of_digits =
                                                              (uint8)addr_len;
    
    cl_bd_data = (wms_client_bd_s_type *) dsat_alloc_memory(sizeof(wms_client_bd_s_type),
                                                            FALSE);
    cl_ts_data = (wms_client_ts_data_s_type *) dsat_alloc_memory(sizeof(wms_client_ts_data_s_type),
                                                                 FALSE);

    di_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    data_ptr = di_ptr->data_ptr;
    /* write sms message body to the response buffer */
    /* Convert the Raw ts data to Client ts data */
    if (wms_ts_decode(&event_info->u.msg_status.u.cdma_message.raw_ts, cl_ts_data)
        != WMS_OK_S)
    {
      /* We got an error while reading the message */
      /* Tell TE about it                          */
      used_val = dsatsmsi_cms_error(di_ptr, (int)DSAT_CMS_UNKNOWN_ERR);
      di_ptr->used = (byte)used_val;
      result = DSAT_CMD_ERR_RSP;
      /* Send the response to the user */
      if ( dsatsmsi_cmd_sm.list_begin )
      {
        resp_state = DSAT_COMPLETE_RSP;
        dsatsmsi_cmd_sm.list_begin = FALSE;
      }
      else
      {
        resp_state = DSAT_END_OF_RSP;
      }
      dsatcmdp_handle_async_cmd_rsp(di_ptr, resp_state);

      /* We are done with the commands, clean up */
      DSAT_SMS_STATE_CLEAN_ALL();
      goto send_result;
    }
    
    /* Unpack the User Payload from Raw Data */
    if((cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_ASCII) ||
       (cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_IA5)|| 
       (cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_GSM_7_BIT_DEFAULT) )
    {
      cl_bd_data->user_data.data_len =
         wms_ts_unpack_ascii(
                     (wms_cdma_user_data_s_type*)&cl_ts_data->u.cdma.user_data,
                                cl_ts_data->u.cdma.user_data.number_of_digits+1,
                                cl_bd_data->user_data.data);
    }
    if((cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_LATIN) ||
		   (cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_LATIN_HEBREW))
    {
      cl_bd_data->user_data.data_len = MIN(WMS_CDMA_USER_DATA_MAX,
											  cl_ts_data->u.cdma.user_data.data_len);

      (void) dsatutil_memscpy((void*)cl_bd_data->user_data.data,WMS_CDMA_USER_DATA_MAX,
                              (void*)cl_ts_data->u.cdma.user_data.data,
                              MIN(WMS_CDMA_USER_DATA_MAX,
                              cl_bd_data->user_data.data_len));
    }

#ifdef FEATURE_DSAT_EXTENDED_SMS 
    else if ( (cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE) 
              || (cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_OCTET)
             || (cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_GSM_7_BIT_DEFAULT) )
    {
      cl_bd_data->user_data.data_len = MIN(WMS_CDMA_USER_DATA_MAX,
                                          cl_ts_data->u.cdma.user_data.data_len);

      (void) dsatutil_memscpy((void*)cl_bd_data->user_data.data,WMS_CDMA_USER_DATA_MAX,
                              (void*)cl_ts_data->u.cdma.user_data.data,
                              MIN(WMS_CDMA_USER_DATA_MAX,
                              cl_bd_data->user_data.data_len));
    }
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    DS_AT_MSG1_HIGH(" Encoding Scheme found %d",
                   cl_ts_data->u.cdma.user_data.encoding);
    
    di_ptr->used = (uint16)(data_ptr - di_ptr->data_ptr);

    if((!dsatsmsi_cmd_sm.list_begin) &&
       ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL ))
    {
      /* This is not the first message             */
      /* Seperate the next message with a <CR><LF> */
      *data_ptr++ = '\n';
    }
    data_ptr += (word)snprintf((char*)data_ptr, di_ptr->size,
                              "%s: ", dsatsmsi_pres_cmd.cmd_hdr);
    if (dsatsmsi_pres_cmd.cmd_name == SMS_CMGL)
    {
      data_ptr = dsatutil_itoa( 
                    dsatsmsi_cmd_sm.indices[dsatsmsi_cmd_sm.present_index - 1],
                    data_ptr,10);
      *data_ptr++ = ',';
    }
#ifdef FEATURE_DSAT_EXTENDED_SMS
    if ( TRUE == dsatsmsi_pres_cmd.is_hc_cmd )
    {
      /* Update the result with stat */
      data_ptr += (word)snprintf((char*)data_ptr, di_ptr->size,
                                       "%d", 
                                       dsatsmsi_tag_lookup_ext(event_info->u.
                                        msg_status.msg_hdr.tag));
      *data_ptr++ = ',';
      /* Update the Result with <oa> */
      data_ptr += (word)snprintf((char*)data_ptr, di_ptr->size,
                                       "%s",
                        event_info->u.msg_status.u.cdma_message.address.digits);
    }
    else
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    {
       /* Update the Result with <stat> */
       data_ptr = dsatsmsi_tag_lookup ( 
         event_info->u.msg_status.msg_hdr.tag, 0, data_ptr );
       *data_ptr++ = ','; 

       /* Update the Result with <oa> */
       data_ptr += (word)snprintf((char*)data_ptr, di_ptr->size,
                                       "\"%s\"",
                event_info->u.msg_status.u.cdma_message.address.digits);
    }

    *data_ptr++ = ','; 
    /* Update the Result with <scts> */
    data_ptr = dsatsmsi_timestamp( cl_ts_data->u.cdma.mc_time ,data_ptr,
                                   dsatsmsi_pres_cmd.is_hc_cmd);
    *data_ptr++ = ',';

    if( event_info->u.msg_status.u.cdma_message.address.number_type 
                                          == WMS_NUMBER_INTERNATIONAL)
    {
      data_ptr += (word)snprintf((char*)data_ptr, di_ptr->size,
                                ",145,");
    }
    else
    {
      data_ptr += (word)snprintf((char*)data_ptr, di_ptr->size,
                                ",129,");
    }

#ifdef FEATURE_DSAT_EXTENDED_SMS
    if (( TRUE == dsatsmsi_pres_cmd.is_hc_cmd ) &&
        ( SMS_CMGR == dsatsmsi_pres_cmd.cmd_name ))
    {
      /* Update the Result with <lang> */
      data_ptr = dsatutil_itoa((uint32) cl_ts_data->u.cdma.language, data_ptr, 10);
      *data_ptr++ = ',';
    
      /* Update the Result with <format> */
      data_ptr = 
        dsatutil_itoa(
        (uint32) encoding707_lookup_ext(cl_ts_data->u.cdma.user_data.encoding),
        data_ptr, 10);
      *data_ptr++ = ',';
      
      /* Update the Result with <length> */
      data_ptr = dsatutil_itoa((uint32) 
                               cl_bd_data->user_data.data_len, 
                               data_ptr,
                               10);
      *data_ptr++ = ',';
    
      /* Update the Result with <priority> */
      data_ptr = dsatutil_itoa((uint32) cl_ts_data->u.cdma.priority, 
                               data_ptr, 10);
      *data_ptr++ = ',';
    
      /* Update the Result with <privacy> */
      data_ptr = dsatutil_itoa((uint32) cl_ts_data->u.cdma.privacy, 
                               data_ptr, 10);
      *data_ptr++ = ',';
    
      /* Update the Result with <type>  */
      if (cl_ts_data->u.cdma.message_id.type == WMS_BD_TYPE_DELIVERY_ACK) 
      {
        /* SMS ACKs should show type = 3; 
         * ACKs have teleservice-type same as normal SMS messages. 
         */
        data_ptr = dsatutil_itoa(3, data_ptr, 10);
      }
      else
      {
        data_ptr = dsatutil_itoa(
          (uint32) teleservice707_lookup_ext(
                      event_info->u.msg_status.u.cdma_message.teleservice),
                      data_ptr, 10);
      }
      *data_ptr++ = ',';
    
      /* Update the Result with <stat> */
      data_ptr += (word)snprintf((char*)data_ptr, di_ptr->size,
                                       "%d", 
                                       dsatsmsi_tag_lookup_ext(
                                       event_info->u.msg_status.msg_hdr.tag));
    }
    else
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    {
      /* Update the Result with <length> */
      data_ptr = dsatutil_itoa((uint32) cl_bd_data->user_data.data_len, 
                                data_ptr,
                               10);
    }
#ifdef FEATURE_DSAT_EXTENDED_SMS
    /* when UNICODE encoding is used, the dsat_fmt_response routine bypasses adding <CR> */
    if (cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE) 
    {
      *data_ptr++ = '\r';
    }
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    *data_ptr++ = '\n';
    
    di_ptr->used = (uint16) ( data_ptr - di_ptr->data_ptr );
    /* Update the Result with <data> */
    if(cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_OCTET 
       || cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_ASCII 
       || cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_GSM_7_BIT_DEFAULT 
       || cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_LATIN
       || cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_LATIN_HEBREW
#ifdef FEATURE_DSAT_EXTENDED_SMS
       || cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE
#endif /* FEATURE_DSAT_EXTENDED_SMS */
      )
    {
      dsat_dsm_pushdown_tail(&di_ptr, 
                             cl_bd_data->user_data.data,
                             cl_bd_data->user_data.data_len,
                             DSM_ITEM_POOL(di_ptr), 
                             FALSE);

      len = cl_bd_data->user_data.data_len;
      DS_AT_MSG1_MED("Received SMS length is %d",len);
    }
#ifdef FEATURE_DSAT_EXTENDED_SMS
    if ( TRUE == dsatsmsi_pres_cmd.is_hc_cmd )
    {
      data_ptr =  di_ptr->data_ptr + di_ptr->used;
      if (len > 0) 
      {
        if (cl_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_UNICODE) 
        {
          *data_ptr++ = 0x00;
        }
        *data_ptr++ = 0x1a;
      }
      di_ptr->used = (uint16) ( data_ptr - di_ptr->data_ptr );
    }
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    /* CMGL specific Processing */
    if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL )
    {
      /* check to see if we keep going or dump the result */
      if(dsatsmsi_cmd_sm.present_index >= dsatsmsi_cmd_sm.num_of_indices)
      {
        if (dsatsmsi_cmd_sm.num_of_stat <= dsatsmsi_cmd_sm.present_stat)
        {
          /* We are done with the commands, clean up */
          if ( dsatsmsi_cmd_sm.list_begin )
          {
            resp_state = DSAT_COMPLETE_RSP;
            dsatsmsi_cmd_sm.list_begin = FALSE;
          }
          else
          {
            resp_state = DSAT_END_OF_RSP;
          }
          DSAT_SMS_STATE_CLEAN_ALL();
          result = DSAT_OK;
        }
        else
        {
          /* We have more stat to read */
          (void)dsatsmsi_get_msg_list(event_info->subs_id);
          result = DSAT_ASYNC_CMD;
          if ( dsatsmsi_cmd_sm.list_begin )
          {
            resp_state = DSAT_START_OF_RSP;
            dsatsmsi_cmd_sm.list_begin = FALSE;
          }
          else
          {
            resp_state = DSAT_INTERMEDIATE_RSP;
          }
        }
      }
      else
      {
        /* We are not done with reading the list */
        (void)dsatsmsi_read_msg_list(event_info->subs_id);
        /* Let ATCOP know that we are not done yet */
        if ( dsatsmsi_cmd_sm.list_begin )
        {
          resp_state = DSAT_START_OF_RSP;
          dsatsmsi_cmd_sm.list_begin = FALSE;
        }
        else
        {
          resp_state = DSAT_INTERMEDIATE_RSP;
        }
        result = DSAT_ASYNC_CMD;
      }
      dsatcmdp_handle_async_cmd_rsp(di_ptr, resp_state);
      goto send_result;
    }

      dsatcmdp_handle_async_cmd_rsp(di_ptr, DSAT_COMPLETE_RSP);
#ifdef FEATURE_DSAT_EXTENDED_SMS
    dsat_unicode_response = (cl_ts_data->u.cdma.user_data.encoding == 
                                                         WMS_ENCODING_UNICODE);
    if (dsatsmsi_pres_cmd.cmd_name == SMS_CMGR) 
    {
      if ((hcmgr_mode == 0) && 
          (event_info->u.msg_status.msg_hdr.tag == WMS_TAG_MT_NOT_READ))  
          /* Confirm if this is desired */
      {
#ifdef FEATURE_DUAL_SIM
    (void) wms_msg_ms_modify_tag(
                              dsatsmsi_client_id,
                              event_info->subs_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                              event_info->u.msg_status.msg_hdr.mem_store,
                              event_info->u.msg_status.msg_hdr.index,
                              WMS_TAG_MT_READ
                             );
#else
    (void) wms_msg_modify_tag(
                              dsatsmsi_client_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                              event_info->u.msg_status.msg_hdr.mem_store,
                              event_info->u.msg_status.msg_hdr.index,
                              WMS_TAG_MT_READ
                             );
#endif /* FEATURE_DUAL_SIM */  
      } 
      else if ((hcmgr_mode == 1) && 
               (event_info->u.msg_status.msg_hdr.tag == WMS_TAG_MT_READ))  /* Confirm if this is desired */
      {
#ifdef FEATURE_DUAL_SIM
    (void) wms_msg_ms_modify_tag(
                              dsatsmsi_client_id,
                              event_info->subs_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                              event_info->u.msg_status.msg_hdr.mem_store,
                              event_info->u.msg_status.msg_hdr.index,
                              WMS_TAG_MT_READ
                             );
#else
    (void) wms_msg_modify_tag(
                              dsatsmsi_client_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                               event_info->u.msg_status.msg_hdr.mem_store,
                               event_info->u.msg_status.msg_hdr.index,
                               WMS_TAG_MT_NOT_READ
                             );
#endif /* FEATURE_DUAL_SIM */      
      }
    }
    else
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    {
      /* Check to see if we need to modify the tag of the msg */
      if(event_info->u.msg_status.msg_hdr.tag == WMS_TAG_MT_NOT_READ)
      {
        /* We got a REC UNREAD tag */
        /* Modify it to REC READ   */
#ifdef FEATURE_DUAL_SIM
          (void) wms_msg_ms_modify_tag(
                                    dsatsmsi_client_id,
                                    event_info->subs_id,
                                    dsatsmsi_cmd_cb_func,
                                    (void*)&dsatsmsi_pres_cmd,
                                    event_info->u.msg_status.msg_hdr.mem_store,
                                    event_info->u.msg_status.msg_hdr.index,
                                    WMS_TAG_MT_READ
                                   );
#else
          (void) wms_msg_modify_tag(
                                    dsatsmsi_client_id,
                                    dsatsmsi_cmd_cb_func,
                                    (void*)&dsatsmsi_pres_cmd,
                                    event_info->u.msg_status.msg_hdr.mem_store,
                                    event_info->u.msg_status.msg_hdr.index,
                                    WMS_TAG_MT_READ
                                   );
#endif /* FEATURE_DUAL_SIM */
      }
    }

    /* We are done with the commands, clean up */
    DSAT_SMS_STATE_CLEAN_ALL();
    result = DSAT_OK;

  }
  else if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMSS )
  {
    wms_status_e_type msg_status;
    wms_send_mode_e_type   send_mode;
    
    if ( event_info->u.msg_status.u.cdma_message.raw_ts.format != WMS_FORMAT_CDMA )
    {
      DS_AT_MSG0_HIGH("Unsupported WMS format");
      return DSAT_ERROR;
    }

    /* We are reading a message in order to replace the */
    /* existing <da> with a new one that user gave      */

    replace707_da_msg = (wms_client_message_s_type *) dsat_alloc_memory(sizeof(wms_client_message_s_type),
                                                                        FALSE);
    replace707_da_msg->u.cdma_message.raw_ts = 
                         event_info->u.msg_status.u.cdma_message.raw_ts;
    
    send_mode = WMS_SEND_MODE_CLIENT_MESSAGE;
  
    replace707_da_msg->msg_hdr = event_info->u.msg_status.msg_hdr;
    replace707_da_msg->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;

    if( (dsat_num_item_type)dsatutil_get_val(
                          DSAT707_SMS_QCSMP_IDX,0,0,MIX_NUM_TYPE) < 8 )
    {
      replace707_da_msg->u.cdma_message.teleservice = 
            dsat707smsi_teleservice_num_to_enum(
               (char *) qcsmp_tid_valstr[(dsat_num_item_type)dsatutil_get_val(
                                          DSAT707_SMS_QCSMP_IDX,0,0,MIX_NUM_TYPE)]);
    }
    else
    {
      result = DSAT_ERROR;
      goto send_result;
    }
    replace707_da_msg->u.cdma_message.subaddress.number_of_digits = 0;
    replace707_da_msg->u.cdma_message.is_tl_ack_requested = FALSE;
    replace707_da_msg->u.cdma_message.is_service_present = FALSE;
    replace707_da_msg->u.cdma_message.address = send_addr;
  
#ifdef FEATURE_DUAL_SIM
        msg_status = wms_msg_ms_send(dsatsmsi_client_id,
                                     event_info->subs_id,
                                     dsatsmsi_cmd_cb_func,
                                     (void*)&dsatsmsi_pres_cmd,
                                     send_mode,
                                     replace707_da_msg);
    
#else
        msg_status = wms_msg_send(
                                     dsatsmsi_client_id,
                                     dsatsmsi_cmd_cb_func,
                                     (void*)&dsatsmsi_pres_cmd,
                                     send_mode,
                                     replace707_da_msg
                                    );
#endif /* FEATURE_DUAL_SIM */
  
    if( msg_status != WMS_OK_S )
    {
      result = DSAT_ERROR;
    }
    else
    {
      result = DSAT_ASYNC_CMD;
    }
  }

send_result:
  /*Free the allocated memory for replace707_da_msg*/
  dsatutil_free_memory_ext((void **)&replace707_da_msg);
  dsatutil_free_memory_ext((void **)&cl_ts_data);
  dsatutil_free_memory_ext((void **)&cl_bd_data);
  return result;

} /* is707_read_msg_event_handler */

/*===========================================================================

FUNCTION  IS707_DELETE_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for delete message event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type is707_delete_msg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  wms_memory_store_e_type mem_store;
  wms_message_tag_e_type tag;
  wms_status_e_type msg_status;
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
/*-------------------------------------------------------------------------*/

  DS_AT_MSG2_HIGH("is707_delete_msg_event_handler, pres_cmd: %d HC cmd %d",
                dsatsmsi_pres_cmd.cmd_name,dsatsmsi_pres_cmd.is_hc_cmd);

  ASSERT( event_info != NULL );

  if ( (dsatsmsi_pres_cmd.cmd_name == SMS_NONE) || (!dsatcmdp_is_cdma_cmd()))
  {
    /* unwanted event */
    DS_AT_MSG0_HIGH("Ignoring event");
    return DSAT_ASYNC_EVENT;
  }

  /* Check to see if we are deleting just one index */
  if( ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGD )&& 
     (dsatsmsi_cmd_sm.one_index == 1))
  {
    /* We got a CMGD with one index */
    /* We are done.                 */
    /* Tell ATCOP we are OK         */
    DSAT_SMS_STATE_CLEAN_ALL();
    result = DSAT_OK;
  }
  /* We are deleting with a wild card */
  else 
  {
    /* Are we done with the present stat */
    if ( dsatsmsi_cmd_sm.num_of_indices != 0 )
    {
      mem_store = dsatsmsi_memory_lookup((int)dsatutil_get_val(
                    DSAT707_SMS_QCPMS_IDX,0,0,NUM_TYPE), CPMS_MEM1,CDMA_CMD_MODE);
      tag = dsatsmsi_cmd_sm.stat_list[dsatsmsi_cmd_sm.present_stat-1];
#ifdef FEATURE_DUAL_SIM
            msg_status = wms_cfg_ms_get_message_list(
                                    dsatsmsi_client_id,
                                    event_info->subs_id,
                                    dsatsmsi_cmd_cb_func,
                                    (void *)&dsatsmsi_pres_cmd,
                                    mem_store,
                                    tag
                                   );
#else
      msg_status = wms_cfg_get_message_list(
                                            dsatsmsi_client_id,
                                            dsatsmsi_cmd_cb_func,
                                            (void *)&dsatsmsi_pres_cmd,
                                            mem_store,
                                            tag
                                           );
#endif /* FEATURE_DUAL_SIM */

      /* Always read from the tip */
      dsatsmsi_cmd_sm.present_index = 0;
      if (msg_status == WMS_OK_S)
      {
        /* still more to come */
        /* tell ATCOP about that */
        result = DSAT_ASYNC_CMD;
      }
    }
    else if ( dsatsmsi_cmd_sm.num_of_stat > 1 ) /* Any more stats to delete */
    {
      if(dsatsmsi_cmd_sm.present_stat < dsatsmsi_cmd_sm.num_of_stat)
      {
        mem_store = dsatsmsi_memory_lookup((int)dsatutil_get_val(
                                DSAT707_SMS_QCPMS_IDX,0,0,NUM_TYPE), 
                                              CPMS_MEM1, CDMA_CMD_MODE);
        tag = dsatsmsi_cmd_sm.stat_list[dsatsmsi_cmd_sm.present_stat];
        dsatsmsi_cmd_sm.present_stat++;
        
#ifdef FEATURE_DUAL_SIM
              msg_status = wms_cfg_ms_get_message_list(
                                            dsatsmsi_client_id,
                                            event_info->subs_id,
                                            dsatsmsi_cmd_cb_func,
                                            (void *)&dsatsmsi_pres_cmd,
                                            mem_store,
                                            tag
                                           );
#else
        msg_status = wms_cfg_get_message_list(
                                                dsatsmsi_client_id,
                                                dsatsmsi_cmd_cb_func,
                                                (void *)&dsatsmsi_pres_cmd,
                                                mem_store,
                                                tag
                                               );
#endif /* FEATURE_DUAL_SIM */

        /* Tell ATCOP that there is mode to come */
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        /* We are finally done */
        /* tell ATCOP we are done */
        result =  DSAT_OK;
      }
    }
    else
    {
      /* this is a case of just single index delete or */
      /* we are done deleting messages                 */
      result = DSAT_OK;
    }
  }
  return result;
}/* is707_delete_msg_event_handler */

/*===========================================================================

FUNCTION  IS707_WRITE_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for write message event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_OK : if the command has been successfully executed
    DSAT_ASYNC_EVENT : If no result code is needed (e.g. unwanted event)
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type is707_write_msg_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  uint32         return_val;
  dsm_item_type * res_buff = NULL;
  byte          * data_ptr = NULL;
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  
  ASSERT( event_info != NULL );

  DS_AT_MSG2_HIGH("is707_write_msg_event_handler, pres_cmd: %d HC cmd %d",
                dsatsmsi_pres_cmd.cmd_name,dsatsmsi_pres_cmd.is_hc_cmd);
  if ( dsatsmsi_pres_cmd.cmd_name != SMS_CMGW )
  {
    /* unwanted event */
    return DSAT_ASYNC_EVENT;
  }

  /* We need to send index at which the message was stored */
  return_val = event_info->u.msg_status.msg_hdr.index;

  res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
  res_buff->used = (word)snprintf((char *) res_buff->data_ptr,
                                         res_buff->size,
                                         "%s: ",
                                         dsatsmsi_pres_cmd.cmd_hdr);
  data_ptr = &(res_buff->data_ptr[res_buff->used]);

  data_ptr = dsatsmsi_itoa ((word)return_val, data_ptr, 10);
  /* update the used val */
  res_buff->used = (word)(data_ptr - res_buff->data_ptr);

  /* send the response to the user */
  dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
  /* We are done with the commands, clean up */
  DSAT_SMS_STATE_CLEAN_ALL();
  result = DSAT_OK;

  return result;
} /* is707_write_msg_event_handler */
#ifdef FEATURE_DSAT_EXTENDED_SMS
/*===========================================================================

FUNCTION  TELESERVICE707_LOOKUP_EXT

DESCRIPTION
  Function to convert WMS teleservice type to HC SMS type. 

DEPENDENCIES
  None

RETURN VALUE
  returns an integer type that describes the converted teleservice type.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_num_item_type teleservice707_lookup_ext
(
  wms_teleservice_e_type tele_type
)
{
  dsat_num_item_type ret_val = 0;
  
  DS_AT_MSG1_HIGH("teleservice707_lookup_ext : tele_type (%d) ", tele_type);
  switch (tele_type) 
  {
  case WMS_TELESERVICE_CMT_95:
    ret_val = 0;
    break;
  case WMS_TELESERVICE_CPT_95:
    ret_val = 1;
    break;
  case WMS_TELESERVICE_VMN_95:
    ret_val = 2;
    break;
  default:
    /* @todo : determine the code for SMS <acks> */
    ret_val = 3;
  }

  return ret_val;
}/* teleservice707_lookup_ext */

/*===========================================================================

FUNCTION  ENCODING707_LOOKUP_EXT

DESCRIPTION
  Function to convert WMS encoding type to HC SMS type. 

DEPENDENCIES
  None

RETURN VALUE
  returns an integer type that describes the converted encoding type.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_num_item_type encoding707_lookup_ext
( 
  wms_user_data_encoding_e_type encod_arg
)
{
  switch (encod_arg) 
  {
  case WMS_ENCODING_GSM_7_BIT_DEFAULT:
    return 0;
  case WMS_ENCODING_ASCII:
    return 1;
  case WMS_ENCODING_IA5:
    return 2;
  case WMS_ENCODING_OCTET:
    return 3;
  case WMS_ENCODING_LATIN:
    return 4;
  case WMS_ENCODING_LATIN_HEBREW:
    return 5;
  case WMS_ENCODING_UNICODE:
    return 6;
  default:
    DS_AT_MSG1_MED("Invalid encoding type %d",encod_arg);
    /* Appease lint */
  }
  /* default : ASCII */
  return 1;
}
#endif /* FEATURE_DSAT_EXTENDED_SMS */
#endif /* FEATURE_CDMA_SMS */
