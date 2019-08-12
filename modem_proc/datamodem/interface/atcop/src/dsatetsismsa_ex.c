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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsismsa_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why

--------   ---     ----------------------------------------------------------
08/17/16   skc     Fixed made to delete SM from GW RAM for no message class SM.
04/15/15   sc      Fixed issue in event handling to use subs_id provided in 
                   event info.
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

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
#include "dsatdl.h"
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

/* SMS Mobile Terminated message queue */
extern q_type sms_mt_msg_q;         

/* Queue for storing free message buffers */ 
extern q_type sms_mt_msg_free_q;


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
boolean waiting_for_an_ack = FALSE;
LOCAL boolean msg_sent = FALSE;

/* LOCAL DECLARATIONS: */
LOCAL wms_transaction_id_type    init_tid = 0;
wms_transaction_id_type    act_tid = 0;

LOCAL wms_message_index_type rcvd_msg_index;

LOCAL sms_mt_msg_e_type last_mt_msg;

/* PROTOTYPES FOR LOCAL FUNCTIONS: */

LOCAL dsat_result_enum_type handle_mt_notification
(
 sms_mt_msg_e_type       msg_type,   /* Type of message       */
 wms_message_index_type  msg_index,  /* Index Number          */
 wms_memory_store_e_type mem_store,   /* Type of Memory Store  */
 sys_modem_as_id_e_type  subs_id      /* SUBS ID index*/
);

LOCAL dsat_result_enum_type write_message_to_ram 
(
  sms_mt_msg_e_type              msg_type, /* Type of message   */
  const sms_mt_msg_info_s_type * deliver, /* SMS-DELIVER */
  sys_modem_as_id_e_type         subs_id   /* SUBS ID index*/
);

LOCAL void add_mt_msg_to_queue 
(
 sms_mt_msg_e_type       msg_type,     /* Type of message      */
 wms_message_index_type  msg_index,    /* Index Number         */
 wms_memory_store_e_type mem_store,    /* Type of Memory Store */
 wms_transaction_id_type tid           /* Transaction ID       */
);

LOCAL dsat_result_enum_type read_msg_event_handler_ex
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

dsat_result_enum_type received_msg_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

dsat_result_enum_type status_msg_report_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

LOCAL dsat_result_enum_type mt_message_error_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

LOCAL dsat_result_enum_type status_report_read_event_handler_ex
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

dsat_result_enum_type status_report_write_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode */
  sms_event_info_s_type  *event_info   /* Event info ptr */
);

void modify_tag
(
 const sms_msg_status_info_s_type * status_msg_ptr,  /* Ptr to msg from WMS     */
 sys_modem_as_id_e_type             subs_id
);

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
/* the Message Event Handler lookup table */
/* note that the order of the functions has to match wms_msg_event_e_type */
const msg_handler_tab_type dsatetsisms_msg_handler_table_entries_ex[] =
{
  { SMS_CFG_EVENT_GW_READY,         dsatsmsi_gw_ready_cfg_event_handler         },
  { SMS_CFG_EVENT_MEMORY_STATUS,    dsatsmsi_memory_status_cfg_event_handler_ex },
  { SMS_CFG_EVENT_LINK_CONTROL,     dsatsmsi_link_ctrl_event_handler            },
#ifdef FEATURE_ETSI_SMS_PS
  { SMS_CFG_EVENT_GW_DOMAIN_PREF,   dsatsmsi_gw_domain_pref_event_handler       },
#endif /* FEATURE_ETSI_SMS_PS */

  { SMS_MSG_EVENT_READ,               read_msg_event_handler_ex                 },
  { SMS_MSG_EVENT_WRITE,              write_msg_event_handler_ex                },
  { SMS_MSG_EVENT_RECEIVED_MESSAGE,   received_msg_event_handler                },
  { SMS_MSG_EVENT_STATUS_REPORT,      status_msg_report_event_handler           },
  { SMS_MSG_EVENT_MT_MESSAGE_ERROR,   mt_message_error_event_handler            },
  { SMS_MSG_EVENT_READ_STS_REPORT,    status_report_read_event_handler_ex       },
  { SMS_MSG_EVENT_WRITE_STS_REPORT,   status_report_write_event_handler         },
};

const dsatsms_msg_handler_tab_type dsatetsisms_msg_handler_table_ex =
{
  dsatetsisms_msg_handler_table_entries_ex,
  ARR_SIZE(dsatetsisms_msg_handler_table_entries_ex)
};
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */


