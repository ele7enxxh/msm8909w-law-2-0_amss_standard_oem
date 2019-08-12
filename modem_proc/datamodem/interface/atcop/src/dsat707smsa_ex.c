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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707smsa_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/15   sc      Fixed issue in event handling to use subs_id provided in 
                   event info.
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

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
#include "dsatdl.h"
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

/*===========================================================================
            Data types
===========================================================================*/

/* import variable from other files of sms module */
/* Variable to find out what command is being processed currently */
extern dsat_sms_cmd_s_type  dsatsmsi_pres_cmd;

/* SMS Mobile Terminated message queue */
extern q_type is707_sms_mt_msg_q;         

/* Queue for storing free message buffers */ 
extern q_type is707_sms_mt_msg_free_q;

wms_client_ts_data_s_type *temp707_ts_data = NULL;

/* LOCAL function prototypes */
dsat_result_enum_type is707_received_msg_event_handler 
(
 dsat_mode_enum_type mode,           /* AT command mode    */
 sms_event_info_s_type * event_info  /* Event info ptr    */
);

LOCAL void dsat707smsi_send_unsol_mt_msg 
(
 sms_event_info_s_type * event_info     /* Event info ptr    */
);

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
/* the Message Event Handler lookup table */
/* note that the order of the functions has to match wms_msg_event_e_type */
const msg_handler_tab_type dsat707sms_msg_handler_table_entries_ex[] = 
{
  { SMS_CFG_EVENT_CDMA_READY,       dsatsmsi_cdma_ready_cfg_event_handler       },
  { SMS_CFG_EVENT_MEMORY_STATUS,    dsatsmsi_memory_status_cfg_event_handler_ex },
  { SMS_MSG_EVENT_RECEIVED_MESSAGE, is707_received_msg_event_handler            },
};

const dsatsms_msg_handler_tab_type dsat707sms_msg_handler_table_ex =
{
  dsat707sms_msg_handler_table_entries_ex,
  ARR_SIZE(dsat707sms_msg_handler_table_entries_ex)
};
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

/*===========================================================================

FUNCTION DSAT707SMS_FLUSH_SMS_INDICATIONS

DESCRIPTION
  Flushes buffer of SMS incoming message indications and sends indications
  to TE. Indications are buffered and sent based on AT parameter setting

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void dsat707sms_flush_sms_indications
(
 void
)
{
  sms_mt_msg_type  *msg_ptr;
  
  /* Check to see the user preferences        */
  /* Flush the indications only if <bfr> is 0 */

  /* The formatting of the response is done based on the current */
  /* functionality that is supported: <mt>: 0, 1 and <ds>: 0, 2  */
  /* We need to check for <mt> and <ds> for formatting the resp  */


  if( ((dsat_num_item_type)dsatutil_get_val(
             DSAT707_SMS_QCNMI_IDX,0,CNMI_BFR_VAL,NUM_TYPE) == 0) && 
      ((dsat_num_item_type)dsatutil_get_val(
             DSAT707_SMS_QCNMI_IDX,0,CNMI_MODE_VAL,NUM_TYPE) != 0) )
  {
    /* Flush the queued indications to TE */

    /* Get messages from the queue until the queue is empty */
    while( (msg_ptr = (sms_mt_msg_type *)q_get( &is707_sms_mt_msg_q )) != NULL )
    {
      /* Check to see if user selected the option */
      /* to route DELIVER or STATUS-REPORT        */
      if( (msg_ptr->msg_type == MT_DELIVER) && 
          ((dsat_num_item_type)dsatutil_get_val(
                DSAT707_SMS_QCNMI_IDX,0,CNMI_MT_VAL,NUM_TYPE) == 0))
      {
        /* We are in do not send the msg to TE mode    */
        /* Return the message buffer to the free queue */
        DS_AT_MSG0_HIGH("Buffer mode, add it to local queue");
        q_put( &is707_sms_mt_msg_free_q, &msg_ptr->link );
        continue;
      }
      else
      {
        /* Send the indication to TE */
        DS_AT_MSG0_HIGH("Sending the indication to TE");
        dsatsmsi_send_async_indication_to_te (
                                      msg_ptr->msg_type,
                                      msg_ptr->index,
                                      msg_ptr->mem_store,
                                      CDMA_CMD_MODE,
                                      dsat_get_current_1x_subs_id()
                                    );
    
        /* Return the message buffer to the free queue */
        q_put( &is707_sms_mt_msg_free_q, &msg_ptr->link );
      }
    }/* while */
  }
  else if( ((dsat_num_item_type)dsatutil_get_val(
             DSAT707_SMS_QCNMI_IDX,0,CNMI_BFR_VAL,NUM_TYPE) == 1) && 
           ((dsat_num_item_type)dsatutil_get_val(
             DSAT707_SMS_QCNMI_IDX,0,CNMI_MODE_VAL,NUM_TYPE) != 0) )
  {
    /* Clear the queued indications */
    while( (msg_ptr = (sms_mt_msg_type *)q_get( &is707_sms_mt_msg_q )) != NULL )
    {
      /* Return the message buffer to the free queue */
      q_put( &is707_sms_mt_msg_free_q, &msg_ptr->link );
    }
  }
  else
  {
    /* Do nothing. Just return */
    return;
  }

  return;
}/* dsat707sms_flush_sms_indications */

