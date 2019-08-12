/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( S M S  C O M M A N D S 
                  A S Y N C )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module is used by SMS AT commands. It mainly contains asynchronous
  event handlers that are used in executing the sms commands.

EXTERNALIZED FUNCTIONS
  dsatetsisms_flush_sms_indications
  Flushes buffer of SMS incoming message indications and sends indications
  to TE. Indications are buffered and sent based on AT parameter setting

EXTERNALIZED FUNCTIONS INTERNAL TO ATCOP SMS UNIT

INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2004 - 2016 by Qualcomm Technologies, Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsismsa.c_v   1.5   12 Jul 2002 10:06:56   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsismsa.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why

--------   ---     ----------------------------------------------------------
08/17/16   skc     Fixed made to delete SM from GW RAM for no message class SM.
12/15/15   skc     Fixed KW Issues
04/09/15   sc      Fixed +CMGL message display issue.
06/27/14   tk      Added support for Dynamic ATCoP.
03/11/14   tk      Optimized debug macros usage in ATCoP.
11/14/13   tk      Fixed issues in online command state.
03/28/13   tk      Mainlined FEATURE_THIN_UI using new NV-EFS item.
11/30/12   tk      ATCoP changes for Triton DSDA.
10/01/12   tk      Fixed +CDS URC issue.
08/22/12   sk      Mainlining Nikel Changes.
07/13/12   tk      Added graceful handling of unsupported WMS formats.
05/18/12   tk      Migrated to MSG 2.0 macros
04/11/12   nc      Fixed operating mode checks for SMS related commands for NikeL targets. 
01/19/12   sk      Feature cleanup.
09/27/11   ht      Included dsut header to support offtarget stubs.
07/03/11   ad      Fixed memory corruption..
02/07/10   ad      Using WMS_WRITE_MODE_INSERT instead of WMS_WRITE_MODE_REPLACE
                   for the first message write to RAM_GW.
10/25/10   ad      Init Changes for DSDS. 
05/10/10   kk      Updating pres_cmd to struct to store more data.
12/15/09   nc      Mainlining FEATURE_DATA_SERIALIZER.
09/10/09   nc      Fixed Klocwork errors. 
04/29/09   ua      Fixed compiler warnings. 
03/11/09   nc      Fixed the crash in read_msg_template_event_handler
                   occurs when default cscs_val is UCS2.
12/02/08   ss      Off target Lint fix.
11/13/08   nc      Moving the SMS MT msg Queue to dsatsms.c
07/23/08   bs      Fixed +CMGL command for reading large SMS.
06/17/08   nc      Added a union sms_msg_mt_info_u to support CDMA mt sms
05/15/08   bs      Fixed possible NULL pointer dereferencing.
03/24/08   bs      Fixed Klocwork criticals.
02/15/08   bs      Fixed +CSCA initial query command
11/04/07   pp      Lint low - fixes.
11/08/07   ua      Using REPLACE mode to write messages into RAM. 
06/08/07   ua      Correcting status report delivery to TE. 
05/02/07   ua      Fixed lint medium errors. 
04/23/07   ua      Fixed lint high errors. 
02/15/07   ss      Fixed lint high errors
02/13/07   pkp     Fixed RVCT compiler warnings.
02/06/07   ua      Correcting CMGL to read Status Reports. 
01/30/07   ss      Replaced Banned string API calls.
12/26/06   ua      Store status report in RAM and send +CDSI to TE 
                   when MO SMS is not stored in SIM. 
11/30/06   ua      Correcting route configurations for CNMI and +CNMA
05/13/05   sb      Enable direct routing of SMS STATUS-REPORT
10/21/05   snb     Remove function local variables of type 
                   wms_client_ts_data_s_type from stack, make single variable 
                   local to file.
05/02/05   sb      Use global variable to replace <da>
04/15/05   sb      Fixed lint errors
03/30/05   dvp     Correcting feature define.
03/02/05   dvp     Fixed bugs in ^SMG[LRO]
02/18/05   dvp     Added command ^SMGO
02/13/05   dvp     Added commands ^SGM[LR]
02/10/05   sb      Accessories related changes.
12/21/04   snb     Only allow processing  and clearing of dsatsmsi_pres_cmd in
                   SMS event handlers when processing related SMS command.
11/18/04   snb     Move GSM-IRA conversion tables and add support for 
                   additional +CSCS character sets.
10/21/04   snb     Add template support to SMS code and correct CMS ERROR
                   response.
09/03/04   snb     Add support for +CMMS command, corrected 
                   delete_msg_event_handler to ignore these events if not in 
                   +CMGD command.
06/03/04   snb     Changed field name to match change in WMS definition.
02/24/04   snb     Added support for ATCoP SMS handling via signals, flow 
                   control of received SMS messages.
11/28/03   sb      CDMA SMS changes
10/22/03   sb      CB integration changes
09/09/03   sb      Fix for ECR33797 and CR34269: Do not expect an ACK for an
                   message with WMS_GW_MSG_WAITING_DISCARD
08/24/03   sb      Cell broadcast editions (first cut)
08/13/03   sb      Avoid duplication of write events for incoming message.
                   Added event callback filters.
04/17/03   sb      Event payload optimization
04/08/03   sb      Status-report and command support
04/01/03   sb      New WMS data structure for status reports
02/10/03   wx      add support for +CGSMS command
12/13/02   wx      fix incorrect pointer advance in 
                   send_async_indication_to_te
11/14/02   wx      Handle WMS_CMD_ERR_MSG_SEND_MODE error.
09/26/02   sb      Code review changes
09/10/02   snb     More robust and thorough mapping of wms errors to 
                   +CMS ERRORs.
09/06/02   sb      Flushing of SMS only when we are in COMMAND mode.
                   MSG_HIGHs for Async events
06/27/02   wx      Request memory status one after another for +CPMS
02/07/02   sb      Created module.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_ETSI_SMS
/* this file should be included only for SMS in GSM or WCDMA mode */
#include "dsati.h"
#include "dsatetsismsi.h"
#include "dsatetsictab.h"
#include "amssassert.h"
#include "dstask.h"
#include "msg.h"
#include "err.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#include <stringl/stringl.h>
#include "stdio.h"
#include "queue.h"

/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains reginal definitions for external variables 
  that are declared else where in ATCOP SMS SUB SYSTEM
===========================================================================*/
/*--------------------------------------------------------------------------
  import variable from other files in sms module
---------------------------------------------------------------------------*/

/* Variable used in CMGL SM */
extern sms_cmd_sm_type  dsatsmsi_cmd_sm;

/* Variable to find out what command is being processed currently */
extern dsat_sms_cmd_s_type  dsatsmsi_pres_cmd;

/* WMS client (ATCOP) ID */
extern wms_client_id_type dsatsmsi_client_id;


/* Variable to retain destination address for CMSS command */         
extern wms_address_s_type   dsatetsismsi_send_addr;

