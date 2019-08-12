/*===========================================================================

                         Q M I _ M M C _ S V C . C

DESCRIPTION

 The QMI MMODE Services Source file. Contains routines common to
 all QMI services.

 Note this library assumes caller is running in QMI-MMODE task context.

EXTERNALIZED FUNCTIONS

   qmi_svc_prepend_msg_hdr()
     Append the message header type and length to the provided QMI service
     message payload

Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_mmode_svc.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12     sk     Initial changes

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "dsm.h"
#include "task.h"
#include "modem_mem.h"
#include "qmi_svc_defs.h"
#include "qmi_svc_utils.h"
#include "qmi_framework.h"
#include "qmi_mmode_svc.h"
#include "qmi_voice.h"
#include "ps_in.h"
#include "qmi_mmode_task_cmd.h"
#include "qmi_nas_internal.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "qm_nv.h"
#include "qm_cbpq.h"
#include "qm_si.h"
#include "qm_nas_clnt.h"
#include "qm_meas.h"
#include "qm_mstats.h"
#include "qm_timer.h"
#include "qm_efs.h"

/*===========================================================================
                    INTERNAL FUNCTION DEFINITIONS
===========================================================================*/


/*===========================================================================
  FUNCTION QMI_MMODE_SVCI_GET_TRANSACTION_CMD_BUF()

  DESCRIPTION
    Allocate a QMI service command buffer

  PARAMETERS
    x_p : QMI Transaction that the command buf will be added to

  RETURN VALUE
    pointer to allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_cmd_buf_type *  qmi_mmode_svci_get_transaction_cmd_buf
(
  qmi_transaction_type *  x_p
)
{
  qmi_cmd_buf_type *     cmd_buf_p;
  q_type *               q_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (x_p);
  if (x_p->n_cmds >= MAX_MSGS_IN_BUNDLE)
  {
    QM_MSG_MED("Too many commands in transaction");
    return NULL;
  }

  if (x_p->cl_sp)  // cl_sp != NULL means this transaction is client specific
  {
    q_ptr = &x_p->cl_sp->cmd_free_q;
    cmd_buf_p = q_get(q_ptr);
    if (NULL == cmd_buf_p)
    {
      QM_MSG_MED("No command buffers available!");
      return NULL;
    }
  }
  else
  {
      QM_MSG_MED("Invalid state pointer !");
      return NULL;
  }

  cmd_buf_p->x_p    = x_p;
  cmd_buf_p->cmd_no = x_p->n_cmds;
  cmd_buf_p->in_progress = FALSE;
  x_p->cmd_list[x_p->n_cmds] = cmd_buf_p;
  x_p->resp_list[x_p->n_cmds] = QMI_SVC_RESPONSE_PENDING;
  x_p->n_cmds++;
  return cmd_buf_p;

} /* qmi_mmode_svci_get_transaction_cmd_buf() */


/*===========================================================================
  FUNCTION QMI_MMODE_SVCI_GET_TRANSACTION()

  DESCRIPTION
    Get a transaction buffer for the QMI
MMODE service 
    by the client state pointer provided

  PARAMETERS
    svc_sp :  pointer to service state from which the transaction is allocated
    cl_sp :  client state pointer for client to get transaction buffer for
             or NULL if sending a broadcast indication

  RETURN VALUE
    NULL if no transaction buffer is available
    otherwise, pointer to transaction buffer

  DEPENDENCIES
    QMI service must be initialized and registered with Framework
 
  SIDE EFFECTS
    service global transaction might be allocated.
===========================================================================*/
static qmi_transaction_type *  qmi_mmode_svci_get_transaction
(
  void *  svc_sp,
  qmi_common_client_state_type *   cl_sp
)
{
  qmi_transaction_type *  x_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(svc_sp);

  x_p = NULL;

  if (cl_sp)
  {
    x_p = q_get( &cl_sp->x_free_q );
  }
  else /* if no client sp, must be a broadcast indication */
  {
      QM_MSG_ERROR("Invalid client state pointer!");
      return NULL;
  }

  if (x_p)
  {
    x_p->cl_sp = cl_sp;
    x_p->svc_sp =  svc_sp;
  }
  return x_p;
} /* qmi_mmode_svci_get_transaction() */