/*===========================================================================

FUNCTION DSATETSISMS_FLUSH_SMS_INDICATIONS

DESCRIPTION
  Flushes buffer of SMS incoming message indications and sends indications
  to TE. Indications are buffered and sent based on AT parameter setting

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void dsatetsisms_flush_sms_indications
(
  sys_modem_as_id_e_type   subs_id /* Subscription Id */
)
{
  sms_mt_msg_type  *msg_ptr;
  
  dsat_num_item_type cnmi_mt_val    = 0;
  dsat_num_item_type cnmi_ds_val    = 0;
  dsat_num_item_type cnmi_bfr_val   = 0;
  dsat_num_item_type cnmi_mode_val  = 0;

  cnmi_mt_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CNMI_ETSI_IDX,subs_id,CNMI_MT_VAL,NUM_TYPE);
  cnmi_ds_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CNMI_ETSI_IDX,subs_id,CNMI_DS_VAL,NUM_TYPE);
  cnmi_bfr_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CNMI_ETSI_IDX,subs_id,CNMI_BFR_VAL,NUM_TYPE);
  cnmi_mode_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CNMI_ETSI_IDX,subs_id,CNMI_MODE_VAL,NUM_TYPE);
  
  /* Check to see the user preferences        */
  /* Flush the indications only if <bfr> is 0 */

  /* The formatting of the response is done based on the current */
  /* functionality that is supported: <mt>: 0, 1 and <ds>: 0, 2  */
  /* We need to check for <mt> and <ds> for formatting the resp  */

  if ( ( waiting_for_an_ack ) &&
                ( ( cnmi_mt_val == 2) || ( cnmi_mt_val == 3 ) ) )
  {
    /* we are still waiting on earlier response     */
    /* Should not send another message, return back */
    DS_AT_MSG0_HIGH("Waiting for an ack. Will send message to TE later");
    return;
  }

  if( (cnmi_bfr_val == 0) && (cnmi_mode_val != 0) )
  {
    /* Flush the queued indications to TE */

    /* Get messages from the queue until the queue is empty */
    while( (msg_ptr = (sms_mt_msg_type *)q_get( &sms_mt_msg_q )) != NULL )
    {
      /* Check to see if user selected the option */
      /* to route DELIVER or STATUS-REPORT        */
      if( ((msg_ptr->msg_type == MT_DELIVER) && (cnmi_mt_val == 0))
          ||
          ((msg_ptr->msg_type == MT_STATUS_REPORT) && (cnmi_ds_val == 0))
        )
      {
        /* We are in do not send the msg to TE mode    */
        /* Return the message buffer to the free queue */
        DS_AT_MSG0_HIGH("Buffer mode, add it to local queue");
        q_put( &sms_mt_msg_free_q, &msg_ptr->link );

        continue;
      }
      else if ( ( msg_ptr->mem_store == WMS_MEMORY_STORE_RAM_GW ) &&
              ( ( cnmi_mt_val == 2) || ( cnmi_mt_val == 3) || ( cnmi_ds_val == 1) ) 
              )
      {
        wms_status_e_type msg_status;
        /* we got a direct routing message */
        rcvd_msg_index = msg_ptr->index;

        DS_AT_MSG1_HIGH("ATCOP : Reading MT SMS from RAM, index: %d", 
                  msg_ptr->index);

        if ( msg_ptr->msg_type == MT_DELIVER )
        {
          last_mt_msg = MT_DELIVER;

#ifdef FEATURE_DUAL_SIM
          msg_status = wms_msg_ms_read
                                   (
                                  dsatsmsi_client_id,
                                  subs_id,
                                  dsatsmsi_cmd_cb_func,
                                  (void*)&dsatsmsi_pres_cmd,
                                  WMS_MEMORY_STORE_RAM_GW,
                                  msg_ptr->index
                                  );
#else
         msg_status = wms_msg_read
                                  (
                                 dsatsmsi_client_id,
                                 dsatsmsi_cmd_cb_func,
                                 (void*)&dsatsmsi_pres_cmd,
                                 WMS_MEMORY_STORE_RAM_GW,
                                 msg_ptr->index
                                 );
#endif /* FEATURE_DUAL_SIM */
        }
        else
        {
          last_mt_msg = MT_STATUS_REPORT;
#ifdef FEATURE_DUAL_SIM
           msg_status = wms_msg_ms_read_sts_report
                        (
                          dsatsmsi_client_id,
                          subs_id,
                          dsatsmsi_cmd_cb_func,
                          (void*)&dsatsmsi_pres_cmd,
                          WMS_MEMORY_STORE_RAM_GW,
                          msg_ptr->index
                        );
#else
           msg_status = wms_msg_read_sts_report
                        (
                          dsatsmsi_client_id,
                          dsatsmsi_cmd_cb_func,
                          (void*)&dsatsmsi_pres_cmd,
                          WMS_MEMORY_STORE_RAM_GW,
                          msg_ptr->index
                        );
#endif /* FEATURE_DUAL_SIM */
        }

        if ( msg_status != WMS_OK_S )
        {
          DS_AT_MSG0_HIGH("ATCOP SMS INTERNAL READ ERROR");
        }
        act_tid = msg_ptr->tid;
        msg_sent = TRUE;
        /* Return the message buffer to the free queue */
        q_put( &sms_mt_msg_free_q, &msg_ptr->link );
        return;
      }
      else
      {
        /* Send the indication to TE */
        DS_AT_MSG0_HIGH("Sending the indication to TE");
        dsatsmsi_send_async_indication_to_te (
                                      msg_ptr->msg_type,
                                      msg_ptr->index,
                                      msg_ptr->mem_store,
                                      ETSI_CMD_MODE,
                                      subs_id
                                    );

        /* Return the message buffer to the free queue */
        q_put( &sms_mt_msg_free_q, &msg_ptr->link );
      }
    }/* while */
  }
  else if( (cnmi_bfr_val == 1) && (cnmi_mode_val != 0) )
  {
    /* Clear the queued indications */
    while( (msg_ptr = (sms_mt_msg_type *)q_get( &sms_mt_msg_q )) != NULL )
    {
      /* Return the message buffer to the free queue */
      q_put( &sms_mt_msg_free_q, &msg_ptr->link );
    }
  }
  else
  {
    /* Do nothing. Just return */
    return;
  }

  return;
}/* dsatetsisms_flush_sms_indications */