#ifdef FEATURE_ETSI_SMS_TEMPLATE
extern boolean dsatetsismsi_template_present;
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.
===========================================================================*/

/* LOCAL DEFINITIONS: */


/*-------------------------------------------------------------------------
  SMS Mobile Terminated message queue node definition
---------------------------------------------------------------------------*/ 
/* Flag to indicate if we are in a waiting for an ACK state */
extern boolean waiting_for_an_ack;

#ifdef FEATURE_ETSI_SMS_TEMPLATE
/* Variable to write SCA, PID and DCS to templates */
extern wms_client_message_s_type *template_ptr;
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

/* LOCAL DECLARATIONS: */
/* Variable to retain  message reference number for send command */
LOCAL dsat_num_item_type  send_mr;

#if 0 /* Appease lint, might be of a future use */
/* Variable to retain  message reference number for status report */
LOCAL wms_message_index_type    send_msg_index = 0;

/* Variable to retain  message reference number for send command */
LOCAL wms_memory_store_e_type    send_msg_mem = WMS_MEMORY_STORE_NONE;
#endif

#ifdef FEATURE_ETSI_SMS_TEMPLATE
LOCAL wms_gw_template_s_type *read_templ_ptr;

LOCAL dsat_string_item_type dsat_csca_sim_sca[CSCA_SCA_STR_MAX_LEN+1]
                                              = "NOT DEFINED";
LOCAL dsat_string_item_type dsat_csca_sim_tosca[CSCA_TOSCA_STR_MAX_LEN+1];

LOCAL dsat_mixed_param_val_type dsat_csca_sim_val[2] = {
  { dsat_csca_sim_sca },
  { dsat_csca_sim_tosca }
};

LOCAL dsat_string_item_type dsat_csca_nv_sca[CSCA_SCA_STR_MAX_LEN+1]
                                              = "NOT DEFINED";
LOCAL dsat_string_item_type dsat_csca_nv_tosca[CSCA_TOSCA_STR_MAX_LEN+1];

LOCAL dsat_mixed_param_val_type dsat_csca_nv_val[2] = {
  { dsat_csca_nv_sca }, 
  { dsat_csca_nv_tosca }
};
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

/* PROTOTYPES FOR LOCAL FUNCTIONS: */

LOCAL dsat_result_enum_type send_msg_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

LOCAL dsat_result_enum_type ack_msg_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

LOCAL dsat_result_enum_type read_msg_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

LOCAL dsat_result_enum_type delete_msg_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

LOCAL dsat_result_enum_type modify_msg_tag_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

LOCAL dsat_result_enum_type read_msg_template_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

LOCAL dsat_result_enum_type write_msg_template_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

LOCAL dsat_result_enum_type delete_msg_template_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

extern dsat_result_enum_type received_msg_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

LOCAL dsat_result_enum_type submit_msg_report_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

extern dsat_result_enum_type status_msg_report_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

extern dsat_result_enum_type mt_message_error_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

LOCAL dsat_result_enum_type status_report_read_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

extern dsat_result_enum_type status_report_write_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

LOCAL dsat_result_enum_type replace_da
(
 wms_client_ts_data_s_type        * tpdu_msg_ptr,   /* Ptr to the TPDU         */
 const sms_msg_status_info_s_type * status_msg_ptr, /* Ptr to msg from WMS   */
 sys_modem_as_id_e_type             subs_id
);

extern void modify_tag
(
 const sms_msg_status_info_s_type * status_msg_ptr,  /* Ptr to msg from WMS     */
 sys_modem_as_id_e_type             subs_id
);

/* the Message Event Handler lookup table */
/* note that the order of the functions has to match wms_msg_event_e_type */
const msg_handler_tab_type dsatetsisms_msg_handler_table_entries[] = 
{
  { SMS_CFG_EVENT_GW_READY,         dsatsmsi_gw_ready_cfg_event_handler         },
  { SMS_CFG_EVENT_ROUTES,           dsatsmsi_route_cfg_event_handler            },
  { SMS_CFG_EVENT_MEMORY_STATUS,    dsatsmsi_memory_status_cfg_event_handler_ex },
  { SMS_CFG_EVENT_MESSAGE_LIST,     dsatsmsi_message_list_cfg_event_handler     },
  { SMS_CFG_EVENT_LINK_CONTROL,     dsatsmsi_link_ctrl_event_handler  },
#ifdef FEATURE_ETSI_SMS_PS
  { SMS_CFG_EVENT_GW_DOMAIN_PREF,   dsatsmsi_gw_domain_pref_event_handler       },
#endif /* FEATURE_ETSI_SMS_PS */
#ifdef FEATURE_ETSI_SMS_CB
  { SMS_BC_MM_EVENT_ADD_SRVS,        dsatsms_cb_handler                         },
  { SMS_BC_MM_EVENT_DELETE_SRVS,     dsatsms_cb_handler                         },
  { SMS_BC_MM_EVENT_PREF,            dsatsms_cb_handler                         },
  { SMS_BC_MM_EVENT_DELETE_ALL_SRVS, dsatsms_cb_handler                         },
  { SMS_BC_MM_EVENT_SRV_IDS,         dsatsms_cb_handler                         },
#endif /* FEATURE_ETSI_SMS_CB */

  { SMS_MSG_EVENT_SEND,               send_msg_event_handler                    },
  { SMS_MSG_EVENT_ACK,                ack_msg_event_handler                     },
  { SMS_MSG_EVENT_READ,               read_msg_event_handler                    },
  { SMS_MSG_EVENT_WRITE,              write_msg_event_handler_ex                },
  { SMS_MSG_EVENT_DELETE,             delete_msg_event_handler                  },
  { SMS_MSG_EVENT_MODIFY_TAG,         modify_msg_tag_event_handler              },
  { SMS_MSG_EVENT_READ_TEMPLATE,      read_msg_template_event_handler           },
  { SMS_MSG_EVENT_WRITE_TEMPLATE,     write_msg_template_event_handler          },
  { SMS_MSG_EVENT_DELETE_TEMPLATE,    delete_msg_template_event_handler         },
  { SMS_MSG_EVENT_RECEIVED_MESSAGE,   received_msg_event_handler                },
  { SMS_MSG_EVENT_SUBMIT_REPORT,      submit_msg_report_event_handler           },
  { SMS_MSG_EVENT_STATUS_REPORT,      status_msg_report_event_handler           },
  { SMS_MSG_EVENT_MT_MESSAGE_ERROR,   mt_message_error_event_handler            },
  { SMS_MSG_EVENT_READ_STS_REPORT,    status_report_read_event_handler          },
  { SMS_MSG_EVENT_WRITE_STS_REPORT,   status_report_write_event_handler         },
};

const dsatsms_msg_handler_tab_type dsatetsisms_msg_handler_table =
{
  dsatetsisms_msg_handler_table_entries,
  ARR_SIZE(dsatetsisms_msg_handler_table_entries)
};

/* FUNCTION DEFINITIONS: */

