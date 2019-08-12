/*!
  @file
  rfca.c

  @brief
  This module contains Control code for RFCA.
*/

/*==============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca.c#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/27/14   aki     Added sleep functions
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/10/13   aki     Mainline RFCA 
06/06/13   aki     Updates to remove KW warnings and errors
03/27/13   aki     Fix off-target compilation issues
10/05/12   aki     Cleanup
10/02/12   aki     Remove RFCA_diag_alloc and RFCA_diag_alloc_callback
07/11/12   aki     Increased the fixed size for immediate DIAG response to 1024
07/09/12   aki     Fixed size DIAG response, correctly copy of FTM log message
06/01/12   aki     NULL check for FTM sequencer log messages
05/17/12   aki     Support for immediate diag response allocation in RFCA_diag 
04/26/12   aki     Asserting if rfca tcb is not found
01/25/12   aki     Diag V2 delayed response implementation
10/28/11   aki     Initial revision

==============================================================================*/


#include "rfa_variation.h"
#include "comdef.h"
#include "stringl.h" /* for memscpy */

#include "ftm.h"
#include "ftmdiag.h"
#include "assert.h"
#include "rcinit_rex.h"
#include "err.h"
#include "rfca.h"
#include "rfca_task.h"
#include "rfca_mem_helper.h"
#include "modem_mem.h"
#include "queue.h"
#include "msg.h"
#include "diagcmd.h"
#include "rfca_dispatcher.h"

/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS
===========================================================================*/

const uint16 RFCA_IMMEDIATE_DIAG_RESPONSE_DEFAULT_SIZE = 1024;

/* Command queue for the test task.  All command to this task will be
** placed on this queue to await processing.
*/
q_type RFCA_task_cmd_q;

/* Queue for incoming FTM Seqeuncer log messages */
q_type RFCA_log_msg_q;

/*----------------------------------------------------------------------------*/
/*!  

  @brief
    Function puts command to RFCA_ task message queue.
  
  @details
    This function put given message to messages queue and signals RFCA_ task.
    
*/
void RFCA_cmd
(
   ftm_msg_type *msg_ptr   
)
{
  rex_tcb_type* rfca_tcb_p = rcinit_lookup_rextask("rfca");
  ASSERT(RCINIT_NULL != rfca_tcb_p);

  (void) q_link(msg_ptr, &msg_ptr->hdr.link );      /* Initialize link field */

  q_put( &RFCA_task_cmd_q, &msg_ptr->hdr.link );         /* Put on command queue */

  (void) rex_set_sigs(rfca_tcb_p, RFCA_CMD_Q_SIG ); /* Signal a queue event */

} /* End RFCA_cmd */


void  *RFCA_immediate_diag_response_buffer = NULL;
uint16 RFCA_immediate_diag_response_buffer_size = 0;

/*----------------------------------------------------------------------------*/
/*!  

  @brief
    Processes new command from incoming queue   

  @details
    This function handles requests to run tests and  other primitives
    by dispatching the appropriate functions and or macros.

*/
void RFCA_req(void)
{
  ftm_msg_type                   *msg_ptr;          /* Pointer to a msg               */

  /* Get a command from the Queue.
  */
  while ( ( msg_ptr = (ftm_msg_type *) q_get( &RFCA_task_cmd_q) ) != NULL)
  {
    
    RFCA_immediate_diag_response_buffer = msg_ptr->data_rsp_ptr;
    RFCA_immediate_diag_response_buffer_size = msg_ptr->rsp_pkt_len;

    if(NULL != msg_ptr->ftm_data_ptr)
    {
      // Call dispatcher
      RfcaReqDispatcher(msg_ptr->ftm_data_ptr);

      RFCA_Free(msg_ptr->ftm_data_ptr);
      msg_ptr->ftm_data_ptr = NULL;
    }

    RFCA_Free(msg_ptr);
    msg_ptr = NULL;
    
    // After prcessing the RFCA request RFCA_immediate_diag_response_buffer should be NULL
    // This is just a sanity check for now
    if((NULL != RFCA_immediate_diag_response_buffer) || (0 != RFCA_immediate_diag_response_buffer))
    {
      MSG_ERROR("Diag response buffer has not been set to NULL or size is not 0.", 0, 0, 0);
    }

  } /* While */
} /* End RFCA_req */