/*===========================================================================

FUNCTION    DSATETSISMSI_FREE_ELEMENT

DESCRIPTION
  Checkes to see if any message items have to be freed. Deletes the message
  from temporary memory.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsismsi_free_element 
(
 boolean                 ack_required, /* Flag to tell if ACK is required */
 sys_modem_as_id_e_type  subs_id
)
{
  wms_status_e_type msg_status;

  /* ACK required variable is mainly used when message waiting indication */
  /* is DISCARD, for everything else we default to TRUE                   */
  /* received an ACK for the message. Now delete it from RAM */

  if ( last_mt_msg == MT_DELIVER )
  {
#ifdef FEATURE_DUAL_SIM
    msg_status = wms_msg_ms_delete(
                              dsatsmsi_client_id,
                              subs_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                              WMS_MEMORY_STORE_RAM_GW,
                              rcvd_msg_index
                             );
#else
    msg_status = wms_msg_delete(
                              dsatsmsi_client_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                              WMS_MEMORY_STORE_RAM_GW,
                              rcvd_msg_index
                             );
#endif /* FEATURE_DUAL_SIM */
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
    msg_status = wms_msg_ms_delete_sts_report
                                          (
                                            dsatsmsi_client_id,
                                            subs_id,
                                            dsatsmsi_cmd_cb_func,
                                            (void*)&dsatsmsi_pres_cmd,
                                            WMS_MEMORY_STORE_RAM_GW,
                                            rcvd_msg_index
                                          );
#else
    msg_status = wms_msg_delete_sts_report
                                          (
                                            dsatsmsi_client_id,
                                            dsatsmsi_cmd_cb_func,
                                            (void*)&dsatsmsi_pres_cmd,
                                            WMS_MEMORY_STORE_RAM_GW,
                                            rcvd_msg_index
                                          );
#endif /* FEATURE_DUAL_SIM */
  }

  if ( msg_status != WMS_OK_S )
  {
    DS_AT_MSG0_HIGH("ATCOP SMS INTERNAL READ ERROR");
  }
  if(dsatcmdp_at_state == DSAT_CMD)
  {
    /* we ACK the msg, check to see if we need to send the next message */
    dsatetsisms_flush_sms_indications(subs_id);
  }

  if ( msg_sent == TRUE )
  {
    /* Another message was was sent */
    waiting_for_an_ack = ack_required;
    msg_sent = FALSE;
  }
  else
  {
    waiting_for_an_ack = FALSE;
  }

  rcvd_msg_index = 0;

  return;
}/* dsatetsismsi_free_element */

/**********************LOCAL UTILITY FUNCTIONS******************************/
/*===========================================================================

FUNCTION    ADD_MT_MSG_TO_QUEUE

DESCRIPTION
  Adds unsolicited response result code to a queue.
  If the queue is full then the first element is deleted and the new
  element is replaces it.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void add_mt_msg_to_queue 
(
 sms_mt_msg_e_type       msg_type,   /* Type of message      */
 wms_message_index_type  msg_index,  /* Index Number         */
 wms_memory_store_e_type mem_store,  /* Type of Memory Store */
 wms_transaction_id_type tid         /* Transaction ID       */
)
{
  queue_mt_msg(msg_type,msg_index,mem_store,tid,ETSI_CMD_MODE);

  return;
} /* add_mt_msg_to_queue */

/*===========================================================================

FUNCTION MODIFY_TAG

DESCRIPTION
  Local function that is used to modify the tag of a message.
  Checks to see if the present tag is "rec unread" if so it changes 
  it to "rec read". Note that we are responsible for changing the tag on 
  mobile terminated messages. WMS takes care of tags for mobile originated
  messages

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/
void modify_tag
(
 const sms_msg_status_info_s_type * status_msg_ptr,  /* Ptr to msg from WMS */
 sys_modem_as_id_e_type             subs_id
)
{
  sms_mt_msg_type  *msg_ptr = NULL;

  if ( status_msg_ptr->msg_hdr.tag == WMS_TAG_MT_NOT_READ )
  {
    /* We got a REC UNREAD tag */
    /* Modify it to REC READ   */
#ifdef FEATURE_DUAL_SIM
    (void) wms_msg_ms_modify_tag(
                              dsatsmsi_client_id,
                              subs_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                              status_msg_ptr->msg_hdr.mem_store,
                              status_msg_ptr->msg_hdr.index,
                              WMS_TAG_MT_READ
                             );
#else
    (void) wms_msg_modify_tag(
                              dsatsmsi_client_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                              status_msg_ptr->msg_hdr.mem_store,
                              status_msg_ptr->msg_hdr.index,
                              WMS_TAG_MT_READ
                             );
#endif /* FEATURE_DUAL_SIM */
    /* We got a REC UNREAD */
    /* Delete the message if it is in our local queue */
    msg_ptr = q_check( &sms_mt_msg_q );
    while( msg_ptr != NULL )
    {
      if( msg_ptr->index == status_msg_ptr->msg_hdr.index )
      {
        q_delete( &sms_mt_msg_q, &msg_ptr->link );
        q_put( &sms_mt_msg_free_q, &msg_ptr->link );
        return;
      }
      else
      {
        msg_ptr = q_next( &sms_mt_msg_q, &msg_ptr->link );
      }
    }
    return;
  }
  else
  {
    /* no need for any change */
    return;
  }
  
} /* modify_tag */