/**********************LOCAL UTILITY FUNCTIONS******************************/

/*===========================================================================

FUNCTION REPLACE_DA

DESCRIPTION
  Local function that is used for +CMSS command execution.
  Checks to see if the message is SMS-SUBMIT or SMS-COMMAND.
  If it is SMS-SUBMIT, replaces <da>.
  else just sends th message

DEPENDENCIES
  None

RETURN VALUE
Returns enum to indicate if it is right or wrong
  DSAT_ERROR: if there is an error in execution
  DSAT_ASYNC_CMD: if everything went well

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type replace_da
(
 wms_client_ts_data_s_type  * tpdu_msg_ptr,   /* Ptr to the TPDU       */
 const sms_msg_status_info_s_type * status_msg_ptr,  /* Ptr to msg from WMS */
 sys_modem_as_id_e_type   subs_id /* Subscription id */
)
{
  wms_status_e_type      msg_status;
  wms_raw_ts_data_s_type *raw_ts_data_ptr = NULL;
  wms_send_mode_e_type   send_mode;
  dsat_result_enum_type   result = DSAT_OK;

  /* Data structure used primarily to replace the <da> */
  wms_client_message_s_type *replace_da_msg = NULL;

  dsatsmsi_pres_cmd.cmd_name = SMS_CMSS;

  /* Replace <da> if it is a SMS-SUBMIT */
  if ( tpdu_msg_ptr->u.gw_pp.tpdu_type == WMS_TPDU_SUBMIT )
  {
    /* We have a SMS-SUBMIT            */
    /* replace da and send the message */
    tpdu_msg_ptr->u.gw_pp.u.submit.address = dsatetsismsi_send_addr;
  }

  /* send the message only if the message is a SUBMIT or COMMAND */
  if ( tpdu_msg_ptr->u.gw_pp.tpdu_type == WMS_TPDU_SUBMIT ||
       tpdu_msg_ptr->u.gw_pp.tpdu_type == WMS_TPDU_COMMAND 
     )
  {
    replace_da_msg = (wms_client_message_s_type *) 
                    dsat_alloc_memory(sizeof(wms_client_message_s_type), FALSE);

    raw_ts_data_ptr = (wms_raw_ts_data_s_type *) 
                    dsat_alloc_memory(sizeof(wms_client_message_s_type), FALSE);

    msg_status = wms_ts_encode(
                                  tpdu_msg_ptr,
                                  raw_ts_data_ptr
                                 );

    replace_da_msg->u.gw_message.raw_ts_data = *raw_ts_data_ptr;
    send_mode = WMS_SEND_MODE_CLIENT_MESSAGE;

    replace_da_msg->msg_hdr = status_msg_ptr->msg_hdr;
    replace_da_msg->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
    
    replace_da_msg->u.gw_message.sc_address = 
      status_msg_ptr->u.gw_message.sc_address;
#ifdef FEATURE_DUAL_SIM
    msg_status = wms_msg_ms_send(
                                 dsatsmsi_client_id,
                                 subs_id,
                                 dsatsmsi_cmd_cb_func,
                                 (void*)&dsatsmsi_pres_cmd,
                                 send_mode,
                                 replace_da_msg
                                );

#else
    msg_status = wms_msg_send(
                                 dsatsmsi_client_id,
                                 dsatsmsi_cmd_cb_func,
                                 (void*)&dsatsmsi_pres_cmd,
                                 send_mode,
                                 replace_da_msg
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

    dsatutil_free_memory((void *)replace_da_msg);
    dsatutil_free_memory((void *)raw_ts_data_ptr);
  }
  else
  {
    /* We have a wrong TPDU type at the index specified by the user */
    /* return an error                                              */
    result = DSAT_ERROR;
  }
  return result;
}/* replace_da */

#ifdef FEATURE_ETSI_SMS_CB
/*===========================================================================

FUNCTION SEND_CB_MSG_TO_TE

DESCRIPTION
  Formats and sends the cell broadcast message to TE> used in both text 
  and PDU modes.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void send_cb_msg_to_te
(
  const wms_raw_ts_data_s_type  *pdu,      /* Ptr to CB PDU             */
  wms_client_ts_data_s_type     *msg,      /* Ptr to decoded CB message */
  sys_modem_as_id_e_type         subs_id   /* Subscription identifier */
)
{
    byte *data_ptr;
    dsm_item_type * res_buff;
    dsm_item_type * formatted_res_buff;
    uint32 i;
    /* uint8 gsm7_val; */

  res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
  if (NULL != res_buff)
  {
    data_ptr = res_buff->data_ptr;
    res_buff->used += (word)snprintf((char*)data_ptr,
                                           res_buff->size - res_buff->used,
                                           "+CBM: ");     
    data_ptr += res_buff->used;
   
    if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) == 0 )
    {
      /* We are in PDU mode */

      data_ptr = dsatutil_itoa((uint32)pdu->len, data_ptr, 10);
      
      *data_ptr++ = '\n';

      for( i=0; i < pdu->len; i++)
      {
        if( pdu->data[i] < 16 )
        {
          *data_ptr++ = '0';
        }
        
        data_ptr = dsatutil_itoa( (uint32) pdu->data[i], data_ptr, 16 );
      }
      *data_ptr = '\0';
      res_buff->used = (word)(data_ptr - res_buff->data_ptr);
    }
    else
    {
      /* We are in Text Mode */
      data_ptr = dsatutil_itoa
        ( msg->u.gw_cb.cb_page_hdr.raw_serial_number, data_ptr, 10 );
      *data_ptr++ = ',';
      data_ptr = dsatutil_itoa
        ( msg->u.gw_cb.cb_page_hdr.cb_srv_id, data_ptr, 10 );
      *data_ptr++ = ',';
      data_ptr = dsatutil_itoa
        ( msg->u.gw_cb.cb_page_hdr.cb_dcs.raw_dcs_data, data_ptr, 10 );
      *data_ptr++ = ',';
      data_ptr = dsatutil_itoa
        ( msg->u.gw_cb.cb_page_hdr.page_number, data_ptr, 10 );
      *data_ptr++ = ',';
      data_ptr = dsatutil_itoa
        ( msg->u.gw_cb.cb_page_hdr.total_pages, data_ptr, 10 );
      *data_ptr++ = '\n';

      if ( msg->u.gw_cb.cb_page_hdr.cb_dcs.alphabet == 
                                           WMS_GW_ALPHABET_7_BIT_DEFAULT )
      {
        if ( msg->u.gw_cb.cb_page_hdr.cb_dcs.language == 
                                              WMS_GW_CB_LANGUAGE_ISO_639 )
        {
          /* future use
          for (i=0; i<2; i++)
          {
            gsm7_val = 
              gsm7_to_ira[msg->u.gw_cb.cb_page_hdr.cb_dcs.iso_639_lang[i]];
          }
          */
          *data_ptr++ = '\n';
        }

        data_ptr = 
          dsatetsismsi_decode_data( msg->u.gw_cb.user_data.sm_data,
                                    (uint8)msg->u.gw_cb.user_data.sm_len,
                                    TRUE,
                                    data_ptr );
      }
      else
      {
        if ( msg->u.gw_cb.cb_page_hdr.cb_dcs.language == 
                                              WMS_GW_CB_LANGUAGE_ISO_639 )
        {
          for (i=0; i<2; i++)
          {
            data_ptr = dsatutil_itoa( 
                       (uint32)msg->u.gw_cb.cb_page_hdr.cb_dcs.iso_639_lang[i], 
                       data_ptr, 16 );
          }
        }

        data_ptr = 
          dsatetsismsi_decode_data( msg->u.gw_cb.user_data.sm_data,
                                    (uint8)msg->u.gw_cb.user_data.sm_len,
                                    FALSE,
                                    data_ptr );
      }
      
      if(data_ptr != NULL)
      {
        *data_ptr = '\0';
        res_buff->used = (word)(data_ptr - res_buff->data_ptr);
      }
      else
      {
        DS_AT_MSG0_ERROR("Not an error, but total Response is stripped");
      }
    }

    /* We are done with filling up response buffer */
    formatted_res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
    /* send the response to the user */
    dsat_fmt_response( &res_buff, DSAT_COMPLETE_RSP, FALSE, 
                       &formatted_res_buff );

    if (NULL != formatted_res_buff)
    {
      /* Flush the queue to the Rm interface. */
      dsatcmdp_send_queued_cmd_results( );

      /* Check the formatted response data item. If it is not empty, send it
         to the Rm interface. */
      if ( formatted_res_buff->used != 0 )
      {
        /* If the accessory feature is enabled, inform SIOLIB which port 
           this URC should go to */
        ds3g_siolib_set_tx_type 
          ( DS3G_SIOLIB_UNSOLICTED_SMS, DS3G_SIOLIB_DATA_PORT ); 

        dsatrsp_send_response( formatted_res_buff, TRUE );
      }
      else
      {
        (void) dsm_free_buffer( formatted_res_buff );
      }
    }
  }
}
#endif /* FEATURE_ETSI_SMS_CB */