/*===========================================================================
  FUNCTION QMI_MMODE_SVCI_FREE_TRANSACTION()

  DESCRIPTION
    Frees the passed transaction buffer

  PARAMETERS
    x_p :  handle to transaction pointer pointing to transaction buffer to
           be freed.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI Voice service must be initialized and registered with Framework

  SIDE EFFECTS
    Nulls out the caller's transaction buffer pointer
===========================================================================*/
static void qmi_mmode_svci_free_transaction
(
  qmi_transaction_type **  x_p
)
{
  void *         svc_sp;
  qmi_common_client_state_type *  cl_sp;
  int i;
  boolean free_x;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (x_p);
  if (*x_p == NULL)
  {
    QM_MSG_MED("Attempt to free a NULL transaction pointer!");
    return;
  }
  svc_sp = (void *) (*x_p)->svc_sp; 
  cl_sp = (*x_p)->cl_sp;

  free_x = TRUE;
  for( i = 0; i < MAX_MSGS_IN_BUNDLE; i++ )
  {
    if( (*x_p)->resp_list[i] != (dsm_item_type *) QMI_SVC_RESPONSE_NONE &&
        (*x_p)->resp_list[i] != (dsm_item_type *) QMI_SVC_RESPONSE_PENDING )
    {
      dsm_free_packet( &((*x_p)->resp_list[i]));
    }
    if( (*x_p)->req_list[i] )
    {
      dsm_free_packet( &((*x_p)->req_list[i]) );
    }
    if ((*x_p)->cmd_list[i])
    {
      if( FALSE == (*x_p)->cmd_list[i]->in_progress )
      {
        qmi_mmode_svc_free_transaction_cmd_buf( &((*x_p)->cmd_list[i]) );
      }
      else
      {
        /*-------------------------------------------------------------------
          don't free cmd buf if it's in process (i.e. currently executing,
          called free_transaction as part of processing, e.g. reset, abort)
        -------------------------------------------------------------------*/
        free_x = FALSE;
        QM_MSG_MED("Freeing transaction while command in progress - deferred.");
        /* ensure partially freed transaction doesn't get dispatched in this
           case - queued response for pending command should be freed after
           processing is complete. */
      }
    }
  }

  if (free_x)
  {
    memset( *x_p, 0, sizeof(qmi_transaction_type) );

    (void) q_link( *x_p, &((*x_p)->link) );
    (*x_p)->cl_sp = (qmi_common_client_state_type *)cl_sp;
    (*x_p)->svc_sp = svc_sp;
    if (cl_sp)
    {
      q_put( &cl_sp->x_free_q, &((*x_p)->link) );
    }
    *x_p = NULL;
  }
  else
  {
    QM_MSG_MED("Transaction in process, will be freed once current cmd is completed");
  }
} /* qmi_mmode_svci_free_transaction() */



/*===========================================================================
  FUNCTION QMI_VOICEI_INPUT()

  DESCRIPTION
    Process the incoming QMI message and send the response back.
    This function is registered with QMUX.

  PARAMETERS
    cmd_buf_p_in : command buffer pointer
    sdu_in    : message to be processed

  RETURN VALUE
    TRUE:   message processed successfully
    FALSE:  invalid message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_mmode_svci_input
(
  qmi_framework_common_msg_hdr_type     *cmn_msg_hdr,
  qmi_mmode_svc_config_type *            svc_cfg,
  qmi_cmd_buf_type *                    cmd_buf_p,
  dsm_item_type **                      sdu_in
)
{
  qmi_common_client_state_type * cl_sp;
  dsm_item_type *          response_ptr = NULL;
  qmi_svc_cmd_hdlr_type *  cmd_hdlr;
  uint16                   cmd;
  boolean                  retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cmd_buf_p);
  ASSERT (sdu_in);
  ASSERT (svc_cfg);
  ASSERT (cmn_msg_hdr);

  QM_MSG_MED_2("Got QMI Service cmd type %d (%d bytes)", cmd_buf_p->cmd_type, dsm_length_packet(*sdu_in));

  cl_sp =  cmd_buf_p->x_p->cl_sp;

  ASSERT (cl_sp);

  /*-------------------------------------------------------------------------
    Checking to see if a cmd hndler is registerd for the input command
  -------------------------------------------------------------------------*/
  cmd_hdlr = svc_cfg->cmd_hdlr_array;
  for (cmd = 0; cmd < svc_cfg->cmd_num_entries; cmd++, cmd_hdlr++)
  {
    if (cmd_buf_p->cmd_type == cmd_hdlr->cmd_type)
    {
      break; 
    }
  }

  if( cmd == svc_cfg->cmd_num_entries || cmd_hdlr->request_hdlr == NULL )
  {
    QM_MSG_MED_2("Unrecognized type (=%d) for service (=%d)! Return error", cmd_buf_p->cmd_type, svc_cfg->service_id);

    retval = qmi_svc_put_result_tlv( &response_ptr,
                                     QMI_RESULT_FAILURE,
                                     QMI_ERR_INVALID_QMI_CMD );
    if (FALSE == retval)
    {
      dsm_free_packet(&response_ptr);
      response_ptr = NULL;
    }
  }
  else
  {
    cmd_buf_p->in_progress = TRUE;
    if((cmd_buf_p->x_p->ctl & QMI_FLAG_MASK_MSGTYPE) == QMI_FLAG_MSGTYPE_CMD)
    {
      QM_MSG_MED_1( "Handling %s", cmd_hdlr->msg_desc);
      response_ptr = cmd_hdlr->request_hdlr( svc_cfg->svc_sp, cmd_buf_p, cl_sp, sdu_in );
    }
    else
    {
      QM_MSG_MED_2("Unhandled (%d cmd) msg type (%d)", (cmd_buf_p->x_p->ctl & QMI_FLAG_MASK_MSGTYPE), cmd_buf_p->cmd_type);
      response_ptr = NULL;
    }
  }

  cmd_buf_p->in_progress = FALSE;
  /*-------------------------------------------------------------------------
    send response if ready.  Check for need to queue this command in 
    pending queue.
  -------------------------------------------------------------------------*/
  if (response_ptr == NULL)
  {
    QM_MSG_MED("(Service generated no response)");
    qmi_mmode_svc_free_transaction_cmd_buf(&cmd_buf_p);
    return FALSE;
  }
  else if (response_ptr == QMI_SVC_RESPONSE_PENDING)
  {
    QM_MSG_MED_1("Response pending for cmd_buf_p %#X", cmd_buf_p);
    // command buffer will be freed later when response is completed
    return TRUE;
  }
  return qmi_mmode_svc_send_response(cmn_msg_hdr, cmd_buf_p, response_ptr );
} /* qmi_mmode_svci_input() */