/*===========================================================================

FUNCTION    HANDLE_MT_NOTIFICATION

DESCRIPTION
  Handles both SMS-DELIVERs and SMS-STATUS-REPORTs in STORE and NOTIFY mode
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_ASYNC_EVENT : If no result code is needed (e.g. unwanted event)

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type handle_mt_notification
(
 sms_mt_msg_e_type       msg_type,   /* Type of message       */
 wms_message_index_type  msg_index,  /* Index Number          */
 wms_memory_store_e_type mem_store,   /* Type of Memory Store  */
 sys_modem_as_id_e_type  subs_id
)
{
  dsat_num_item_type val_check;
  dsat_num_item_type cnmi_mt_val    = 0;
  dsat_num_item_type cnmi_ds_val    = 0;
  dsat_num_item_type cnmi_mode_val  = 0;

  cnmi_mt_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CNMI_ETSI_IDX,subs_id,CNMI_MT_VAL,NUM_TYPE);
  cnmi_ds_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CNMI_ETSI_IDX,subs_id,CNMI_DS_VAL,NUM_TYPE);
  cnmi_mode_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CNMI_ETSI_IDX,subs_id,CNMI_MODE_VAL,NUM_TYPE);
  if ( msg_type == MT_DELIVER )
  {
    val_check = cnmi_mt_val;
  }
  else
  {
    val_check = cnmi_ds_val;
  }

  /* Check to see if ATCOP is busy with other commands */
  /* If not send the notification directly to TE       */
  if ( !dsatcmdp_block_indications_ex() )
  {
    /* Check to see what type notification routing is set */
    if (cnmi_mode_val == 0 )
    {
      /* we are in mode 0: Keep buffering them */
      add_mt_msg_to_queue ( msg_type, msg_index, mem_store, 0 );

      return DSAT_ASYNC_EVENT;
    }
    else if( val_check == 0 )
    {
      /* User selected the option not to route */
      return DSAT_ASYNC_EVENT;
    }
    else
    {
      /* Check to see if we have any outstanding notifications */
      /* If yes, send them to TE so that they are in order     */
      dsatetsisms_flush_sms_indications(subs_id );

      /* Send the present indications directly to TE */
      dsatsmsi_send_async_indication_to_te ( msg_type, 
                                             msg_index, 
                                             mem_store,
                                             ETSI_CMD_MODE,
                                             subs_id);

      return DSAT_ASYNC_EVENT;
    }
  }
  else
  {
    /* ATCOP is Busy */
    if ( cnmi_mode_val == 1 )
    {
      /* User selected the option to ignore notifications */ 
      return DSAT_ASYNC_EVENT;
    }
    else
    {
      /* buffer the indication based on user preference */
      /* it will be flushed to TE at a later stage      */
      add_mt_msg_to_queue ( msg_type, msg_index, mem_store, 0 );

      /* Tell ATCOP not to send any result code */
      return DSAT_ASYNC_EVENT;
    }
  }
} /* handle_mt_notification */

/*===========================================================================

FUNCTION    WRITE_MESSAGE_TO_RAM

DESCRIPTION
  Writes the notification to RAM temporarily. This is used in the direct
  notification case. Handles SMS-DELIVERs.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_ASYNC_EVENT : If no result code is needed (e.g. unwanted event)

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type write_message_to_ram 
(
  sms_mt_msg_e_type                  msg_type,       /* Type of message   */
  const sms_mt_msg_info_s_type      *deliver,        /* SMS-DELIVER       */
  sys_modem_as_id_e_type             subs_id
)
{
  wms_status_e_type msg_status;
  wms_write_mode_e_type write_mode; 

  /* Data structure used primarily to replace the <da> or write to RAM */
  wms_client_message_s_type *temp_msg_ptr = NULL;
  
  /* Check to see if ATCOP is busy with other commands */
  /* If not send the notification directly to TE       */
  if ( !dsatcmdp_block_indications_ex() )
  {
    dsatsmsi_pres_cmd.cmd_name = SMS_CNMA;
    /* 
       Always use INSERT which will lead to memory allocation if not done previously.
    */
    write_mode = WMS_WRITE_MODE_INSERT;

    /* now fill the message to write to RAM */
    temp_msg_ptr = (wms_client_message_s_type *) 
                        dsat_alloc_memory (sizeof(wms_client_message_s_type), FALSE);

    temp_msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_RAM_GW;
    temp_msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
    temp_msg_ptr->u.gw_message = deliver->u.gw_message;
    temp_msg_ptr->u.gw_message.raw_ts_data.format = WMS_FORMAT_GW_PP;

    if ( msg_type == MT_DELIVER )
    {
      DS_AT_MSG0_HIGH("Writing DELIVER to RAM (Temp)");

      temp_msg_ptr->msg_hdr.tag = WMS_TAG_MT_NOT_READ;

#ifdef FEATURE_DUAL_SIM
      msg_status = wms_msg_ms_write(
                                 dsatsmsi_client_id,
                                 subs_id,
                                 dsatsmsi_cmd_cb_func,
                                 (void*)&dsatsmsi_pres_cmd,
                                 write_mode,
                                 temp_msg_ptr
                                );
#else
      msg_status = wms_msg_write(
                                 dsatsmsi_client_id,
                                 dsatsmsi_cmd_cb_func,
                                 (void*)&dsatsmsi_pres_cmd,
                                 write_mode,
                                 temp_msg_ptr
                                );
#endif /* FEATURE_DUAL_SIM */
    }
    else
    {
      DS_AT_MSG0_HIGH("Writing STATUS REPORT to RAM (Temp)");

      temp_msg_ptr->msg_hdr.tag = WMS_TAG_STATUS_RPT;
#ifdef FEATURE_DUAL_SIM
      msg_status = wms_msg_ms_write_sts_report
                           (
                             dsatsmsi_client_id,
                             subs_id,
                             dsatsmsi_cmd_cb_func,
                             (void*)&dsatsmsi_pres_cmd,
                             write_mode,
                             temp_msg_ptr
                           );
#else
      msg_status = wms_msg_write_sts_report
                                          (
                                            dsatsmsi_client_id,
                                            dsatsmsi_cmd_cb_func,
                                            (void*)&dsatsmsi_pres_cmd,
                                            write_mode,
                                            temp_msg_ptr
                                          );
#endif /* FEATURE_DUAL_SIM */
    }
    if ( msg_status != WMS_OK_S )
    {
      DS_AT_MSG1_ERROR("Failed to write message, status: %d", msg_status);
    }
  }
  
  init_tid = deliver->transaction_id;

  dsatutil_free_memory((void *)temp_msg_ptr);
  return DSAT_ASYNC_EVENT;

} /* write_message_to_ram */