/******************** the Message Event Handler functions *******************/
/*===========================================================================

FUNCTION  SEND_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for send message event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_OK : if the command has been successfully executed
    DSAT_ASYNC_CMD : if command is successfully sent to WMS
    DSAT_ASYNC_EVENT : If no result code is needed (e.g. unwanted event)
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type send_msg_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  unsigned int used_val;
  dsat_result_enum_type result = DSAT_OK;
  wms_status_e_type decode_status;
  dsm_item_type * res_buff;
  wms_client_ts_data_s_type *temp_ts_data = NULL;

  if ( dsatsmsi_pres_cmd.cmd_name != SMS_CMGS &&
       dsatsmsi_pres_cmd.cmd_name != SMS_CMSS &&
       dsatsmsi_pres_cmd.cmd_name != SMS_CMGC &&
       dsatsmsi_pres_cmd.cmd_name != SMS_CNMA )
  {
    /* unwanted event */
    return DSAT_ASYNC_EVENT;
  }

  DS_AT_MSG0_HIGH("ATCOP SMS: Send Message Event Handler");

  event_info->u.msg_status.u.gw_message.raw_ts_data.format = WMS_FORMAT_GW_PP;
  
  temp_ts_data = (wms_client_ts_data_s_type *) 
                      dsat_alloc_memory (sizeof(wms_client_ts_data_s_type), FALSE);

  decode_status = wms_ts_decode(&event_info->u.msg_status.u.gw_message.raw_ts_data, 
                                temp_ts_data);

  if ( decode_status == WMS_OK_S )
  {
    /* Retain Useful information from thie event            */
    /* Wait for REPORT from SC and then send it to the user */

    /* SUBMIT-REPORT */
    send_mr = temp_ts_data->u.gw_pp.u.submit.message_reference;

	#if 0 /* Appease lint, might be of a future use */
    /* STATUS-REPORT */
    send_msg_index = event_info->u.msg_status.msg_hdr.index;
    send_msg_mem = event_info->u.msg_status.msg_hdr.mem_store;
    #endif
    result = DSAT_ASYNC_CMD;
  }
  else
  {
    res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    /* We got a error in decoding message */
    used_val = dsatsmsi_cms_error(res_buff, (int)DSAT_CMS_UNKNOWN_ERR);
    res_buff->used = (byte)used_val;
    result = DSAT_CMD_ERR_RSP;
    /* send the response to the user */
    dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);

    /* We are done with the commands, clean up */
    DSAT_SMS_STATE_CLEAN_ALL();
  }

  dsatutil_free_memory((void *)temp_ts_data);
    
  return result;
}/* send_msg_event_handler */

/*===========================================================================

FUNCTION  ACK_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for ack message event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_OK : if the command has been successfully executed
    DSAT_ASYNC_EVENT : If no result code is needed (e.g. unwanted event)

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type ack_msg_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  if ( dsatsmsi_pres_cmd.cmd_name == SMS_CNMA )
  {
    DS_AT_MSG0_HIGH("ATCOP SMS: Ack Message Event Handler");
    /* The ACK is successfully sent so reset this variable*/
    waiting_for_an_ack = FALSE;
    DSAT_SMS_STATE_CLEAN();
    return DSAT_OK;
  }
  else
  {
    return DSAT_ASYNC_EVENT;
  }
} /* ack_msg_event_handler */