/*===========================================================================
  FUNCTION QMI_MMODE_SVCI_QUEUE_RESPONSE()

  DESCRIPTION
    Schedule the response to be sent back to the client The bundled response
    will be sent when all responses have been queued.

  PARAMETERS
    cmd_buf_p :  The command buffer for the request for which the response
                 is being queued.
    msg       :  A DSM chain containing the response

  RETURN VALUE
    TRUE  -  Success
    FALSE -  Failure

  DEPENDENCIES
    none

  SIDE EFFECTS
    The command buffer pointed to by cmd_buf_p will be freed.
    The dsm chain referenced by *msg is handled herein.  Caller should 
    never reference the pointer again after this call.
===========================================================================*/
static boolean qmi_mmode_svci_queue_response
( 
  qmi_cmd_buf_type **  cmd_buf_p,
  dsm_item_type **     msg
)
{
  qmi_transaction_type *  x_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cmd_buf_p);
  ASSERT (*cmd_buf_p);
  ASSERT ((*cmd_buf_p)->x_p);
  ASSERT (msg);
  /*-------------------------------------------------------------------------
    find this outstanding transaction and save the response.
    If all responses are present, generate the response bundle and send
  -------------------------------------------------------------------------*/
  x_p = (*cmd_buf_p)->x_p;
  
  /*-------------------------------------------------------------------------
    check if cmd buffer was one of the cmds in the transaction.
  -------------------------------------------------------------------------*/
  if( (*cmd_buf_p) != x_p->cmd_list[(*cmd_buf_p)->cmd_no] )
  {
    /*-----------------------------------------------------------------------
      If cmd was not part of the associated transaction, bad news!
    -----------------------------------------------------------------------*/
    QM_MSG_ERROR("Command is linked to transaction it doesn't belong to! discarding");
    ASSERT(0);  // happens if command buf is freed while being processed
    dsm_free_packet( msg );
    qmi_mmode_svc_free_transaction_cmd_buf( cmd_buf_p );
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If so, save the response dsm pointer in the transaction
    If response dsm pointer is null, the free command buffer call below
    will set the pointer to 1 to indicate that processing is complete but
    no response was generated.
  -------------------------------------------------------------------------*/
  x_p->cmd_list[(*cmd_buf_p)->cmd_no]  = NULL;
  x_p->resp_list[(*cmd_buf_p)->cmd_no] = *msg;
  *msg = NULL;

  /*-------------------------------------------------------------------------
    The command buffer is no longer needed so free it up for reuse
  -------------------------------------------------------------------------*/
  qmi_mmode_svc_free_transaction_cmd_buf( cmd_buf_p );
  return TRUE;

} /* qmi_mmode_svci_queue_response() */


/*===========================================================================
  FUNCTION QMI_MMODE_SVCI_DISPATCH_TRANSACTION()

  DESCRIPTION
    Dispatches all QMI commands in a passed transaction.
    This should be called after the command handlr function has unbundled the
    request.

  PARAMETERS
    x_p : pointer to transaction containing commands to dispatch

  RETURN VALUE
    None

  DEPENDENCIES
    QMI Voice service must be initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_mmode_svci_dispatch_transaction
(
  qmi_framework_common_msg_hdr_type     *cmn_msg_hdr,
  qmi_mmode_svc_config_type *            svc_cfg,
  qmi_transaction_type *  x_p
)
{
  qmi_cmd_buf_type *  cmd_buf_p;
  dsm_item_type *     msg;
  int                 i;
  boolean             free_t = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Dispatch each of the commands in the transaction
  -------------------------------------------------------------------------*/
  for( i = 0; i < x_p->n_cmds; i++ )
  {
    cmd_buf_p = x_p->cmd_list[i];
    msg = x_p->req_list[i];
    x_p->req_list[i] = NULL;

    /*-----------------------------------------------------------------------
      Dispatch the current SDU/msg transaction
    -----------------------------------------------------------------------*/
    if( FALSE == qmi_mmode_svci_input(cmn_msg_hdr, svc_cfg, cmd_buf_p, &msg ) )
    {
      QM_MSG_MED_3("Transaction %x Command #%d (%x) processing failed", x_p, i, cmd_buf_p);
      free_t = TRUE;
     //  we should ensure the transaction doesn't stall here.
    }

    /*-----------------------------------------------------------------------
      Ensure the input SDU is freed.  Command handlers don't need to worry
      about freeing the input message.
    -----------------------------------------------------------------------*/
    dsm_free_packet ( &msg );
  }
  if(TRUE == free_t)
  {
    // this indicates that qmi_mmode_svci_input() failed for one of the bundled 
    // commands in the transaction. This failure could be either problem
    // with assembling response or invalid command type was sent to the
    // service. In either case right thing to do is to cleanup transaction
    // at this point, otherwise it will never be cleaned up.
    qmi_mmode_svci_free_transaction( &x_p );
  }
} /* qmi_mmode_svci_dispatch_transaction() */



/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