/*===========================================================================

FUNCTION  READ_MSG_EVENT_HANDLER_EX

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
LOCAL dsat_result_enum_type read_msg_event_handler_ex
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  wms_status_e_type status;
  dsat_err_e_type   err_info = DSAT_ERR_NO_ERROR;

  wms_client_ts_data_s_type *temp_ts_data = NULL;

  if ( dsatsmsi_pres_cmd.cmd_name != SMS_CNMA )
  {
    /* unwanted event */
    return DSAT_ASYNC_EVENT;
  }

  DS_AT_MSG0_HIGH("ATCOP SMS: Read Message Event Handler ex");

  /* Check whether the received WMS format is unsupported and if yes handle gracefully */
  if ( event_info->u.msg_status.u.gw_message.raw_ts_data.format != WMS_FORMAT_GW_PP )
  {
    /*Unsupported WMS format*/
    err_info = DSAT_ERR_INVALID_SYNTAX;
    goto send_result;
  }

  temp_ts_data = (wms_client_ts_data_s_type *) 
                      dsat_alloc_memory (sizeof(wms_client_ts_data_s_type), FALSE);

  /* decode the message */
  status = wms_ts_decode
  ( &event_info->u.msg_status.u.gw_message.raw_ts_data, temp_ts_data );

  if( status != WMS_OK_S )
  {
    /* We are done with the commands, clean up */
    goto send_result;
  }

  /* Check to see if we need to modify the tag of the msg */
  modify_tag ( &event_info->u.msg_status,event_info->subs_id );

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
  }
  dsatutil_free_memory((void *)temp_ts_data);
  return DSAT_ASYNC_EVENT;

send_result:

  /* We are done with the commands, clean up */
  DS_AT_MSG1_ERROR("read_msg_event_handler_ex() Error_Info: %d" , err_info);
  DSAT_SMS_STATE_CLEAN();

  dsatutil_free_memory((void *)temp_ts_data);
  return DSAT_ASYNC_EVENT;
} /* read_msg_event_handler_ex */