/*===========================================================================

FUNCTION  READ_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for read message event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS
    DSAT_ASYNC_EVENT : If no result code is needed (e.g. unwanted event)
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type read_msg_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsm_item_type * res_buff = NULL;
  byte * buff_ptr;
  unsigned int used_val;
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  dsat_rsp_enum_type resp_state;
  wms_status_e_type status;
  dsat_err_e_type   err_info = DSAT_ERR_NO_ERROR;

  wms_client_ts_data_s_type *temp_ts_data = NULL;

  if ( dsatsmsi_pres_cmd.cmd_name != SMS_CMGR &&
       dsatsmsi_pres_cmd.cmd_name != SMS_CMGL &&
       dsatsmsi_pres_cmd.cmd_name != SMS_CMSS &&
       dsatsmsi_pres_cmd.cmd_name != SMS_CNMA )
  {
    /* unwanted event */
    return result;
  }

  DS_AT_MSG0_HIGH("ATCOP SMS: Read Message Event Handler");

  /* Check whether the received WMS format is unsupported and if yes handle gracefully */
  if ( event_info->u.msg_status.u.gw_message.raw_ts_data.format != WMS_FORMAT_GW_PP )
  {
    DS_AT_MSG0_HIGH("Unsupported WMS format");

    result  = DSAT_OK;
    err_info = DSAT_ERR_NO_ERROR;

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
            res_buff = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
            /* Add trailer at the end of response */
            res_buff->used = 0;
            dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_END_OF_RSP);
          }
        }
        else
        {
          /* We have more stat to read */
          (void)dsatsmsi_get_msg_list(event_info->subs_id);
          return DSAT_ASYNC_CMD;
        }
      }
      else
      {
        /* We are not done with reading the list */
        (void)dsatsmsi_read_msg_list(event_info->subs_id);
        return DSAT_ASYNC_CMD;
      }
    }
    else if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMSS )
    {
      result   = DSAT_ERROR;
      err_info = DSAT_ERR_INVALID_FORMAT;
    }
    else if ( dsatsmsi_pres_cmd.cmd_name == SMS_CNMA )
    {
      goto send_event;
    }

    /* We are done with the commands, clean up */

    /* We are returning here for +CMGL and +CMSS command
       1) We will be returning DSAT_OK for +CMGL
       2) or We will be returning DSAT_ERROR for +CMSS*/
    goto send_result;
  }

  /* get the DSM item */
  res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
  buff_ptr = res_buff->data_ptr;

  temp_ts_data = (wms_client_ts_data_s_type *) 
                      dsat_alloc_memory (sizeof(wms_client_ts_data_s_type), FALSE);

  /* decode the message */
  status = wms_ts_decode( &event_info->u.msg_status.u.gw_message.raw_ts_data, 
                          temp_ts_data );

  if( status != WMS_OK_S )
  {
    /* We got an error while reading the message */
    /* Tell TE about it                          */
    used_val = dsatsmsi_cms_error(res_buff, (int)DSAT_CMS_UNKNOWN_ERR);
    res_buff->used = (byte)used_val;
    result = DSAT_CMD_ERR_RSP;
    /* send the response to the user */
    if ( dsatsmsi_cmd_sm.list_begin )
    {
      resp_state = DSAT_COMPLETE_RSP;
      dsatsmsi_cmd_sm.list_begin = FALSE;
    }
    else
    {
      resp_state = DSAT_END_OF_RSP;
    }

    dsatcmdp_handle_async_cmd_rsp(res_buff, resp_state);

    /* We are done with the commands, clean up */
    err_info = DSAT_ERR_INVALID_OPERATION;
    goto send_result;
  }
  
  /* Read the message only if it is for CMGL or CMGR */
  /* In case of CMGL: first read the message ans then check to see */
  /* if we have to read more messages                              */
  if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL || 
       dsatsmsi_pres_cmd.cmd_name == SMS_CMGR ||
       dsatsmsi_pres_cmd.cmd_name == SMS_CNMA )
  {
    if((!dsatsmsi_cmd_sm.list_begin) && (dsatsmsi_pres_cmd.cmd_name==SMS_CMGL))
    {
      /* This is not the first message             */
      /* Seperate the next message with a <CR><LF> */
      *buff_ptr++ = '\n';
      res_buff->used++;
    }

    /*check to see if it is a TEXT message or a PDU message */
    if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE))
    {
      (void)dsatetsismsi_read_txt_msg( temp_ts_data,
                                       res_buff,
                                       &event_info->u.msg_status );
    }
    else
    {
      (void)dsatetsismsi_read_pdu_msg( &event_info->u.msg_status,
                                       res_buff,
                                       temp_ts_data );
    }
    /* Check to see if we need to modify the tag of the msg */

    modify_tag ( &event_info->u.msg_status,event_info->subs_id );
  }
  else if( dsatsmsi_pres_cmd.cmd_name == SMS_CMSS )
  {
    /* We are reading a message in order to replace the */
    /* existing <da> with a new one that user gave      */
    result = replace_da ( temp_ts_data, 
                          &event_info->u.msg_status,
                          event_info->subs_id);

    (void) dsm_free_buffer( res_buff );
    goto send_event;
  }
  else
  {
    /* We don't care about this event */
    (void) dsm_free_buffer( res_buff );
    goto send_event;
  }

  /* We are done reading the present message */
  /* check to see if we need to read more    */
  if( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL )
  {
    /* We got a CMGL command */
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
        result = DSAT_OK;
        err_info = DSAT_ERR_NO_ERROR;
        dsatcmdp_handle_async_cmd_rsp(res_buff, resp_state);
        goto send_result;
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
    dsatcmdp_handle_async_cmd_rsp(res_buff, resp_state);
    goto send_event;
  }
  else
  {
    if ( dsatsmsi_pres_cmd.cmd_name == SMS_CNMA )
    {
      dsm_item_type * formatted_res_buff = NULL;

      /* We are done with filling up raw response buffer */
      /* Format it and send it to TE                     */
      formatted_res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
      /* send the response to the user */
      dsat_fmt_response( &res_buff, DSAT_COMPLETE_RSP, FALSE, 
                         &formatted_res_buff );
  
      if (NULL != formatted_res_buff)
      {
        /* Flush the queue to the Rm interface. */
        dsatcmdp_send_queued_cmd_results( );

        /* Check the formatted response data item. If it is not empty, send it
           to the Rm interface. */
        if ( formatted_res_buff->used != 0 )
        {
          /* If the accessory feature is enabled, inform SIOLIB which port 
             this URC should go to */
          ds3g_siolib_set_tx_type 
            ( DS3G_SIOLIB_UNSOLICTED_SMS, DS3G_SIOLIB_DATA_PORT ); 

          dsatrsp_send_response( formatted_res_buff, TRUE );
        }
        else
        {
          (void) dsm_free_buffer( formatted_res_buff );
        }
      }

      if ( temp_ts_data->u.gw_pp.u.deliver.dcs.msg_waiting == 
                                            WMS_GW_MSG_WAITING_DISCARD ||
          (temp_ts_data->u.gw_pp.u.deliver.dcs.msg_class == 
                                            WMS_MESSAGE_CLASS_NONE && 
           temp_ts_data->u.gw_pp.u.deliver.dcs.msg_waiting == 
                                            WMS_GW_MSG_WAITING_NONE))
      {
        /* No ACK is needed for this message */
        /* Free the element from the queue and send the next */
        /* In a data card environment this variable will be TRUE (need ACK) */
        /* Refer 27.005 Section 3.4.1 Table 2 */
        boolean ack_required = FALSE;
        dsatetsismsi_free_element( ack_required, event_info->subs_id);
        goto send_event;
      }
      else
      {
        /* In write message event handler if csms=1 then waiting_for_an_ack
        is set to TRUE; but this doesnt hold good in THIN_UI for class 0 msg */
        if ((FALSE == dsatme_is_thin_ui()) &&
            (temp_ts_data->u.gw_pp.u.deliver.dcs.msg_class ==
                                                       WMS_MESSAGE_CLASS_0))
        {
          waiting_for_an_ack = FALSE;
        }
        goto send_event;
      }
    }
    else
    {
      /* We processing a CMGR command   */
      /* no further processing required */
      /* send the response to the user  */
        dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);

      /* We are done with the commands, clean up */
      result = DSAT_OK;
      goto send_result;
    }
  }