void qmi_mmode_svc_req_hdlr
(
  qmi_mmode_svc_config_type *  svc_cfg,
  qmi_framework_msg_hdr_type* msg_hdr,
  qmi_common_client_state_type* cl_sp,
  dsm_item_type * sdu_in 
)
{
  int32                         temp;
  int16                         svc_id;
  uint16                        cmd_type;
  uint16                        sdu_len;
  uint16                        remaining_bytes;
  dsm_item_type *               next_sdu;
  qmi_transaction_type *  x_p;
  qmi_cmd_buf_type *   cmd_buf_p;
  
/*-------------------------------------------------------------------------*/
  ASSERT(svc_cfg);
  ASSERT(svc_cfg->svc_sp);
  ASSERT(msg_hdr);
  ASSERT(sdu_in);
  ASSERT(msg_hdr->common_hdr.client_id != QMI_FRAMEWORK_SVC_MAX_CLIENTS);
  ASSERT(cl_sp);

  svc_id = svc_cfg->service_id;
  QM_MSG_MED_3("QMI svc(%d) Request handler:  clid%d, tx_id:%d ", svc_id, msg_hdr->common_hdr.client_id, msg_hdr->common_hdr.transaction_id);

  /*-------------------------------------------------------------------------
    Get a transaction buffer to track all contained commands in the bundle
  -------------------------------------------------------------------------*/
  x_p = qmi_mmode_svci_get_transaction(svc_cfg->svc_sp, cl_sp);
  if (x_p == NULL)
  {
    QM_MSG_MED_1("No transaction container!  Discarding service %d message", svc_id);
    dsm_free_packet(&sdu_in);
    return; 
  }

  x_p->ctl  = msg_hdr->msg_ctl_flag;
  x_p->x_id = msg_hdr->common_hdr.transaction_id;

  /*-------------------------------------------------------------------------                  
    Parse the incoming service message
  -------------------------------------------------------------------------*/
  next_sdu = NULL;
  remaining_bytes = (uint16) dsm_length_packet(sdu_in);
  while (sdu_in)
  {
    /*-----------------------------------------------------------------------
      Extract service message header
    -----------------------------------------------------------------------*/
    temp = dsm_pull16( &sdu_in ); 
    if (temp == -1)
    {
      QM_MSG_MED_1("Short QMI svc header!  Discarding transaction %d", x_p->x_id);
      dsm_free_packet( &sdu_in );
      qmi_mmode_svci_free_transaction( &x_p );
      return;
    }
    cmd_type = ps_ntohs(temp);

    temp = dsm_pull16( &sdu_in );
    if (temp == -1)
    {
      QM_MSG_MED_1("Short QMI svc header!  Discarding transaction %d", x_p->x_id);
      dsm_free_packet( &sdu_in );
      qmi_mmode_svci_free_transaction( &x_p );
      return;
    }
    sdu_len = ps_ntohs(temp);
    remaining_bytes -= 4;

    if (sdu_len > remaining_bytes)
    {
      QM_MSG_MED_1("Short QMI VOICE svc payload!  Discarding service %d message", svc_id);
      dsm_free_packet( &sdu_in );
      qmi_mmode_svci_free_transaction( &x_p );
      return;
    }
    else if (sdu_len < remaining_bytes)
    {
      /*---------------------------------------------------------------------
        save subsequent SDUs and isolate the current by trimming,
        unbundling individual commands into separate dsm chains
      ---------------------------------------------------------------------*/
      ASSERT (next_sdu == NULL);
      temp = dsm_dup_packet( &next_sdu, sdu_in, sdu_len, 65535 );      
      if (0 == temp)
      {
        QM_MSG_MED("No DUP items for bundled command!  dropping");
        dsm_free_packet( &next_sdu ); // is this needed?
        dsm_free_packet( &sdu_in );
        qmi_mmode_svci_free_transaction( &x_p );
        return;
      }
      dsm_trim_packet( &sdu_in, sdu_len );
    }

    /*-----------------------------------------------------------------------
      sdu_in now contains a single command.
      Get a QMI command buffer to hold the command
    -----------------------------------------------------------------------*/
    ASSERT (x_p);
    cmd_buf_p = qmi_mmode_svci_get_transaction_cmd_buf( x_p );
    if (cmd_buf_p == NULL)
    {
      QM_MSG_MED_1("No QMI command buffer!  Discarding service %x message", svc_id);
      dsm_free_packet(&sdu_in);
      qmi_mmode_svci_free_transaction( &x_p );
      return;
    }

    cmd_buf_p->cmd_type = cmd_type;
    x_p->req_list[x_p->n_cmds-1] = sdu_in;

    /*-----------------------------------------------------------------------
      move to next sdu in bundle, if present
    -----------------------------------------------------------------------*/
    sdu_in = next_sdu;
    next_sdu = NULL;
    remaining_bytes -= sdu_len;
  }
  QM_MSG_MED_3("QMI Svc dispatch transaction(%x) with (%d cmds) ctl_flag= %x", x_p->x_id, x_p->n_cmds, x_p->ctl);
  qmi_mmode_svci_dispatch_transaction(&msg_hdr->common_hdr,svc_cfg,x_p);

}/* qmi_mmode_svc_req_hdlr */