/*===========================================================================

FUNCTION DSAT707SMSI_SEND_UNSOL_MT_MSG

DESCRIPTION
  This Function sends a unsolicited Indication to TE on a reception of MT SMS
  without storing it in the TA.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/

void dsat707smsi_send_unsol_mt_msg 
(
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsat707smsa_send_unsol_mt_msg_fp))
  {
    dsatdl_vtable.dsat707smsa_send_unsol_mt_msg_fp(event_info);
  }

  return;
} /* dsat707smsi_send_unsol_mt_msg */


/*===========================================================================

FUNCTION  IS707_RECEIVED_MSG_EVENT_HANDLER

DESCRIPTION
  Handler function for received message (incoming) event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type is707_received_msg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
 sms_event_info_s_type * event_info     /* Event info ptr    */
)
{
  wms_message_index_type  msg_index;
  wms_memory_store_e_type mem_store;
#ifdef FEATURE_DSAT_EXTENDED_SMS
  boolean delivery_ack_recvd = FALSE;
  wms_status_e_type msg_status;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

/*-------------------------------------------------------------------------*/

  DS_AT_MSG2_HIGH("is707_received_msg_event_handler, pres_cmd %d HC cmd %d",
                dsatsmsi_pres_cmd.cmd_name,dsatsmsi_pres_cmd.is_hc_cmd);
  
  ASSERT( event_info != NULL );
  
  mem_store = event_info->u.mt_msg.msg_hdr.mem_store;
  msg_index = event_info->u.mt_msg.msg_hdr.index;
  temp707_ts_data = (wms_client_ts_data_s_type *) dsat_alloc_memory(sizeof(wms_client_ts_data_s_type),
                                                                    FALSE);
#ifdef FEATURE_DSAT_EXTENDED_SMS
  msg_status = wms_ts_decode (&event_info->u.mt_msg.u.cdma_message.raw_ts,
                              temp707_ts_data );

  if ( msg_status == WMS_OK_S )
  {
    if (temp707_ts_data->u.cdma.message_id.type == WMS_BD_TYPE_DELIVERY_ACK) 
    {
      DS_AT_MSG0_MED("Delivery Ack received");
      delivery_ack_recvd = TRUE;
    }
  }
#endif /* FEATURE_DSAT_EXTENDED_SMS */

  if ( !dsatcmdp_block_indications_ex() )
  {
    /* ATCoP is not Busy */
    if( ( (dsat_num_item_type)dsatutil_get_val(
              DSAT707_SMS_QCNMI_IDX,0,CNMI_MODE_VAL,NUM_TYPE) == 0 )

#ifndef FEATURE_DSAT_EXTENDED_SMS
        && ( (dsat_num_item_type)dsatutil_get_val(
                DSAT707_SMS_QCNMI_IDX,0,CNMI_MT_VAL,NUM_TYPE) != 2 )
#endif /* FEATURE_DSAT_EXTENDED_SMS */
       )
    {
      /* <mode> equals 0 Hence we need to buffer */
      queue_mt_msg( MT_DELIVER, msg_index, mem_store, 0 ,CDMA_CMD_MODE);
      goto send_result;
    }
    else if( (dsat_num_item_type)dsatutil_get_val(
                 DSAT707_SMS_QCNMI_IDX,0,CNMI_MT_VAL,NUM_TYPE)== 0 )
    {
      /* User selected the option not to route */
      goto send_result;
    }
    else
    {
      /* Check to see if we have any outstanding notifications */
      /* If yes, send them to TE so that they are in order     */
      dsat707sms_flush_sms_indications( );

      if (temp707_ts_data->u.cdma.download_mode != WMS_DOWNLOAD_MODE_UPDATE_PRL) 
      {
        mem_store = WMS_MEMORY_STORE_NONE;
      }
#ifdef FEATURE_DSAT_EXTENDED_SMS
      if (delivery_ack_recvd) 
      {
        if ((dsat_num_item_type)dsatutil_get_val(
                 DSAT707_SMS_QCNMI_IDX,0,CNMI_DS_VAL,NUM_TYPE) == 2) 
        {
          dsatsmsi_send_async_indication_to_te ( MT_STATUS_REPORT, 
                                                 msg_index, 
                                                 mem_store,
                                                 CDMA_CMD_MODE,
                                                 event_info->subs_id); /* +CDSI */
        } 
        else if ((dsat_num_item_type)dsatutil_get_val(
                 DSAT707_SMS_QCNMI_IDX,0,CNMI_DS_VAL,NUM_TYPE) == 1) 
        {
          dsat707smsi_send_unsol_mt_msg(event_info);  /* CDS */
        }
      }
      else
#endif /* FEATURE_DSAT_EXTENDED_SMS */
      {
        if( ( (dsat_num_item_type)dsatutil_get_val(
             DSAT707_SMS_QCNMI_IDX,0,CNMI_MT_VAL,NUM_TYPE) == 2 ) ||
            ( (dsat_num_item_type)dsatutil_get_val(
             DSAT707_SMS_QCNMI_IDX,0,CNMI_MT_VAL,NUM_TYPE) == 3  ))
        {
          dsat707smsi_send_unsol_mt_msg(event_info);
        }
        else
        {
          /* Send the present indications directly to TE */
          dsatsmsi_send_async_indication_to_te ( MT_DELIVER, 
                                                 msg_index, 
                                                 mem_store, 
                                                 CDMA_CMD_MODE,
                                                 event_info->subs_id);
        }
      }
      goto send_result;
    }
  }
  else
  {
    /* ATCoP is Busy */
    if ( (dsat_num_item_type)dsatutil_get_val(
              DSAT707_SMS_QCNMI_IDX,0,CNMI_MODE_VAL,NUM_TYPE) != 1 )
    {
#ifdef FEATURE_DSAT_EXTENDED_SMS
     /* when configured for ^HCMT message, ignore */
      if(!delivery_ack_recvd && ((dsat_num_item_type)dsatutil_get_val(
               DSAT707_SMS_QCNMI_IDX,0,CNMI_MT_VAL,NUM_TYPE) == 2) )  
      {
        DS_AT_MSG0_HIGH("Not Buffering as RAM in CDMA is not supported");
        goto send_result;
      }
      else if (delivery_ack_recvd && ((dsat_num_item_type)dsatutil_get_val(
               DSAT707_SMS_QCNMI_IDX,0,CNMI_DS_VAL,NUM_TYPE) != 2) ) 
      {
        goto send_result;
      }
      
      /* buffer the indication based on user preference */
      /* it will be flushed to TE at a later stage      */
      queue_mt_msg (delivery_ack_recvd ? MT_STATUS_REPORT : MT_DELIVER, msg_index, mem_store, 0,CDMA_CMD_MODE );
#else
      if((dsat_num_item_type)dsatutil_get_val(
                 DSAT707_SMS_QCNMI_IDX,0,CNMI_MT_VAL,NUM_TYPE) == 2)   /* when configured for CMT message, ignore */
      {
        DS_AT_MSG0_HIGH("Not Buffering as RAM in CDMA is not supported");
        goto send_result;
      }
      /* buffer the indication based on user preference */
      /* it will be flushed to TE at a later stage      */
      queue_mt_msg (MT_DELIVER, msg_index, mem_store, 0 , CDMA_CMD_MODE);
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    }
  }

/* Tell ATCOP not to send any result code and return ASYNC event and free the buffer*/
send_result:
  dsatutil_free_memory_ext((void **) &temp707_ts_data);

  return DSAT_ASYNC_EVENT;
} /* is707_received_msg_event_handler */

#endif /* FEATURE_CDMA_SMS */