/*Needed for async result where we dont have to clean the state.*/
send_event:
  dsatutil_free_memory((void *)temp_ts_data);
  return result;

send_result:
  /* We are done with the commands, clean up */
  DSAT_SMS_STATE_CLEAN_ALL();
  dsatutil_free_memory((void *)temp_ts_data);
  DS_AT_MSG1_HIGH("read_msg_event_handler(), err_info: %d" , err_info);
  return result;

} /* read_msg_event_handler */

/*===========================================================================

FUNCTION  WRITE_MSG_EVENT_HANDLER

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
dsat_result_enum_type write_msg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  uint32                 return_val;
  dsm_item_type         *res_buff;
  byte                  *data_ptr;
  dsat_result_enum_type  result = DSAT_OK;
  byte                   cmgw_name_ptr[] = "+CMGW: ";
  int                    i = 0;
  byte                   c;

  /* We need to send index at which the message was stored */
  return_val = event_info->u.msg_status.msg_hdr.index;

  res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
  data_ptr = res_buff->data_ptr;

  while((c=cmgw_name_ptr[i++]) != '\0')
  {
    *data_ptr++ = c;
  }

  data_ptr = dsatsmsi_itoa ((word)return_val, data_ptr, 10);
  /* update the used val */
  res_buff->used = (byte)(data_ptr - res_buff->data_ptr);

  /* send the response to the user */
  dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
  /* We are done with the commands, clean up */
  DSAT_SMS_STATE_CLEAN_ALL();
  result = DSAT_OK;

  return result;
} /* write_msg_event_handler */

/*===========================================================================

FUNCTION  DELETE_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for delete message event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_OK : if the command has been successfully executed 
    DSAT_ASYNC_CMD : if command is successfully sent to WMS
    DSAT_ASYNC_EVENT : If no result code is needed (e.g. unwanted event)

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type delete_msg_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  wms_memory_store_e_type mem_store;
  wms_message_tag_e_type tag;
  wms_status_e_type msg_status;
  dsat_result_enum_type result = DSAT_OK;

  if(!dsatcmdp_is_etsi_cmd())
  {
    return DSAT_ASYNC_EVENT;
  }
  if ( dsatsmsi_pres_cmd.cmd_name != SMS_CMGD )
  {
    /* unwanted event */
    return DSAT_ASYNC_EVENT;
  }
  
  DS_AT_MSG0_HIGH("ATCOP SMS: Delete Message Event Handler");

  /* Check to see if we are deleting just one index */
  if ( dsatsmsi_cmd_sm.one_index == 1 )
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
      mem_store = dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
       DSATETSI_EXT_ACT_CPMS_ETSI_IDX,event_info->subs_id,0,NUM_TYPE), CPMS_MEM1,ETSI_CMD_MODE);
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
        mem_store = dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
         DSATETSI_EXT_ACT_CPMS_ETSI_IDX,event_info->subs_id,0,NUM_TYPE), CPMS_MEM1,ETSI_CMD_MODE);
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
        DSAT_SMS_STATE_CLEAN_ALL();
        result = DSAT_OK;
      }
    }
    else
    {
      /* this is a case of just single index delete or */
      /* we are done deleting messages                 */
      result = DSAT_OK;
      DSAT_SMS_STATE_CLEAN_ALL();
    }
  }

  return result;
}/* delete_msg_event_handler */

/*===========================================================================

FUNCTION  MODIFY_MSG_TAG_EVENT_HANDLER

DESCRIPTION
  Handler function for modify message tag event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_ASYNC_EVENT : If no result code is needed (e.g. unwanted event)

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type modify_msg_tag_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  /* Just return */
  return DSAT_ASYNC_EVENT;
}/* modify_msg_tag_event_handler */

/*===========================================================================

FUNCTION  READ_MSG_TAG_EVENT_HANDLER

DESCRIPTION
  Handler function for read message tag event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_ASYNC_EVENT : If no result code is needed (e.g. unwanted event)

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type read_msg_template_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
#ifdef FEATURE_ETSI_SMS_TEMPLATE


  if ( (dsatsmsi_pres_cmd.cmd_name == SMS_CSCA) ||
       (dsatsmsi_pres_cmd.cmd_name == SMS_CSMP) )
  {
    return dsatetsisms_process_template_read(&event_info->u.msg_status.u.gw_template);
  }
  else
  {

    read_templ_ptr = &event_info->u.msg_status.u.gw_template;
  
    if ( read_templ_ptr->mask & WMS_GW_TEMPLATE_MASK_SC_ADDR )
    {
      boolean is_csca= TRUE;
      byte sca_ptr[SCA_MAX_LEN] = {0};
      byte no_quotes[SCA_MAX_LEN] = {0};
      uint8 type_of_addr = 0;
      byte *temp_ptr;
      dsat_string_item_type * str_item;
      
      temp_ptr = sca_ptr;
  
      /* We are reading the SCA, save it in our tables */
      temp_ptr = dsatetsismsi_addr_to_str (
                read_templ_ptr->sc_addr,
                temp_ptr,
                &type_of_addr, 
                is_csca,
                (uint8)SCA_MAX_LEN 
                );
      if( temp_ptr == NULL )
      {
        DS_AT_MSG0_ERROR("Reading SCA unsuccessful");
        return DSAT_ERROR;
      }
      *temp_ptr = '\0';
      if ( FALSE ==  dsatsmsi_strip_quotes( sca_ptr, no_quotes, sizeof(no_quotes) ) )
      {
        DS_AT_MSG0_ERROR("Strip Quotes failed");
        return DSAT_ERROR;
      }
  
      if(event_info->u.msg_status.msg_hdr.mem_store == WMS_MEMORY_STORE_SIM)
      {
        (void) strlcpy( (char *)dsat_csca_sim_val[0].string_item,
                          (const char *)no_quotes,
                          CSCA_SCA_STR_MAX_LEN+1);
        (void) dsatutil_itoa
          ((uint32) type_of_addr, dsat_csca_sim_val[1].string_item, 10);
      }
  
      if(event_info->u.msg_status.msg_hdr.mem_store == WMS_MEMORY_STORE_NV_GW)
      {
        (void) strlcpy( (char *)dsat_csca_nv_val[0].string_item,
                          (const char *)no_quotes,
                          CSCA_SCA_STR_MAX_LEN+1);
        (void) dsatutil_itoa
          ((uint32) type_of_addr, dsat_csca_nv_val[1].string_item, 10);
      }
  
      /* SCA address from SIM takes higher priority */
      if(strncmp((const char *)dsat_csca_sim_val[0].string_item,
                     (const char *)"NOT DEFINED",strlen((const char *)"NOT DEFINED")))
      {
        str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,event_info->subs_id,0,MIX_STR_TYPE);
        (void) strlcpy( (char *)str_item,
                          (const char *)dsat_csca_sim_val[0].string_item,
                          CSCA_SCA_STR_MAX_LEN+1);
        str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,event_info->subs_id,1,MIX_STR_TYPE);
        (void) strlcpy( (char *)str_item,
                          (const char *)dsat_csca_sim_val[1].string_item,
                          CSCA_TOSCA_STR_MAX_LEN+1);
      }
      else if(strncmp((const char *)dsat_csca_nv_val[0].string_item,
                     (const char *)"NOT DEFINED",strlen((const char *)"NOT DEFINED")))
      {
        str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,event_info->subs_id,0,MIX_STR_TYPE);
        (void) strlcpy( (char *)str_item,
                          (const char *)dsat_csca_nv_val[0].string_item,
                          CSCA_SCA_STR_MAX_LEN+1);
        str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,event_info->subs_id,1,MIX_STR_TYPE);
        (void) strlcpy( (char *)str_item,
                          (const char *)dsat_csca_nv_val[1].string_item,
                          CSCA_TOSCA_STR_MAX_LEN+1);
      }
    }
    
    if ( read_templ_ptr->mask & WMS_GW_TEMPLATE_MASK_PID )
    {   
      /* We are reading the PID, save it in our tables */
      DSATUTIL_SET_VAL(DSATETSI_EXT_CSMP_IDX,event_info->subs_id,2,0,read_templ_ptr->pid,MIX_NUM_TYPE)
    }
    
    if ( read_templ_ptr->mask & WMS_GW_TEMPLATE_MASK_DCS )
    {   
      uint8 dcs = 0;
      /* We are reading the DCS, save it in our tables */
     (void) wms_ts_encode_dcs( &read_templ_ptr->dcs, &dcs );
      DSATUTIL_SET_VAL(DSATETSI_EXT_CSMP_IDX,event_info->subs_id,3,0,dcs,MIX_NUM_TYPE)
    } 

    if (dsatsmsi_pres_cmd.cmd_name == SMS_TEMPLATE)
    {
      return dsatsms_process_sms_cmd();
    }
  }
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

  return DSAT_ASYNC_EVENT;
}/* read_msg_template_event_handler */