/*===========================================================================
FUNCTION QMI_MMODE_SVC_SEND_RESPONSE()

DESCRIPTION
  This function calls QMI Framework API to send a Service response to the
  client.

PARAMETERS 
  cmd_buf_p: command buffer
  msg_ptr : dsm item

RETURN VALUE
  TRUE: If responses send to Framework
  FALSE: otherwise

DEPENDENCIES
  QMI Voice service must be initialized and registered with Framework

SIDE EFFECTS
  None
===========================================================================*/
boolean qmi_mmode_svc_send_response
(
  qmi_framework_common_msg_hdr_type *  common_hdr,
  qmi_cmd_buf_type *                 cmd_buf_p,
  dsm_item_type *                    msg_ptr
)
{
  qmi_transaction_type * x_p;
  int                          i;
  qmi_framework_msg_hdr_type msg_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT (common_hdr);
  ASSERT (cmd_buf_p);
  ASSERT (msg_ptr);

  if(FALSE == qmi_svc_prepend_msg_hdr(&msg_ptr, cmd_buf_p->cmd_type)) 
  {
    QM_MSG_MED_1("Unable to send Response for cmd %d", cmd_buf_p->cmd_type);
    /*-----------------------------------------------------------------------
      Note - no need to free msg_ptr, taken care of in 
             qmi_voicei_prepend_msg_hdr()
    -----------------------------------------------------------------------*/
    qmi_mmode_svc_free_transaction_cmd_buf(&cmd_buf_p);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    find this outstanding transaction and save the response.
    If all responses are present, generate the response bundle and send
  -------------------------------------------------------------------------*/
  x_p = cmd_buf_p->x_p;
  ASSERT (x_p);
  
  /*-------------------------------------------------------------------------
    Queue the response (ok if freed above - need to queue regardless for 
    accounting purposes)
  -------------------------------------------------------------------------*/
  if( FALSE == qmi_mmode_svci_queue_response( &cmd_buf_p, &msg_ptr ) )
  {
    QM_MSG_ERROR("Unable to push pending response to send queue");
    ASSERT (0); // should never fail
    ASSERT (msg_ptr == NULL); // msg_ptr is freed in queue_response
    return FALSE;
  }

  ASSERT (cmd_buf_p == NULL);
  ASSERT (msg_ptr == NULL);

  /*-------------------------------------------------------------------------
    If other responses are pending, return now.  Transmission of the bundled
    response will be defered until all responses are "sent" by command hdlrs
  -------------------------------------------------------------------------*/
  for( i = 0; i < x_p->n_cmds; i++ )
  {
    if( QMI_SVC_RESPONSE_PENDING == x_p->resp_list[i] )
    {
      QM_MSG_MED("qmi_svc response pending");
      return TRUE;
    }
  }

  /*-------------------------------------------------------------------------
    If all responses are present, generate the bundle by appending all 
    responses and send the whole out
    dequeue off bundle_resp_q and verify that cmd buffers are correct
  -------------------------------------------------------------------------*/
  for( i = 0; i < x_p->n_cmds; i++ )
  {
    ASSERT( x_p->resp_list[i] != QMI_SVC_RESPONSE_PENDING );
    if( x_p->resp_list[i] != QMI_SVC_RESPONSE_NONE )
    {
        dsm_append( &msg_ptr, &(x_p->resp_list[i]) );
    }
  }

  if (msg_ptr)
  {

    msg_hdr.common_hdr.client_id      = common_hdr->client_id; 
    msg_hdr.common_hdr.qmi_instance   = common_hdr->qmi_instance;  
    msg_hdr.common_hdr.service        = common_hdr->service;  
    msg_hdr.common_hdr.transaction_id = x_p->x_id;

    msg_hdr.msg_ctl_flag = QMI_FLAG_MSGTYPE_RESP;
    if( x_p->n_cmds > 1 )
    {
      msg_hdr.msg_ctl_flag |= QMI_FLAG_MASK_COMPOUND;
    }
    msg_hdr.msg_len  = (uint16) dsm_length_packet(msg_ptr);
    qmi_framework_svc_send_response( &msg_hdr, msg_ptr );
  }

  /*-------------------------------------------------------------------------
    free the transaction buffer
  -------------------------------------------------------------------------*/
  qmi_mmode_svci_free_transaction( &x_p );  
  return TRUE;
} /* qmi_mmode_svc_send_response */


/*===========================================================================
  FUNCTION QMI_MMODE_SVC_SEND_INDICATION()

  DESCRIPTION
    This function is calls the QMI Framework API to send out the service
    indication to client.

  PARAMETERS
    msg_hdr_p : QMI Framework message header
    cmd_type : type of indication
    ind : dsm item
 
  RETURN VALUE
    TRUE if ind sent to Framework sucessfully
    FALSE otherwise
 
  DEPENDENCIES
    QMI Voice service must be initialized and registered with Framework
 
  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_mmode_svc_send_indication
(
  qmi_framework_common_msg_hdr_type  *common_hdr,
  uint16           cmd_type,
  dsm_item_type *  ind
)
{
  qmi_framework_msg_hdr_type  framework_msg_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(common_hdr);  

  /*-----------------------------------------------------------------------
    Fill the QMI Framework message header before sending Indication to
    Framework.
  -----------------------------------------------------------------------*/
  framework_msg_hdr.common_hdr.client_id = common_hdr->client_id;
  /* if unicast indication, qmi framework map the instance and no need to fill*/
  framework_msg_hdr.common_hdr.qmi_instance = common_hdr->qmi_instance;
  framework_msg_hdr.common_hdr.service = common_hdr->service;
  /* for indication the transaction ID can be ignored */
  framework_msg_hdr.common_hdr.transaction_id = 0;
  framework_msg_hdr.msg_ctl_flag = QMI_FLAG_MSGTYPE_IND;
  framework_msg_hdr.msg_len = (uint16) dsm_length_packet(ind);

  QM_MSG_MED_3("Sending QMI Indication to Framework:  clid:%d, tx_id:%d, ind_id:%d", framework_msg_hdr.common_hdr.client_id, framework_msg_hdr.common_hdr.transaction_id, cmd_type);

  if(FALSE == qmi_svc_prepend_msg_hdr(&ind, cmd_type)) 
  {
    QM_MSG_MED_1("Unable to send indication for cmd %d", cmd_type);
    return FALSE;
  }

  if(TRUE != qmi_framework_svc_send_ind(&framework_msg_hdr, ind))  
  {
    QM_MSG_ERROR("Unable to send QMI indication to Framework");
    return FALSE;
  }
  return TRUE;
} /* qmi_mmode_svc_send_indication */