/*===========================================================================

FUNCTION  WRITE_MSG_EVENT_HANDLER_EX

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
LOCAL dsat_result_enum_type write_msg_event_handler_ex
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  uint32 return_val;
  dsat_result_enum_type result = DSAT_OK;
  sms_cmd_name_e_type user_data;

  if ( dsatsmsi_pres_cmd.cmd_name != SMS_CMGW &&
       dsatsmsi_pres_cmd.cmd_name != SMS_CNMA )
  {
    /* unwanted event */
    return DSAT_ASYNC_EVENT;
  }

  user_data = *((sms_cmd_name_e_type*)event_info->u.msg_status.user_data);

  DS_AT_MSG0_HIGH("ATCOP SMS: Write Message Event Handler");

  /* We need to send index at which the message was stored */
  return_val = event_info->u.msg_status.msg_hdr.index;

  if ( dsatsmsi_pres_cmd.cmd_name == SMS_CNMA )
  {
    sms_mt_msg_e_type msg_type = MT_STATUS_REPORT;

    DS_AT_MSG0_HIGH("ATCOP: Wrote MT msg in RAM");

    if ( user_data != SMS_CNMA )
    {
      return DSAT_ASYNC_EVENT;
    }

    msg_type = MT_DELIVER;

    /* Wrote the directly routed message into RAM. Now check to see if we can */
    /* send the message to TE                                                 */
    if ( waiting_for_an_ack )
    {
      /* Already waiting for an ACK, add it to the queue. Will send later */
      DS_AT_MSG0_HIGH("ATCOP SMS: TE busy. Saving the DELIVER");
      add_mt_msg_to_queue(msg_type,return_val,WMS_MEMORY_STORE_RAM_GW,init_tid);
    }
    else
    {
      /* We are not waiting for an ACK. Add the message to queue, send to TE */
      DS_AT_MSG0_HIGH("ATCOP SMS: Reading the DELIVER from temp memory");
      add_mt_msg_to_queue(msg_type,return_val,WMS_MEMORY_STORE_RAM_GW,init_tid);
      if(dsatcmdp_at_state == DSAT_CMD)
      {
        dsatetsisms_flush_sms_indications(event_info->subs_id);
      }
      if ( msg_sent == TRUE )
      {
        /* when CSMS=1, for class 1 messages +CNMA should be sent
           in case of class 0 messages only in THIN_UI +CNMA must be sent
           in FULL UI wms would ack */
        if ( (dsat_num_item_type)dsatutil_get_val(
                 DSATETSI_EXT_CSMS_IDX,event_info->subs_id,0,NUM_TYPE) == 1)
        {
          waiting_for_an_ack = TRUE;
        }
        msg_sent = FALSE;
      }
    }
    result = DSAT_ASYNC_EVENT;
  }
  else
  {
    DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.write_msg_event_handler_fp);
    result = dsatdl_vtable.write_msg_event_handler_fp(mode, event_info);
  }

  return result;
} /* write_msg_event_handler_ex */

/*===========================================================================

FUNCTION  RECEIVED_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for received message (incoming) event from WMS

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
dsat_result_enum_type received_msg_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  wms_status_e_type msg_status;
  wms_message_index_type  msg_index;
  wms_memory_store_e_type mem_store;
  dsat_result_enum_type result = DSAT_ASYNC_EVENT; 
  dsat_num_item_type cnmi_mt_val    = 0;
  dsat_num_item_type cnmi_bm_val    = 0;
  dsat_num_item_type cnmi_mode_val  = 0;
  dsat_err_e_type    err_info = DSAT_ERR_NO_ERROR;

  wms_client_ts_data_s_type *temp_ts_data = NULL;
  

  DS_AT_MSG0_HIGH("ATCOP SMS: Received Message Event Handler");

  if ( ( event_info->u.mt_msg.u.gw_message.raw_ts_data.format != WMS_FORMAT_GW_PP ) &&
       ( event_info->u.mt_msg.u.gw_message.raw_ts_data.format != WMS_FORMAT_GW_CB ) )
  {
    /*Unsupported WMS format*/
    err_info = DSAT_ERR_INVALID_FORMAT;
    goto send_result;
  }

  mem_store = event_info->u.mt_msg.msg_hdr.mem_store;
  msg_index = event_info->u.mt_msg.msg_hdr.index;

  temp_ts_data = (wms_client_ts_data_s_type *) 
                      dsat_alloc_memory (sizeof(wms_client_ts_data_s_type), FALSE);

  msg_status = wms_ts_decode (&event_info->u.mt_msg.u.gw_message.raw_ts_data,
                              temp_ts_data );

  if ( msg_status != WMS_OK_S )
  {
    /*ERROR in decoding received message*/
    err_info = DSAT_ERR_INVALID_OPERATION;
    goto send_result;
  }
  cnmi_mt_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CNMI_ETSI_IDX,event_info->subs_id,CNMI_MT_VAL,NUM_TYPE);
  cnmi_bm_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CNMI_ETSI_IDX,event_info->subs_id,CNMI_BM_VAL,NUM_TYPE);
  cnmi_mode_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CNMI_ETSI_IDX,event_info->subs_id,CNMI_MODE_VAL,NUM_TYPE);
#ifdef FEATURE_ETSI_SMS_CB 
  /* Check to see what type notification routing is set */
  if ( temp_ts_data->format == WMS_FORMAT_GW_CB )
  {
    /* we received a Cell broadcast message */
    if ( cnmi_bm_val == 0 )
    {
      /* User selected not to route to TE */
      err_info = DSAT_ERR_INVALID_OPERATION;
    }
    else if ( !dsatcmdp_block_indications_ex() )
    {
      /* TE is not busy, send the message */
      if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.send_cb_msg_to_te_fp))
      {
        dsatdl_vtable.send_cb_msg_to_te_fp(
          &event_info->u.mt_msg.u.gw_message.raw_ts_data,
          temp_ts_data,
          event_info->subs_id);
      }
    }
    /*Return dsat_async. Also return async if format is WMS_FORMAT_GW_CB*/
    goto send_result;
  }
#endif /* FEATURE_ETSI_SMS_CB */
  if ( ( cnmi_mt_val == 0 ) || ( cnmi_mt_val == 1 ) ||
       ( temp_ts_data->u.gw_pp.u.deliver.dcs.msg_class == WMS_MESSAGE_CLASS_2 ) ||
       ( temp_ts_data->u.gw_pp.u.deliver.dcs.msg_waiting == 
         WMS_GW_MSG_WAITING_STORE ) ||
       ( ( cnmi_mt_val == 3 ) && 
          (temp_ts_data->u.gw_pp.u.deliver.dcs.msg_class != WMS_MESSAGE_CLASS_3 ) )
     )
  {
    /* we are in store and route mode */
    result = handle_mt_notification ( MT_DELIVER, msg_index, mem_store ,
                                     event_info->subs_id);
  }
  else if ( cnmi_mode_val == 1 )
  {
    /* We are in direct notification mode*/
    result = write_message_to_ram ( MT_DELIVER, &event_info->u.mt_msg,
                        event_info->subs_id );
  }
  else
  {
    /* please lint */
    result = DSAT_ASYNC_EVENT;
  }