/*----------------------------------------------------------------------------*/
/*!  

  @brief
    Processes new command from incoming queue   

  @details
    This function handles requests to run tests and  other primitives
    by dispatching the appropriate functions and or macros.

*/
void RFCA_log_req(void)
{
  RFCA_log_msg_queue_item_type *msg_ptr;            

  // Get a command from the Queue
  while ( ( msg_ptr = (RFCA_log_msg_queue_item_type *) q_get( &RFCA_log_msg_q) ) != NULL)
  {
    if(NULL != msg_ptr->log_msg)
    {
      // Call dispatcher
      RfcaRespDispatcher(msg_ptr->log_msg->Header.length, (byte*)msg_ptr->log_msg);

      RFCA_Free(msg_ptr->log_msg);
      msg_ptr->log_msg = NULL;
    }

    RFCA_Free(msg_ptr);
    msg_ptr = NULL;
  } /* While */
} /* End RFCA_log_req */


/*----------------------------------------------------------------------------*/
/*!  

  @brief
    Function to send commands to RFCA_ task.
  
  @details
    This function allocates local buffer for message and then calls @ref RFCA_cmd.
    
*/

void *RFCA_diag
(
   void *msg_ptr,
   uint16 msg_size
)
{

  DIAG_RFCA_REQ *rfca_req = msg_ptr;

  ftm_msg_type *new_ftm_msg = (ftm_msg_type*)RFCA_Malloc(sizeof(ftm_msg_type));
  if(NULL == new_ftm_msg)
  {
    MSG_ERROR("Failed to allocate memory from modem heap for RFCA request. RFCA command (id=%u) will not be executed.\n", rfca_req->rfca_msg_id, 0, 0);
    return NULL;
  }

  new_ftm_msg->ftm_data_ptr = (ftm_msg_type*)RFCA_Malloc(msg_size);
  if(NULL == new_ftm_msg->ftm_data_ptr)
  {
    MSG_ERROR("Failed to allocate memory from modem heap for RFCA request. RFCA command (id=%u) will not be executed.\n", rfca_req->rfca_msg_id, 0, 0);
    
    RFCA_Free(new_ftm_msg);
    new_ftm_msg = NULL;    
    return NULL;
  }

  // Allocate memory for the response
  new_ftm_msg->data_rsp_ptr = (void*)diagpkt_subsys_alloc_v2((diagpkt_subsys_id_type)DIAG_SUBSYS_FTM, FTM_RFCA_C, RFCA_IMMEDIATE_DIAG_RESPONSE_DEFAULT_SIZE);
  if(NULL == new_ftm_msg->data_rsp_ptr)
  {
    MSG_ERROR("Failed to allocate memory for RFCA diag response. RFCA command (id=%u) will not be executed.\n", rfca_req->rfca_msg_id, 0, 0);

    RFCA_Free(new_ftm_msg->ftm_data_ptr);
    new_ftm_msg->ftm_data_ptr = NULL;

    RFCA_Free(new_ftm_msg);
    new_ftm_msg = NULL;
    return NULL;
  }

  new_ftm_msg->rsp_pkt_len = RFCA_IMMEDIATE_DIAG_RESPONSE_DEFAULT_SIZE;

  // Copy request message
  memscpy(new_ftm_msg->ftm_data_ptr, msg_size, msg_ptr, msg_size);
  // Initialize rest of the struct
  new_ftm_msg->req_pkt_len = msg_size;
  new_ftm_msg->hdr.done_q_ptr = NULL;
  //new_ftm_msg->hdr.link = NULL;
  RFCA_cmd(new_ftm_msg);

  return NULL;
}

ftm_async_response_type ftm_async_response = {0, NULL};


void RFCA_ftm_async_cmd_resp(ftm_rsp_pkt_type *rsp_pkt)
{  
  while(NULL != ftm_async_response.data)
  {
    ;
  }
  if(NULL != ftm_async_response.data)
  {
    MSG_ERROR("Cleaning up old ftm async response buffer, which should not be the case.", 0, 0, 0);
    RFCA_Free(ftm_async_response.data);
    ftm_async_response.data = NULL;
  }
  ftm_async_response.data = NULL;
  ftm_async_response.size = 0;

  if( (FTM_RSP_DO_NEW_PKT == rsp_pkt->cmd || FTM_RSP_DO_LEGACY == rsp_pkt->cmd) && (0 != rsp_pkt->pkt_len) ) // Everyting else is considered as failure
  {
    ftm_async_response.data = RFCA_Malloc(rsp_pkt->pkt_len);
    if(NULL != ftm_async_response.data)
    {
      memscpy(ftm_async_response.data, rsp_pkt->pkt_len, rsp_pkt->pkt_payload, rsp_pkt->pkt_len);
      ftm_async_response.size = (word)(rsp_pkt->pkt_len);
    }    
  }
  /* signal rfca to continue */
  {
    rex_tcb_type* rfca_tcb_p = rcinit_lookup_rextask("rfca");
    ASSERT(RCINIT_NULL != rfca_tcb_p);
    rex_set_sigs(rfca_tcb_p, RFCA_FTM_ASYNC_RESP_SIG);
  }
}