/*===========================================================================
  FUNCTION QMI_MMODE_SVC_INITIALIZE_COMMON_CLIENT()

  DESCRIPTION
    Initialize the new client state for the given common client state ptr.
    
  PARAMETERS
    cl_sp :  client state pointer
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_mmode_svc_initialize_common_client
(
  qmi_common_client_state_type * cmn_cl_sp,
  void *                         svc_sp
)
{
  int i;
  qmi_transaction_type *  x_p;
  qmi_cmd_buf_type *      cmd_buf_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(cmn_cl_sp);
  memset( cmn_cl_sp , 0, sizeof(qmi_common_client_state_type) );

  cmn_cl_sp->clid  = QMI_SVC_CLID_UNUSED;

  /*-----------------------------------------------------------------------
    initialize the transaction buffers and place in free q.
    client state pointer must always point to the client state pointer
    to ensure even freeing a freed transaction (as in voicei_reset_client)
    queues the transaction in the correct client state info
  -----------------------------------------------------------------------*/
  memset (&cmn_cl_sp->x_free_q, 0, sizeof(q_type)); 
  (void) q_init( &cmn_cl_sp->x_free_q );
  x_p = &cmn_cl_sp->transactions[0];
  for (i=MAX_PENDING_TRANSACTIONS; i; i--, x_p++ )
  {
    x_p->svc_sp = svc_sp; 
    x_p->cl_sp = cmn_cl_sp;
    (void) q_link( x_p, &x_p->link );
    q_put( &cmn_cl_sp->x_free_q, &x_p->link );
  }

  /* initialize the command buffers and and place in free q */
  memset (&cmn_cl_sp->cmd_free_q, 0, sizeof(q_type));
  (void) q_init( &cmn_cl_sp->cmd_free_q );
  cmd_buf_p = &cmn_cl_sp->cmd_buf[0];
  for (i=MAX_PENDING_COMMANDS; i; i--, cmd_buf_p++ )
  {
    (void) q_link( cmd_buf_p, &cmd_buf_p->link );
    q_put( &cmn_cl_sp->cmd_free_q, &cmd_buf_p->link );
  }

} /* qmi_mmode_svc_initialize_common_client() */


/*===========================================================================
  FUNCTION QMI_MMODE_SVC_RESET_COMMON_CLIENT()

  DESCRIPTION
    Resets the state for the given client
    
  PARAMETERS
    cl_sp :  client state pointer
    
  RETURN VALUE
    ptr to response
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_mmode_svc_reset_common_client
(
  qmi_common_client_state_type * cl_sp
)
{
  int i;
  qmi_transaction_type *  x_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cl_sp);

  /*-------------------------------------------------------------------------
    q_init() is used here as a way to clear the queue and not as a way to 
    initialize the queue.
  -------------------------------------------------------------------------*/ 
  (void) q_init( &cl_sp->x_free_q );
  for( i = 0; i < MAX_PENDING_TRANSACTIONS; i++ )
  {
    x_p = &cl_sp->transactions[i];
    qmi_mmode_svci_free_transaction(&x_p );
  }
} /* qmi_svc_common_reset_client() */