/*===========================================================================

FUNCTION  WRITE_MSG_TEMPLATE_EVENT_HANDLER

DESCRIPTION
  Handler function for write message template event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_ASYNC_EVENT : If no result code is needed (e.g. unwanted event)

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type write_msg_template_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsat_result_enum_type result;

#ifdef FEATURE_ETSI_SMS_TEMPLATE
  if((dsatsmsi_pres_cmd.cmd_name == SMS_CSCA)||
     (dsatsmsi_pres_cmd.cmd_name == SMS_CSMP))
  {
    DSAT_SMS_STATE_CLEAN();
    /* We created the template, use this one from now on */
    dsatetsismsi_template_present = TRUE;
    result = DSAT_OK;
    dsatutil_free_memory_ext((void **)&template_ptr);
  }
  else
#endif /* FEATURE_ETSI_SMS_TEMPLATE */
  {
    result = DSAT_ASYNC_EVENT;
  }

  return result;
}/* write_msg_template_event_handler */

/*===========================================================================

FUNCTION  DELETE_MSG_TEMPLATE_EVENT_HANDLER

DESCRIPTION
  Handler function for delete message template event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_ASYNC_EVENT : If no result code is needed (e.g. unwanted event)

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type delete_msg_template_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
 return DSAT_ASYNC_EVENT;
}/* delete_msg_template_event_handler */

/*===========================================================================

FUNCTION  SUBMIT_MSG_REPORT_EVENT_HANDLER

DESCRIPTION
  Handler function for submit message report event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_OK : if the command has been successfully executed
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type submit_msg_report_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsm_item_type * res_buff;
  byte * data_ptr;
  int used;
  dsat_result_enum_type result = DSAT_OK;

  if ( dsatsmsi_pres_cmd.cmd_name != SMS_CMGS &&
       dsatsmsi_pres_cmd.cmd_name != SMS_CMSS &&
       dsatsmsi_pres_cmd.cmd_name != SMS_CMGC )
  {
    /* unwanted event */
    return DSAT_ASYNC_EVENT;
  }

  DS_AT_MSG0_HIGH("ATCOP SMS: Submit Message Report Event Handler");
  
  res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
  data_ptr =  res_buff->data_ptr;
  if ( event_info->u.submit_report.report_status == WMS_RPT_OK )
  {
    /* We have successfully sent the message */
    /* Just return the message reference number to the user */

    if (dsatsmsi_pres_cmd.cmd_name == SMS_CMGS)
    {
      used = (word)snprintf((char*)data_ptr,res_buff->size,"+CMGS: ");    
    }
    else if (dsatsmsi_pres_cmd.cmd_name == SMS_CMGC)
    {
      used = (word)snprintf((char*)data_ptr,res_buff->size,"+CMGC: ");
    }
    else
    {    
      used = (word)snprintf((char*)data_ptr,res_buff->size,"+CMSS: ");
    }

    data_ptr += used;
  
    data_ptr = dsatsmsi_itoa (send_mr, data_ptr, 10);

    if ( ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CSMS_IDX,
           event_info->subs_id,0,NUM_TYPE) == 1) && 
         ( event_info->u.submit_report.tpdu_type != WMS_TPDU_NONE ) )
    {
      sms_submit_report_ack_type ack;
      int i;

      *data_ptr++ = ',';

      ack = event_info->u.submit_report.u.ack;

      if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) == 1 )
      {
        data_ptr = dsatetsismsi_timestamp( ack.timestamp, data_ptr );
      }
      else
      {
        for( i=0; i<ack.sm_len; i++)
        {
          if( ack.sm_data[i] < 16 )
          {
            *data_ptr++ = '0';
          }
        
          data_ptr = dsatutil_itoa( ack.sm_data[i], data_ptr, 16 );
         }
      }
    }
    res_buff->used =  (byte)(data_ptr - res_buff->data_ptr);
    result = DSAT_OK;
    DSAT_SMS_STATE_CLEAN_ALL();
    send_mr = 0;
  }/* if(success) */
  else
  {
    /* We got an error from RP or CP protocol:
       verify it is valid before attempting to use */
    int i=255;
    int error_cause;
    if ( ( event_info->u.submit_report.report_status == WMS_RPT_RP_ERROR ||
           event_info->u.submit_report.report_status == WMS_RPT_CP_ERROR ) &&
         ( (uint8)event_info->u.submit_report.u.tp_cause > 127 )&&
         ( (uint8)event_info->u.submit_report.u.tp_cause <= i ))/* lint 650 */
    {
      error_cause = (int)event_info->u.submit_report.u.tp_cause;
    }
    else
    {
      /* Internal error                */
      /* Tell TE that it is an unknown */
      error_cause = (int)DSAT_CMS_UNKNOWN_ERR;
    }

    res_buff->used += 
      (byte)dsatsmsi_cms_error(res_buff, error_cause);
    result = DSAT_CMD_ERR_RSP;
    DSAT_SMS_STATE_CLEAN_ALL();
    send_mr = 0;
  }

  /* send the response to the user */
  dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
  
  return result;
}/* submit_msg_report_event_handler */