/*----------------------------------------------------------------------------*/
/*!  

  @brief
    Function to send log messages to RFCA_ task.
  
  @details   
    This function allocates local buffer for log message and then adds it to log queue.
    
*/
void RFCA_ftm_seq_log
(
   FTM_SEQUENCER_FTM_SequencerLog *log_ptr
)
{
  int payloadLength = 0;

  if(NULL == log_ptr)
  {
    // There's nothing to do with an empty log message
    return;
  }

  {
    RFCA_log_msg_queue_item_type *new_log_msg_item = NULL;
    FTM_SEQUENCER_FTM_SequencerLog * new_log_msg_buffer = NULL;
    payloadLength = sizeof(FTM_SEQUENCER_DiagLogHeader) + sizeof(FTM_SEQUENCER_FTM_SequencerLogHeader) + log_ptr->FTM_SequencerHeader.FTM_RESPONSE_SIZE;
    new_log_msg_item = (RFCA_log_msg_queue_item_type*)RFCA_Malloc(sizeof(RFCA_log_msg_queue_item_type));
    new_log_msg_buffer = (FTM_SEQUENCER_FTM_SequencerLog*)RFCA_Malloc(payloadLength);  
    
    if(NULL == new_log_msg_item || NULL == new_log_msg_buffer)
    {
      MSG_ERROR("Failed to allocate memory for log message from FTM seqeuncer.", 0, 0, 0);
      
      if(NULL != new_log_msg_buffer)
      {
        RFCA_Free(new_log_msg_buffer);
        new_log_msg_buffer = NULL;
      }
      
      if(NULL != new_log_msg_item)
      {
        RFCA_Free(new_log_msg_item);
        new_log_msg_item = NULL;
      }    
    }
    else
    {    
      // Copy message
      memscpy(new_log_msg_buffer, payloadLength, log_ptr, payloadLength);
      new_log_msg_item->log_msg = new_log_msg_buffer;

      (void) q_link(new_log_msg_item, &new_log_msg_item->link );      /* Initialize link field */

      q_put( &RFCA_log_msg_q, &new_log_msg_item->link );         /* Put on log message queue */
    
      {      
        rex_tcb_type* rfca_tcb_p = rcinit_lookup_rextask("rfca");
        ASSERT(RCINIT_NULL != rfca_tcb_p);
        (void) rex_set_sigs(rfca_tcb_p, RFCA_FTM_SEQ_LOG_MSG_SIG); /* Signal a queue event */
      }
    }
  }
  return;
}


/*----------------------------------------------------------------------------*/
// Copy of similar functions in diagmm.c to prevent RFCA task going to sleep
// while test execution is ongoing and delayed diag messages are pending
/*----------------------------------------------------------------------------*/

#include "npa.h" /* For npa_client_handle, etc */

npa_client_handle rfca_npa_handle = NULL;

/*----------------------------------------------------------------------------*/
/*!  

  @brief
    This callback is called when sleep task is ready for NPA handles to be created.
    
*/
static void RFCA_get_cpu_client_handle_cb( void *context,
                                           unsigned int event_type,
                                           void *data,
                                           unsigned int data_size )
{
  rfca_npa_handle = npa_create_sync_client( "/core/cpu/vdd",
                                            "RFCA_NPA_CLIENT",
                                            NPA_CLIENT_REQUIRED );
}

/*----------------------------------------------------------------------------*/
/*!  

  @brief
    This procedure initializes variables for RFCA sleep voting.
    
*/
void RFCA_sleep_init(void)
{  
 
  /* Registering a callback function to the resource to obtain its client
     and query handles. The CB will be called immediately if the 
     NPA node already exists, so it's safe to just register the CB here. */  
  npa_resource_available_cb( "/core/cpu/vdd", 
                             RFCA_get_cpu_client_handle_cb, 
                             NULL );

                                           
}

/*----------------------------------------------------------------------------*/
/*!  

  @brief
    This procedure votes for sleep for the RFCA task.
    
*/
void RFCA_allow_sleep(void)
{
  if( rfca_npa_handle )
  {
    npa_cancel_request( rfca_npa_handle );
  }
  else
  {
    MSG_ERROR("Cannot vote for sleep: rfca_npa_handle is null", 0, 0, 0);
  }
}

/*----------------------------------------------------------------------------*/
/*!  

  @brief
      This procedure votes against sleep for the RFCA task.
    
*/
void RFCA_forbid_sleep(void)
{
  if( rfca_npa_handle )
  {
      npa_issue_required_request( rfca_npa_handle, 1);
  }
  else
  {
    MSG_ERROR("Cannot vote against sleep: rfca_npa_handle is null", 0, 0, 0);
  }
}