/*===========================================================================
  FUNCTION QMI_MMODE_SVC_FREE_TRANSACTION_CMD_BUF()

  DESCRIPTION
    Frees a previously allocated QMI MMODE SVC transaction command buffer

  PARAMETERS
    cmd_buf_handle : Handle to command buf pointer which points to the cmd
                     buffer to be freed.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI service must be initialized and registered with Framework
 
  SIDE EFFECTS
    Removes reference to this command buf from the associated transaction
    structure.  
===========================================================================*/
void qmi_mmode_svc_free_transaction_cmd_buf
(
  qmi_cmd_buf_type **  cmd_buf_handle
)
{
  qmi_common_client_state_type *  cl_sp;
  qmi_cmd_buf_type *            freeing_cmd_buf_p;
  q_type *                      q_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cmd_buf_handle);
  /* get the pointer locally so after we modify the x_p below we don't 
  dereference the null x_p->cmd_list[0] */
  freeing_cmd_buf_p = *cmd_buf_handle; 
  *cmd_buf_handle = NULL; // free the callers reference immediately

  QM_MSG_MED_1("Freeing QMI command buffer %x", freeing_cmd_buf_p);

  if (freeing_cmd_buf_p)
  {
    ASSERT (freeing_cmd_buf_p->x_p);
    cl_sp = freeing_cmd_buf_p->x_p->cl_sp;

    /* remove reference to this command buffer from transaction */
    ASSERT(freeing_cmd_buf_p->cmd_no < MAX_MSGS_IN_BUNDLE);
    QM_MSG_MED_3("About to clear reference to cmd_buf_p (%x) from x_p(%x:%d)", freeing_cmd_buf_p, freeing_cmd_buf_p->x_p, freeing_cmd_buf_p->cmd_no);
    ASSERT(freeing_cmd_buf_p);
    ASSERT(freeing_cmd_buf_p->x_p);

    /* clear the reference to this cmd buffer in transaction to handle
       the case where the response hasn't yet been queued (this would
       also clear the reference) */
    freeing_cmd_buf_p->x_p->cmd_list[freeing_cmd_buf_p->cmd_no] = NULL;

    /* if cmd buffer is freed before response is generated, need to update
       the containing transaction accounting to ensure response is not 
       withheld on account of this freed response */
       
    QM_MSG_MED_3("Check if need to clear pending response (%x) from x_p(%x:%d)", freeing_cmd_buf_p, freeing_cmd_buf_p->x_p, freeing_cmd_buf_p->cmd_no);

    if( freeing_cmd_buf_p->x_p->resp_list[freeing_cmd_buf_p->cmd_no] 
        == QMI_SVC_RESPONSE_PENDING )
    {
      /* indicate that no response is forthcoming for this command */
      freeing_cmd_buf_p->x_p->resp_list[freeing_cmd_buf_p->cmd_no] = 
        (dsm_item_type *) QMI_SVC_RESPONSE_NONE;
    }

    memset( freeing_cmd_buf_p, 0, sizeof (qmi_cmd_buf_type) );
    (void) q_link( freeing_cmd_buf_p, &(freeing_cmd_buf_p->link) );
    if (cl_sp)
    {
      q_ptr = &cl_sp->cmd_free_q;
      q_put( q_ptr, &(freeing_cmd_buf_p->link) );
    }
    else
    {
      QM_MSG_MED("Freeing cmd_buf for global transaction");
    }
  }
  else
  {
    QM_MSG_MED("Attempt to free NULL command buffer!");
  }
} /* qmi_mmode_svc_free_transaction_cmd_buf() */


/*===========================================================================
FUNCTION QMI_MMODE_SVC_INIT()

DESCRIPTION
  This function initializes various qmi services in qmi mmode task.
 
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_mmode_svc_init
(
  void
)
{
  /*-------------------------------------------------------------------------
    Initialize QM-Modules here
  -------------------------------------------------------------------------*/
  qm_nv_init();
  qm_cbpq_init();
  qm_si_init();
  qm_nas_clnt_init();
  qm_meas_init();
  qm_mstats_init();
  qm_timer_init();
  qm_efs_init();

  /* Initialize qmi services here */
  qmi_voice_init();
  qmi_nas_init();

}/* qmi_mmode_svc_init */