/*===========================================================================

FUNCTION  STATUS_REPORT_READ_EVENT_HANDLER

DESCRIPTION
  Handler function for STATUS-REPORT read message event from WMS

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
LOCAL dsat_result_enum_type status_report_read_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsm_item_type * formatted_res_buff = NULL;
  dsm_item_type * res_buff = NULL;
  unsigned int used_val;
  dsat_result_enum_type result;
  wms_status_e_type status;
  dsat_rsp_enum_type resp_state;

  wms_client_ts_data_s_type *temp_ts_data = NULL;

  DS_AT_MSG0_HIGH("ATCOP SMS: STATUS REPORT Read Message Event Handler");

  if ( (dsatsmsi_pres_cmd.cmd_name != SMS_CMGR) && 
       (dsatsmsi_pres_cmd.cmd_name != SMS_CNMA) &&
       (dsatsmsi_pres_cmd.cmd_name != SMS_CMGL))
  {
    /* unwanted event */
    return DSAT_ASYNC_EVENT;
  }
  
  /* Check whether the received WMS format is unsupported and if yes handle gracefully */
  if ( event_info->u.msg_status.u.gw_message.raw_ts_data.format != WMS_FORMAT_GW_PP )
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
            res_buff = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
            /* Add trailer at the end of response */
            res_buff->used = 0;
            dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_END_OF_RSP);
          }
        }
        else
        {
          /* We have more stat to read */
          (void)dsatsmsi_get_msg_list(event_info->subs_id);
          return DSAT_ASYNC_CMD;
        }
      }
      else
      {
        /* We are not done with reading the list */
        (void)dsatsmsi_read_msg_list(event_info->subs_id);
        return DSAT_ASYNC_CMD;
      }
    }
    else if ( dsatsmsi_pres_cmd.cmd_name == SMS_CNMA )
    {
      return DSAT_ASYNC_EVENT;
    }

    /* We are done with the commands, clean up */
    DSAT_SMS_STATE_CLEAN_ALL();
    return result;
  }

  /* get the DSM item */
  res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
 
  temp_ts_data = (wms_client_ts_data_s_type *) 
                      dsat_alloc_memory (sizeof(wms_client_ts_data_s_type), FALSE);

  /* decode the message */
  status = wms_ts_decode
  ( &event_info->u.msg_status.u.gw_message.raw_ts_data, temp_ts_data );

  if( status != WMS_OK_S )
  {
    /* We got an error while reading the message */
    /* Tell TE about it                          */
    DS_AT_MSG0_ERROR("ATCOP SMS: Error in STATUS REPORT decoding");

    used_val = dsatsmsi_cms_error(res_buff, (int)DSAT_CMS_UNKNOWN_ERR);
    res_buff->used = (byte)used_val;
    result = DSAT_CMD_ERR_RSP;
    /* send the response to the user */
    if ( dsatsmsi_cmd_sm.list_begin )
    {
      dsatsmsi_cmd_sm.list_begin = FALSE;
    }
    dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);

    /* We are done with the commands, clean up */
    DSAT_SMS_STATE_CLEAN_ALL();
    dsatutil_free_memory((void *)temp_ts_data);
    return result;
  }

  if((!dsatsmsi_cmd_sm.list_begin) && (dsatsmsi_pres_cmd.cmd_name ==SMS_CMGL))
  {
    /* This is not the first message             */
    /* Seperate the next message with a <CR><LF> */
    *res_buff->data_ptr= '\n';
    res_buff->used++;
  }
  /*check to see if it is a TEXT message or a PDU message */
  if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE))
  {
    (void)dsatetsismsi_read_txt_msg
        ( temp_ts_data, res_buff, &event_info->u.msg_status );
  }
  else
  {
    (void)dsatetsismsi_read_pdu_msg( &event_info->u.msg_status, 
                                     res_buff, 
                                     temp_ts_data );
  }

  if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL)
  {
    /* We got a CMGL command */
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
        if ( TRUE == dsatsmsi_get_msg_list(event_info->subs_id) )
        {
          result = DSAT_ASYNC_CMD;
        }
        else
        {
          DS_AT_MSG0_ERROR(" Could not get the msg list ");
          dsatutil_free_memory((void *)temp_ts_data);
          return DSAT_ERROR;
        }
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
      if ( TRUE == dsatsmsi_read_msg_list(event_info->subs_id) )
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        DS_AT_MSG0_ERROR(" Could not read the msg list ");
        dsatutil_free_memory((void *)temp_ts_data);
        return DSAT_ERROR;
      }
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
    dsatcmdp_handle_async_cmd_rsp(res_buff, resp_state);
    dsatutil_free_memory((void *)temp_ts_data);
    return result;
  }
  else if ( dsatsmsi_pres_cmd.cmd_name == SMS_CNMA )
  {
    /* We are processing an incoming STATUS-REPORT */
    DS_AT_MSG0_HIGH("ATCOP SMS: Handling incoming STATUS REPORT");

    /* We are done with filling up raw response buffer */
    /* Format it and send it to TE                     */
    formatted_res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
    /* send the response to the user */
    dsat_fmt_response( &res_buff, DSAT_COMPLETE_RSP, FALSE, 
                       &formatted_res_buff );
  
    if (NULL != formatted_res_buff)
    {
      /* Flush the queue to the Rm interface. */
      dsatcmdp_send_queued_cmd_results( );

      /* Check the formatted response data item. If it is not empty, send it
         to the Rm interface. */
      if ( formatted_res_buff->used != 0 )
      {
        /* If the accessory feature is enabled, inform SIOLIB which port 
           this URC should go to */
        ds3g_siolib_set_tx_type 
          ( DS3G_SIOLIB_UNSOLICTED_SMS, DS3G_SIOLIB_DATA_PORT ); 

        dsatrsp_send_response( formatted_res_buff, TRUE );
      }
      else
      {
        (void) dsm_free_buffer( formatted_res_buff );
      }
    }

    /* ACK need to be sent through CNMA only when CSMS=1 */
    if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CSMS_IDX,
                                         event_info->subs_id,0,NUM_TYPE) == 0 )
    {
      waiting_for_an_ack = FALSE;
      DSAT_SMS_STATE_CLEAN();
    }
    dsatutil_free_memory((void *)temp_ts_data);
    return DSAT_ASYNC_EVENT;
  }
  else
  {
    /* We are processing a STATUS-REPORT read (AT+CMGR) */
    DS_AT_MSG0_HIGH("ATCOP SMS: STATUS REPORT Read");

    dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);

    /* We are done with the commands, clean up */
    DSAT_SMS_STATE_CLEAN_ALL();

    dsatutil_free_memory((void *)temp_ts_data);
    return DSAT_OK;
  }
   
} /* status_report_read_event_handler */


#endif /* FEATURE_ETSI_SMS */