send_result:
  DS_AT_MSG1_HIGH("received_msg_event_handler() err_info: %d" , err_info);
  dsatutil_free_memory((void *)temp_ts_data);
  return result;
} /* received_msg_event_handler */

/*===========================================================================

FUNCTION  STATUS_MSG_REPORT_EVENT_HANDLER

DESCRIPTION
  Handler function for status message report event from WMS

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
dsat_result_enum_type status_msg_report_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  /* Routing Algorithm :
     Direct Routing - Save the message in RAM first and send it to TE.
                    - Send one message at a time and only if TE is not busy.
                    - Message is sent to TE after write is ACked by WMS.
     Store and Route - Send one message at a time and only if TE is not busy.
   */
  wms_status_e_type msg_status;
  dsat_result_enum_type result;
  wms_message_index_type  msg_index;
  wms_memory_store_e_type mem_store;
  dsat_num_item_type cnmi_ds_val    = 0;
  wms_client_ts_data_s_type *temp_ts_data = NULL;

  DS_AT_MSG0_HIGH("ATCOP SMS: Status Message Report Event Handler");

  if ( event_info->u.mt_msg.u.gw_message.raw_ts_data.format != WMS_FORMAT_GW_PP )
  {
    DS_AT_MSG0_HIGH("Unsupported WMS format");
    return DSAT_ASYNC_EVENT;
  }
  cnmi_ds_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CNMI_ETSI_IDX,event_info->subs_id,CNMI_DS_VAL,NUM_TYPE);

  mem_store = event_info->u.mt_msg.msg_hdr.mem_store;

  msg_index = event_info->u.mt_msg.msg_hdr.index;

  temp_ts_data = (wms_client_ts_data_s_type *) 
                      dsat_alloc_memory (sizeof(wms_client_ts_data_s_type), FALSE);
  msg_status = wms_ts_decode 
    ( &event_info->u.mt_msg.u.gw_message.raw_ts_data, temp_ts_data );

  /* Check to see what type notification routing is set */
  /* Note this lookup is based on index value           */
  if ( msg_status != WMS_OK_S )
  {
    /* Bad Message. Ignore the status report. */
    result = DSAT_ASYNC_EVENT;
  }
  else if ( cnmi_ds_val == 2 )
  {
    /* we are in store and route mode for status report */
    if ( mem_store == WMS_MEMORY_STORE_NONE )
    {
      /* There is no corresponding MO SMS in SIM. WMS did not save the */
      /* status report. No way to notify TE. Ignore the status report  */
      DS_AT_MSG0_HIGH("ATCOP: Save status report in RAM and send CDSI to TE");
      result = write_message_to_ram ( MT_STATUS_REPORT,
                                      &event_info->u.mt_msg,
                                      event_info->subs_id);
    }
    else
    {
      DS_AT_MSG0_HIGH("ATCOP: Handling <ds> val of 2 for status report");
      result =
        handle_mt_notification ( MT_STATUS_REPORT, 
                                 msg_index,
                                 mem_store,event_info->subs_id);
    }
  }
  else if ( cnmi_ds_val == 1 )
  {
    /* We are in direct notification mode */
    DS_AT_MSG0_HIGH("ATCOP: Handling <ds> val of 1 for status report");

    /* We are in direct notification mode*/
    result = write_message_to_ram ( MT_STATUS_REPORT, &event_info->u.mt_msg,
                     event_info->subs_id);
  } 
  else
  {
    /* Routing of status report is not set. Ignore the message */
    result = DSAT_ASYNC_EVENT;
  }

  dsatutil_free_memory((void *)temp_ts_data);

  return result;
}/* status_msg_report_event_handler */

/*===========================================================================

FUNCTION  MT_MESSAGE_ERROR_EVENT_HANDLER

DESCRIPTION
  Handler function for mt message error event from WMS.
  The event could be the one related to MT msg ack timeout.

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
LOCAL dsat_result_enum_type mt_message_error_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  int index;

  DS_AT_MSG0_HIGH("ATCOP SMS: MT Message Error Event Handler");

  /*Case to handle error recieved from Lower layers and Network Timeout*/
  if( ( event_info->u.err_report   == WMS_RPT_SMR_TR2M_TIMEOUT || 
        event_info->u.err_report   == WMS_RPT_LL_ERROR)  && 
      ( dsatsmsi_pres_cmd.cmd_name == SMS_CNMA ) )
  {
    boolean ack_required = TRUE;

    for (index = 0; index < 5; index++ )
    {
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CNMI_ETSI_IDX,event_info->subs_id,index,0,0,NUM_TYPE)
    }

    /* Now reset the routing configuration */
    (void) dsatsmsi_route_config ( MT_VAL_0, DS_VAL_0,event_info->subs_id );

    dsatetsismsi_free_element( ack_required,event_info->subs_id );

    waiting_for_an_ack = FALSE;

  }  
  return DSAT_ASYNC_EVENT;
}/* mt_message_error_event_handler */