/*===========================================================================
  FUNCTION QMI_MMODE_IDL_MESSAGE_DECODE()

  DESCRIPTION
    Decode QMI NAS message into c structure from incoming dsm item

  PARAMETERS
    svc         The QMI IDL service object.
    message_id  QMI message id
    msg_ptr     Pointer to dsm item that contains TLVs 
    p_req       Pointer to C structure to contain decoded message
    p_req_size  Size of C structure to contain decoded data

  RETURN VALUE
    QMI_ERR_NONE_V01 is returned if decode is successful, otherwise relevant error is returned

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_error_type_v01 qmi_mmode_idl_message_decode
 ( 
   qmi_idl_service_object_type svc, 
   uint16_t message_id, 
   dsm_item_type **msg_ptr, 
   void *p_req, 
   uint32_t p_req_size
 )
 {
   qmi_error_type_v01 errval = QMI_ERR_NONE_V01;
   int32_t            retval = QMI_IDL_LIB_NO_ERR;
   uint32             size;
   uint8              *buf = NULL;
  
 
   if( msg_ptr != NULL && *msg_ptr != NULL ) // We reutrn QMI_ERR_NONE_V01 if msg_ptr is NULL. The check() function is going to verify the malformed message in case there is no TLVs present.
   {
     size = dsm_length_packet( *msg_ptr );
     QM_MSG_MED_1("qmi_mmode_idl_message_decode dsm_pullup size %d", size);

     if ( size > 0 )
     {
       buf = (uint8 *)modem_mem_alloc((size_t)size, MODEM_MEM_CLIENT_QMI);
     
       if ( buf != NULL )
       {
         if ( size == dsm_pullup( msg_ptr, buf, size ) )
         {
           retval = qmi_idl_message_decode( svc,
                                            QMI_IDL_REQUEST,     // IDL message type
                                            message_id,          // QMI NAS cmd type
                                            (void*)buf,          // pointer to incoming byte stream
                                            (uint32_t)size,      // pointer to no of bytes to be read
                                            p_req,               // pointer to c structure
                                            p_req_size           // max size of request message, don't get this from IDL_ACCESSOR_MAX_LEN function
                                          );
         }
         else
         {
           retval = QMI_IDL_LIB_LENGTH_INCONSISTENCY;
           QM_MSG_ERROR_1("qmi_mmode_idl_message_decode dsm_pullup failed size %d", size);
         }
     
         modem_mem_free( (uint8*)buf, MODEM_MEM_CLIENT_QMI );
       }
       else
       {
         retval = QMI_IDL_LIB_LENGTH_INCONSISTENCY;
         QM_MSG_ERROR("qmi_mmode_idl_message_decode no memory for buffer");
       }
     }
     else
     {
       retval = qmi_idl_message_decode( svc,
                                        QMI_IDL_REQUEST,     // IDL message type
                                        message_id,          // QMI NAS cmd type
                                        (*msg_ptr)->data_ptr,   // pointer to incoming byte stream
                                        (*msg_ptr)->used,      // pointer to no of bytes to be read
                                        p_req,               // pointer to c structure
                                        p_req_size           // max size of request message, don't get this from IDL_ACCESSOR_MAX_LEN function
                                       );
     }
   
     switch( retval )
     {  
       case QMI_IDL_LIB_NO_ERR:
         errval = QMI_ERR_NONE_V01;
         break;
 
       case QMI_IDL_LIB_ARRAY_TOO_BIG:
         errval = QMI_ERR_ARG_TOO_LONG_V01;
         break;
 
       case QMI_IDL_LIB_MESSAGE_ID_NOT_FOUND:
         errval = QMI_ERR_NOT_SUPPORTED_V01;
         break;
 
       case QMI_IDL_LIB_TLV_DUPLICATED :
       case QMI_IDL_LIB_LENGTH_INCONSISTENCY:
       case QMI_IDL_LIB_MISSING_TLV:
       case QMI_IDL_LIB_UNKNOWN_MANDATORY_TLV:
         errval = QMI_ERR_MALFORMED_MSG_V01;
         break;
   
       case QMI_IDL_LIB_EXTENDED_ERR:
       case QMI_IDL_LIB_BUFFER_TOO_SMALL:
       case QMI_IDL_LIB_PARAMETER_ERROR:
       case QMI_IDL_LIB_UNRECOGNIZED_SERVICE_VERSION:
       case QMI_IDL_LIB_INCOMPATIBLE_SERVICE_VERSION:
       default:
         errval = QMI_ERR_INTERNAL_V01;
         break;
     }
 
     if ( retval != QMI_IDL_LIB_NO_ERR )
     {
       QM_MSG_ERROR_2("qmi_mmode_idl_message_decode failed %d %d", retval, errval);
     }
   }
 
   return errval;
 }

/*===========================================================================
  FUNCTION QMI_MMODE_IDL_MESSAGE_ENCODE()

  DESCRIPTION
    Encode QMI NAS message into dsm item from incoming c structure

  PARAMETERS
    svc          The QMI IDL service object.
    type         QMI IDL type ( QMI_IDL_RESPONSE or QMI_IDL_INDICATION )
    message_id   QMI message id
    p_src        Pointer to C structure containing message data
    p_src_size   Size of C structure that contains message data
    msg_ptr      Address of pointer to dsm item for storing encoded message data

  RETURN VALUE
    TRUE if success, FALSE if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_mmode_idl_message_encode
( 
  qmi_idl_service_object_type  svc, 
  qmi_idl_type_of_message_type type, 
  uint16_t                     message_id, 
  void                         *p_src, 
  uint32_t                     p_src_size, 
  dsm_item_type                **p_response 
)
{
  qmi_error_type_v01  eval        = QMI_IDL_LIB_NO_ERR;
  int32_t             eval2       = QMI_IDL_LIB_NO_ERR;
  uint32_t            rsp_buf_len = 0;
  uint8 *             rsp_buf     = NULL;
  uint32_t            size_buf    = 0;
  boolean             retval      = FALSE;

  *p_response = NULL;
 
  /*--------------------------------------------------------------------------------
    Accessor function for getting the maximum message length of a response
    message. Message length includes c struct size, size of type and size of length.
  ----------------------------------------------------------------------------------*/
  eval = qmi_idl_get_max_message_len( svc, type, message_id, &rsp_buf_len );
  QM_MSG_HIGH_1("qmi_mmode_idl_message_encode: Message being encoded is %d", message_id);
  if ( eval == QMI_IDL_LIB_NO_ERR )
  {
    rsp_buf = (uint8 *)modem_mem_alloc(rsp_buf_len, MODEM_MEM_CLIENT_QMI);

    if ( rsp_buf != NULL )
    {
      memset(rsp_buf, 0x00, rsp_buf_len);
      eval2 = qmi_idl_message_encode( svc,
                                      type,         // IDL message type
                                      message_id,   // QMI NAS cmd type
                                      p_src,        // pointer to c structure
                                      p_src_size,   // size of c structure which is to be encoded
                                      rsp_buf,      // buffer to contain encoded message data
                                      rsp_buf_len,  // size of buffer, should be maximum size of the encoded c structure
                                      &size_buf     // pointer to store length of the encoded message
                                    );

      if ( eval2 == QMI_IDL_LIB_NO_ERR )
      {
        if ( size_buf != dsm_pushdown_packed( p_response, &rsp_buf[0], size_buf, DSM_DS_SMALL_ITEM_POOL) )
        {
          QM_MSG_ERROR_1("Pushdown Failed for qmi_nas_idl_message_encode %d", size_buf);
          dsm_free_packet(p_response);
        }
        else
        {
          retval = TRUE;
        }
      }
      else
      {
        QM_MSG_ERROR_2("qmi_idl_message_encode failed eval2 = %d,  src_size = %d", eval2, p_src_size);
      }
    }
    else
    {
      QM_MSG_ERROR_1("Not enough memory to allocate for QMI_NAS_IDL_MESSAGE_ENCODE cmd_type = %d", message_id);
    }
  }
  else
  {
    QM_MSG_ERROR_1("qmi_idl_get_max_message_len failed %d", eval);
  }
  
  if ( rsp_buf != NULL ) { modem_mem_free((uint8*) rsp_buf, MODEM_MEM_CLIENT_QMI); }

  return retval;
}