/*===========================================================================

FUNCTION  STATUS_REPORT_WRITE_EVENT_HANDLER

DESCRIPTION
  Handler function for STATUS-REPORT write message event from WMS

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
dsat_result_enum_type status_report_write_event_handler 
(
 dsat_mode_enum_type mode, /* AT command mode    */
 sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsat_result_enum_type result = DSAT_OK;
  sms_cmd_name_e_type user_data;
  sms_mt_msg_e_type msg_type = MT_STATUS_REPORT;
  wms_memory_store_e_type mem_store;
  dsat_num_item_type cnmi_ds_val = 0;
  uint32 return_val;

  user_data = *((sms_cmd_name_e_type*)event_info->u.msg_status.user_data);
  mem_store = event_info->u.msg_status.msg_hdr.mem_store;

  DS_AT_MSG0_HIGH("ATCOP SMS: STATUS-REPORT Write Message Event Handler");

  if ( user_data != SMS_CNMA )
  {
    return DSAT_ASYNC_EVENT;
  }

  /* We need to send index at which the message was stored */
  return_val = event_info->u.msg_status.msg_hdr.index;

  cnmi_ds_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CNMI_ETSI_IDX,event_info->subs_id,CNMI_DS_VAL,NUM_TYPE);
  
  /* Send +CDSI result when status report is stored in RAM. */
  if ( cnmi_ds_val == 2 )
  {
    /* Status report is saved in RAM. Now send the indication to TE with
       stored memory index */
    DS_AT_MSG0_HIGH("ATCOP: Handling <ds> val of 2 for status report stored in RAM");
    result =
      handle_mt_notification ( MT_STATUS_REPORT, return_val,
           mem_store, event_info->subs_id);
    return result;
  }

  /* Wrote the directly routed message into RAM. Now check to see if we can */
  /* send the message to TE                                                 */
  if ( waiting_for_an_ack )
  {
    DS_AT_MSG0_HIGH("ATCOP SMS: TE busy. Saving the STATUS-REPORT");
    /* Already waiting for an ACK, add it to the queue. Will send later */
    add_mt_msg_to_queue(msg_type,return_val,WMS_MEMORY_STORE_RAM_GW,init_tid);
  }
  else
  {
    /* We are not waiting for an ACK. Add the message to queue, send to TE */
    add_mt_msg_to_queue(msg_type,return_val,WMS_MEMORY_STORE_RAM_GW,init_tid);
    if(dsatcmdp_at_state == DSAT_CMD)
    {
      DS_AT_MSG0_HIGH("ATCOP SMS: Reading the STATUS-REPORT from temp memory");
      dsatetsisms_flush_sms_indications(event_info->subs_id);
    }

    if ( msg_sent == TRUE )
    {
      waiting_for_an_ack = TRUE;
      msg_sent = FALSE;
    }
    else
    {
      waiting_for_an_ack = FALSE;
    } 
  }

  result = DSAT_ASYNC_EVENT;

  return result;
} /* status_report_write_event_handler */


/*===========================================================================

FUNCTION  STATUS_REPORT_READ_EVENT_HANDLER_EX

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
LOCAL dsat_result_enum_type status_report_read_event_handler_ex
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  wms_status_e_type status;
  dsat_err_e_type err_info = DSAT_ASYNC_EVENT;
  wms_client_ts_data_s_type *temp_ts_data = NULL;

  DS_AT_MSG0_HIGH("ATCOP SMS: STATUS REPORT Read Message Event Handler");

  if ( dsatsmsi_pres_cmd.cmd_name != SMS_CNMA )
  {
    /* unwanted event */
    return DSAT_ASYNC_EVENT;
  }
  
  /* Check whether the received WMS format is unsupported and if yes handle gracefully */
  if ( event_info->u.msg_status.u.gw_message.raw_ts_data.format != WMS_FORMAT_GW_PP )
  {
    DS_AT_MSG0_HIGH("Unsupported WMS format");

    /* We are done with the commands, clean up */
    err_info = DSAT_ERR_INVALID_FORMAT;
    goto send_result;
  }
  temp_ts_data = (wms_client_ts_data_s_type *) 
                      dsat_alloc_memory (sizeof(wms_client_ts_data_s_type), FALSE);

  /* decode the message */
  status = wms_ts_decode
  ( &event_info->u.msg_status.u.gw_message.raw_ts_data, temp_ts_data );

  if( status != WMS_OK_S )
  {
    /* We got an error while reading the message */
    DS_AT_MSG0_ERROR("ATCOP SMS: Error in STATUS REPORT decoding");

    /* We are done with the commands, clean up */
    err_info = DSAT_ERR_INVALID_OPERATION;
    goto send_result;
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

send_result:
  /* We are done with the commands, clean up */
  DSAT_SMS_STATE_CLEAN_ALL();
  dsatutil_free_memory((void *)temp_ts_data);

  DS_AT_MSG1_ERROR("status_report_read_event_handler_ex() err_info: %d" , err_info);
  return DSAT_ASYNC_EVENT;

} /* status_report_read_event_handler_ex */

#endif /* FEATURE_ETSI_SMS */
