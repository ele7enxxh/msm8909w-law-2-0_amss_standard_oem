/*===========================================================================

                  D S _ Q M I _ F W _ C O M M O N . C

DESCRIPTION

 The Data Services QMI framework common header file. This file contains
 routines to support common message handling for QMI services using the
 new QMI framework.

EXTERNALIZED FUNCTIONS

Copyright (c) 2012-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_fw_common.c#1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
02/13/14    vrk    Added cmd_type to ds_qmi_fw_new_async_cmd_buf
11/28/12    svj    Fix to support null indications 
08/20/12    wd     Initial version
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stdio.h>
#include <string.h>

#include "amssassert.h"
#include "ds_Utils_DebugMsg.h"

#include "ds_qmi_fw_common.h"
#include "ds_qmi_svc.h"
#include "ps_system_heap.h"
#include "ps_utils.h"


/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/
static void ds_qmi_fw_dispatch_transaction
(
  ds_qmi_fw_svc_cfg_type            * svc_cfg,
  void                              * svc_sp, 
  qmi_framework_common_msg_hdr_type * common_hdr,
  qmi_transaction_type              * x_p
);

static boolean ds_qmi_fw_input
(
  ds_qmi_fw_svc_cfg_type            * svc_cfg, 
  void                              * svc_sp,
  qmi_framework_common_msg_hdr_type * common_hdr,
  qmi_cmd_buf_type                  * cmd_buf_p,
  dsm_item_type                    ** sdu_in
);

static boolean ds_qmi_fw_queue_response
( 
  qmi_cmd_buf_type **  cmd_buf_p,
  dsm_item_type    **  msg
);

/*===========================================================================

                       EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION DS_QMI_FW_RECV()

  DESCRIPTION
    Receives the QMUX SDU and passes each individual SVC message to
    ds_qmi_fw_input.

  PARAMETERS
    svc_cfg: service configuration info
    svc_sp : service provided state pointer (user data)
    msg_hdr: msg hdr passed in from the QMI framework
    cl_sp  : client state pointer 
    sdu_in : DSM item containing incoming msg

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_fw_recv
(
  ds_qmi_fw_svc_cfg_type            * svc_cfg,
  void                              * svc_sp,
  qmi_framework_msg_hdr_type        * msg_hdr,
  qmi_common_client_state_type      * cl_sp,
  dsm_item_type                     * sdu_in
)
{
  qmi_transaction_type *         x_p;
  qmi_cmd_buf_type *             cmd_buf_p;
  int32                          temp;
  uint16                         cmd_type;
  uint16                         sdu_len;
  dsm_item_type *                next_sdu;
  uint16                         remaining_bytes;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (svc_cfg == NULL) || (svc_sp == NULL) ||
       (cl_sp == NULL) || (msg_hdr == NULL) || (sdu_in == NULL) )
  {
    ASSERT(0);
    dsm_free_packet(&sdu_in);
    return;
  }

  /*-------------------------------------------------------------------------
    Get a transaction buffer to track all contained commands in the bundle
  -------------------------------------------------------------------------*/
  x_p = ds_qmi_fw_get_transaction(svc_sp, cl_sp);
  if (x_p == NULL)
  {
    LOG_MSG_ERROR_1("No transaction container! Discarding service %d message",
                    msg_hdr->common_hdr.service);
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
      LOG_MSG_ERROR_0("Short QMI svc header! Discarding transaction");
      dsm_free_packet( &sdu_in );
      ds_qmi_fw_free_transaction( &x_p );
      return;
    }
    cmd_type = ps_ntohs(temp);

    temp = dsm_pull16( &sdu_in );
    if (temp == -1)
    {
      LOG_MSG_ERROR_0("Short QMI svc header! Discarding transaction");
      dsm_free_packet( &sdu_in );
      ds_qmi_fw_free_transaction( &x_p );
      return;
    }
    sdu_len = ps_ntohs(temp);
    remaining_bytes -= 4;

    if (sdu_len > remaining_bytes)
    {
      LOG_MSG_ERROR_1("Short QMI svc payload! Discarding service %d message",
                      msg_hdr->common_hdr.service);
      dsm_free_packet( &sdu_in );
      ds_qmi_fw_free_transaction( &x_p );
      return;
    }
    else if (sdu_len < remaining_bytes)
    {
      /*---------------------------------------------------------------------
        save subsequent SDUs and isolate the current by trimming,
        unbundling individual commands into separate dsm chains
      ---------------------------------------------------------------------*/
      ASSERT (next_sdu == NULL);
      temp = PS_UTILS_DSM_DUP_PACKET( &next_sdu, sdu_in, sdu_len, 65535);
      if (0 == temp)
      {
        dsm_free_packet( &next_sdu ); // is this needed?
        dsm_free_packet( &sdu_in );
        ds_qmi_fw_free_transaction( &x_p );
        return;
      }
      dsm_trim_packet( &sdu_in, sdu_len );
    }

    /*-----------------------------------------------------------------------
      sdu_in now contains a single command.
      Get a QMI command buffer to hold the command
    -----------------------------------------------------------------------*/
    ASSERT (x_p);
    cmd_buf_p = ds_qmi_fw_new_cmd_buf( x_p );
    if (cmd_buf_p == NULL)
    {
      LOG_MSG_INFO1_1 ("No QMI command buffer!  Discarding service %x message",
                       svc_sp);
      dsm_free_packet(&sdu_in);
      ds_qmi_fw_free_transaction( &x_p );
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

  LOG_MSG_INFO2_3 ("Dispatch the transaction(%x) with (%d cmds) ctl= %x",
                   msg_hdr->common_hdr.transaction_id,
                   x_p->n_cmds,
                   msg_hdr->msg_ctl_flag);

  ds_qmi_fw_dispatch_transaction(svc_cfg, svc_sp, &msg_hdr->common_hdr, x_p);

} /* ds_qmi_fw_recv() */

/*===========================================================================
FUNCTION DS_QMI_FW_SEND_RESPONSE()

DESCRIPTION
  This function calls QMI Framework API to send a QMI Service response to a
  clients.

PARAMETERS 
  common_hdr: QMI framework common msg hdr
  cmd_buf_p:  command buffer
  msg_ptr :   dsm item

RETURN VALUE
  TRUE: If responses send to Framework
  FALSE: otherwise

DEPENDENCIES
  QMI service must be initialized and registered with Framework

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_qmi_fw_send_response
(
  qmi_framework_common_msg_hdr_type * common_hdr,
  qmi_cmd_buf_type                  * cmd_buf_p,
  dsm_item_type                     * msg_ptr
)
{
  qmi_transaction_type       * x_p;
  qmi_framework_msg_hdr_type   msg_hdr;
  int                          i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if ( (common_hdr == NULL) || (cmd_buf_p == NULL) ||
       (msg_ptr == NULL) || (cmd_buf_p->x_p == NULL) )
  {
    ASSERT(0);
    dsm_free_packet(&msg_ptr);
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
    return FALSE;
  }

  if (FALSE == qmi_svc_prepend_msg_hdr(&msg_ptr, cmd_buf_p->cmd_type)) 
  {
    LOG_MSG_ERROR_1("Unable to send Response for cmd %d", cmd_buf_p->cmd_type);
    dsm_free_packet(&msg_ptr);
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    find this outstanding transaction and save the response.
    If all responses are present, generate the response bundle and send
  -------------------------------------------------------------------------*/
  x_p = cmd_buf_p->x_p;
  
  /*-------------------------------------------------------------------------
    Queue the response (ok if freed above - need to queue regardless for 
    accounting purposes)
  -------------------------------------------------------------------------*/
  if( FALSE == ds_qmi_fw_queue_response( &cmd_buf_p, &msg_ptr ) )
  {
    LOG_MSG_ERROR_0("Unable to push pending response to send queue");
    dsm_free_packet(&msg_ptr);
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
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
      LOG_MSG_INFO2_0 ("qmi_response pending");
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
    msg_hdr.common_hdr = *common_hdr;

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
  ds_qmi_fw_free_transaction( &x_p );  
  return TRUE;

} /* ds_qmi_fw_send_response() */

/*===========================================================================
  FUNCTION DS_QMI_FW_SEND_IND()

  DESCRIPTION
    This function is calls the QMI Framework API to send out the QMI service
    indication to client.

  PARAMETERS
    msg_hdr_p : QMI Framework message header
    cmd_type : type of indication
    ind : dsm item
 
  RETURN VALUE
    TRUE if ind sent to Framework sucessfully
    FALSE otherwise
 
  DEPENDENCIES
    QMI service must be initialized and registered with Framework
 
  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_qmi_fw_send_ind
(
  qmi_framework_common_msg_hdr_type * common_hdr,
  uint16                              cmd_type,
  dsm_item_type                     * ind
)
{
  qmi_framework_msg_hdr_type   framework_msg_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (common_hdr == NULL) 
  {
    ASSERT(0);
    dsm_free_packet(&ind);
    return FALSE;
  }
      
  framework_msg_hdr.common_hdr   = *common_hdr;
  framework_msg_hdr.msg_ctl_flag = QMI_FLAG_MSGTYPE_IND;
  framework_msg_hdr.msg_len      = (uint16)dsm_length_packet(ind);

  LOG_MSG_INFO2_3( "Sending Indication to QMI Framework: clid:%d, tx_id:%d, ind_id:%d",
                   framework_msg_hdr.common_hdr.client_id,
                   framework_msg_hdr.common_hdr.transaction_id,
                   cmd_type );

  if (FALSE == qmi_svc_prepend_msg_hdr(&ind, cmd_type)) 
  {
    dsm_free_packet(&ind);
    return FALSE;
  }

  if (TRUE != qmi_framework_svc_send_ind(&framework_msg_hdr, ind))  
  {
    LOG_MSG_ERROR_0( "Unable to send QMI indication to QMI Framework" );
    dsm_free_packet(&ind);
    return FALSE;
  }

  return TRUE;

} /* ds_qmi_fw_send_ind() */

/*===========================================================================
  FUNCTION DS_QMI_FW_NEW_CMD_BUF()

  DESCRIPTION
    Allocate a QMI message command buffer

  PARAMETERS
    x_p : QMI Transaction that the command buf will be added to

  RETURN VALUE
    pointer to allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_cmd_buf_type * ds_qmi_fw_new_cmd_buf
(
  qmi_transaction_type *  x_p
)
{
  qmi_cmd_buf_type *     cmd_buf_p;
  q_type *               q_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (x_p == NULL)
  {
    ASSERT(0);
    return NULL;
  }

  if (x_p->n_cmds >= MAX_MSGS_IN_BUNDLE)
  {
    LOG_MSG_INFO1_0 ("Too many commands in transaction");
    return NULL;
  }

  if (x_p->cl_sp)  // cl_sp != NULL means this transaction is client specific
  {
    q_ptr = &x_p->cl_sp->cmd_free_q;
    cmd_buf_p = q_get(q_ptr);
    if (NULL == cmd_buf_p)
    {
      LOG_MSG_ERROR_0("No command buffers available!");
      return NULL;
    }
  }
  else
  {
      LOG_MSG_ERROR_0("Invalid state pointer !");
      return NULL;
  }

  cmd_buf_p->x_p    = x_p;
  cmd_buf_p->cmd_no = x_p->n_cmds;
  cmd_buf_p->in_progress = FALSE;
  x_p->cmd_list[x_p->n_cmds] = cmd_buf_p;
  x_p->resp_list[x_p->n_cmds] = QMI_SVC_RESPONSE_PENDING;
  x_p->n_cmds++;

  return cmd_buf_p;

} /* ds_qmi_fw_new_cmd_buf() */

/*===========================================================================
  FUNCTION DS_QMI_FW_FREE_CMD_BUF()

  DESCRIPTION
    Frees a previously allocated QMI message command buffer

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
void ds_qmi_fw_free_cmd_buf
(
  qmi_cmd_buf_type **  cmd_buf_handle
)
{
  qmi_common_client_state_type  * cl_sp;
  qmi_cmd_buf_type              * freeing_cmd_buf_p;
  q_type *                        q_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmd_buf_handle == NULL)
  {
    ASSERT(0);
    return;
  }

  /* get the pointer locally so after we modify the x_p below we don't 
  dereference the null x_p->cmd_list[0] */
  freeing_cmd_buf_p = *cmd_buf_handle; 
  *cmd_buf_handle = NULL; // free the callers reference immediately

  LOG_MSG_INFO3_1 ("Freeing QMI command buffer %x", freeing_cmd_buf_p);

  if (freeing_cmd_buf_p)
  {
    if (freeing_cmd_buf_p->x_p == NULL)
    {
      ASSERT(0);
      return;
    }
    cl_sp = freeing_cmd_buf_p->x_p->cl_sp;

    /* remove reference to this command buffer from transaction */
    if (freeing_cmd_buf_p->cmd_no >= MAX_MSGS_IN_BUNDLE)
    {
      ASSERT(0);
      return;
    }

    LOG_MSG_INFO3_3 ("About to clear reference to cmd_buf_p (%x) from x_p(%x:%d)",
                     freeing_cmd_buf_p,
                     freeing_cmd_buf_p->x_p,
                     freeing_cmd_buf_p->cmd_no );

    /* clear the reference to this cmd buffer in transaction to handle
       the case where the response hasn't yet been queued (this would
       also clear the reference) */
    freeing_cmd_buf_p->x_p->cmd_list[freeing_cmd_buf_p->cmd_no] = NULL;

    /* if cmd buffer is freed before response is generated, need to update
       the containing transaction accounting to ensure response is not 
       withheld on account of this freed response */
       
    LOG_MSG_INFO3_3 ("Check if need to clear pending response (%x) from x_p(%x:%d)",
                     freeing_cmd_buf_p,
                     freeing_cmd_buf_p->x_p,
                     freeing_cmd_buf_p->cmd_no );

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
      LOG_MSG_INFO2_0("Freeing cmd_buf for global transaction");
    }
  }
  else
  {
    LOG_MSG_INFO2_0 ("Attempt to free NULL command buffer!");
  }

} /* ds_qmi_fw_free_cmd_buf() */

/*===========================================================================
  FUNCTION DS_QMI_FW_GET_TRANSACTION()

  DESCRIPTION
    Get a transaction buffer for the QMI service 
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
qmi_transaction_type *  ds_qmi_fw_get_transaction
(
  void                         * svc_sp,
  qmi_common_client_state_type * cl_sp
)
{
  qmi_transaction_type *  x_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (svc_sp == NULL)
  {
    ASSERT(0);
    return NULL;
  }

  x_p = NULL;

  if (cl_sp)
  {
    x_p = q_get( &cl_sp->x_free_q );
  }
  else
  {
      LOG_MSG_ERROR_0("Invalid client state pointer!");
      return NULL;
  }

  if (x_p)
  {
    x_p->cl_sp = cl_sp;
    x_p->svc_sp =  svc_sp;
  }

  return x_p;

} /* ds_qmi_fw_get_transaction() */

/*===========================================================================
  FUNCTION DS_QMI_FW_FREE_TRANSACTION()

  DESCRIPTION
    Frees the passed transaction buffer

  PARAMETERS
    x_p :  handle to transaction pointer pointing to transaction buffer to
           be freed.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI service must be initialized and registered with Framework

  SIDE EFFECTS
    Nulls out the caller's transaction buffer pointer
===========================================================================*/
void ds_qmi_fw_free_transaction
(
  qmi_transaction_type **  x_p
)
{
  void                         * svc_sp;
  qmi_common_client_state_type * cl_sp;
  int i;
  boolean free_x;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (x_p == NULL)
  {
    ASSERT(0);
    return;
  }

  if (*x_p == NULL)
  {
    LOG_MSG_INFO2_0("Attempt to free a NULL transaction pointer!");
    return;
  }

  svc_sp = (*x_p)->svc_sp; 
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
        ds_qmi_fw_free_cmd_buf( &((*x_p)->cmd_list[i]) );
      }
      else
      {
        /*-------------------------------------------------------------------
          don't free cmd buf if it's in process (i.e. currently executing,
          called free_transaction as part of processing, e.g. reset, abort)
        -------------------------------------------------------------------*/
        free_x = FALSE;
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
    (*x_p)->cl_sp = cl_sp;
    (*x_p)->svc_sp = svc_sp;
    if (cl_sp)
    {
      q_put( &cl_sp->x_free_q, &((*x_p)->link) );
    }
    *x_p = NULL;
  }
  else
  {
    LOG_MSG_INFO2_0 ("Transaction in process, "
                     "will be freed once current cmd is completed");
  }

} /* ds_qmi_fw_free_transaction() */

/*===========================================================================
  FUNCTION DS_QMI_FW_NEW_ASYNC_CMD_BUF()

  DESCRIPTION
    Allocate a QMI asynchronous command buffer

  PARAMETERS
    cmd_buf_p : original QMI message command buffer
    cookie       : cookie to validate asynchronous command buffer
  RETURN VALUE
    pointer to allocated asynchronous command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
ds_qmi_fw_async_cmd_buf_type * ds_qmi_fw_new_async_cmd_buf
(
  qmi_cmd_buf_type * cmd_buf_p, 
  uint32             cookie
)
{
  ds_qmi_fw_async_cmd_buf_type * async_cmd_buf = NULL;

  PS_SYSTEM_HEAP_MEM_ALLOC(async_cmd_buf, sizeof(ds_qmi_fw_async_cmd_buf_type),
                           ds_qmi_fw_async_cmd_buf_type*);
  if(async_cmd_buf == NULL)
  {
    return NULL;
  }
  async_cmd_buf->cookie =  cookie;
  async_cmd_buf->cmd_buf = cmd_buf_p;
  async_cmd_buf->cmd_type = cmd_buf_p->cmd_type; 

  return async_cmd_buf;

}/* ds_qmi_fw_new_async_cmd_buf() */

/*===========================================================================

                       INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION DS_QMI_FW_DISPATCH_TRANSACTION()

  DESCRIPTION
    Dispatches all QMI commands in a passed transaction.
    This should be called after the comman handlr function has unbundled the request.

  PARAMETERS
    svc_cfg:    service configuration info
    svc_sp :    service state pointer 
    common_hdr: QMI framework common msg hdr
    x_p :       pointer to transaction containing commands to dispatch

  RETURN VALUE
    None

  DEPENDENCIES
    QMI service must be initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void ds_qmi_fw_dispatch_transaction
(
  ds_qmi_fw_svc_cfg_type            * svc_cfg,
  void                              * svc_sp, 
  qmi_framework_common_msg_hdr_type * common_hdr,
  qmi_transaction_type              * x_p
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
  for ( i = 0; i < x_p->n_cmds; i++ )
  {
    cmd_buf_p = x_p->cmd_list[i];
    /*-----------------------------------------------------------------------
      If reset command is present in the bundle, it frees the other commands
      in the bundle. Gracefully handling such freed commands by continuing through.
      -------------------------------------------------------------------------*/
    if ( (i > 0) && (cmd_buf_p == NULL) )
    {
      continue;
    }
    msg = x_p->req_list[i];
    x_p->req_list[i] = NULL;

    /*-----------------------------------------------------------------------
      Dispatch the current SDU/msg transaction
    -----------------------------------------------------------------------*/
    if( FALSE == ds_qmi_fw_input( svc_cfg, svc_sp, common_hdr, cmd_buf_p, &msg ) )
    {
      LOG_MSG_INFO2_3 ("Transaction %x Command #%d (%x) processing failed",
                       x_p, i, cmd_buf_p);
      free_t = TRUE;
     //  we should ensure the transaction doesn't stall here.
    }

    /*-----------------------------------------------------------------------
      Ensure the input SDU is freed.  Command handlers don't need to worry
      about freeing the input message.
    -----------------------------------------------------------------------*/
    dsm_free_packet ( &msg );
  }

  if (TRUE == free_t)
  {
    // this indicates that ds_qmi_fw_input() failed for one of the bundled 
    // commands in the transaction. This failure could be either problem
    // with assembling response or invalid command type was sent to the
    // service. In either case right thing to do is to cleanup transaction
    // at this point, otherwise it will never be cleaned up.
    ds_qmi_fw_free_transaction( &x_p );
  }

} /* ds_qmi_fw_dispatch_transaction() */

/*===========================================================================
  FUNCTION DS_QMI_FW_INPUT()

  DESCRIPTION
    Process the incoming QMI message and send the response back.

  PARAMETERS
    svc_cfg:      service configuration
    svc_sp :      service state pointer
    common_hdr:   QMI framework common msg hdr
    cmd_buf_p_in: command buffer pointer
    sdu_in:       message to be processed

  RETURN VALUE
    TRUE:   message processed successfully
    FALSE:  invalid message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean ds_qmi_fw_input
(
  ds_qmi_fw_svc_cfg_type            * svc_cfg, 
  void                              * svc_sp,
  qmi_framework_common_msg_hdr_type * common_hdr,
  qmi_cmd_buf_type                  * cmd_buf_p,
  dsm_item_type                    ** sdu_in
)
{
  qmi_common_client_state_type * cl_sp;
  dsm_item_type                * response_ptr = NULL;
  qmi_svc_cmd_hdlr_type        * cmd_hdlr;
  uint16                         cmd;
  boolean                        retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (svc_cfg == NULL) || (svc_sp == NULL) || (common_hdr == NULL) ||
       (cmd_buf_p == NULL) || (cmd_buf_p->x_p == NULL) || (sdu_in == NULL) )
  {
    ASSERT(0);
    return FALSE;
  }

  LOG_MSG_INFO3_2( "Got QMI Service cmd type %d (%d bytes)",
                   cmd_buf_p->cmd_type, dsm_length_packet(*sdu_in));

  cl_sp = cmd_buf_p->x_p->cl_sp;

  if (cl_sp == NULL)
  {
    ASSERT (0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Checking to see if a cmd hndler is registerd for the input command
  -------------------------------------------------------------------------*/
  cmd_hdlr = svc_cfg->cmd_hdlr_array;
  for (cmd = 0; cmd < svc_cfg->cmd_num_entries; cmd++, cmd_hdlr++)
  {
    if ((cmd_hdlr == NULL) || (cmd_buf_p->cmd_type == cmd_hdlr->cmd_type))
    {
      break; 
    }
  }

  if((cmd == svc_cfg->cmd_num_entries) || (cmd_hdlr == NULL) || (cmd_hdlr->msg_desc == NULL))
  {
    LOG_MSG_ERROR_3( "Unrecognized type (=%d) for service (=%d)! cmd_entries(=%d) Return error",
             cmd_buf_p->cmd_type, common_hdr->service, cmd );
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
      MSG_SPRINTF_1( MSG_SSID_DS, MSG_LEGACY_MED, "Handling %s", cmd_hdlr->msg_desc);
      response_ptr = cmd_hdlr->request_hdlr( svc_sp, cmd_buf_p, cl_sp, sdu_in );
    }
    else
    {
      LOG_MSG_INFO2_2 ("Unhandled (%d cmd) msg type (%d)",
                       cmd_buf_p->x_p->ctl&QMI_FLAG_MASK_MSGTYPE,
                       cmd_buf_p->cmd_type);
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
    LOG_MSG_INFO2_0 ("(Service generated no response)");
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
    return FALSE;
  }
  else if (response_ptr == QMI_SVC_RESPONSE_PENDING)
  {
    LOG_MSG_INFO2_1 ("Response pending for cmd_buf_p %p", cmd_buf_p);
    // command buffer will be freed later when response is completed
    return TRUE;
  }

  return ds_qmi_fw_send_response( common_hdr, cmd_buf_p, response_ptr );

} /* ds_qmi_fw_input() */

/*===========================================================================
  FUNCTION DS_QMI_FW_QUEUE_RESPONSE()

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
static boolean ds_qmi_fw_queue_response
( 
  qmi_cmd_buf_type **  cmd_buf_p,
  dsm_item_type    **  msg
)
{
  qmi_transaction_type *  x_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (cmd_buf_p == NULL) || (*cmd_buf_p == NULL) ||
       ((*cmd_buf_p)->x_p == NULL) || (msg == NULL) )
  {
    ASSERT(0);
    return FALSE;
  }

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
    DATA_ERR_FATAL("Command is linked to transaction it doesn't belong to! discarding");
    dsm_free_packet( msg );
    ds_qmi_fw_free_cmd_buf( cmd_buf_p );
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
  ds_qmi_fw_free_cmd_buf( cmd_buf_p );
  return TRUE;

} /* ds_qmi_fw_queue_response() */

/*===========================================================================
  FUNCTION DS_QMI_FW_COMMON_CL_INIT()

  DESCRIPTION
    Common client initialization

  PARAMETERS
    sp         :  service provided state pointer (user data)
    client_sp  :  client state pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_fw_common_cl_init
(
  void                         * svc_sp,
  qmi_common_client_state_type * cl_sp
)
{
  qmi_cmd_buf_type *            cmd_buf_p;
  qmi_transaction_type *        x_p;
  int                           i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (svc_sp);
  ASSERT (cl_sp);
  /*-----------------------------------------------------------------------
    initialize client state
    - initialize async command buffers and put them in the free queue
    - initialize the transaction buffers and place in free q.
    - client state pointer in the transaction must always point to the
      client state pointer to ensure even freeing a freed transaction 
      queues the transaction in the correct client state info
  ---------------------------------------------------------------------*/
  memset (&cl_sp->x_free_q, 0, sizeof(q_type));
  (void) q_init( &cl_sp->x_free_q );
  x_p = &cl_sp->transactions[0];
  for (i=MAX_PENDING_TRANSACTIONS; i; i--, x_p++ )
  {
    x_p->svc_sp = svc_sp; 
    x_p->cl_sp  = cl_sp;   
    (void) q_link( x_p, &x_p->link );
    q_put( &cl_sp->x_free_q, &x_p->link );
  }

  /* initialize the command buffers and and place in free q */
  memset (&cl_sp->cmd_free_q, 0, sizeof(q_type));
  (void) q_init( &cl_sp->cmd_free_q );
  cmd_buf_p = &cl_sp->cmd_buf[0];
  for (i=MAX_PENDING_COMMANDS; i; i--, cmd_buf_p++ )
  {
    (void) q_link( cmd_buf_p, &cmd_buf_p->link );
    q_put( &cl_sp->cmd_free_q, &cmd_buf_p->link );
  }
} /* ds_qmi_fw_common_cl_init() */

/*===========================================================================
  FUNCTION DS_QMI_FW_COMMON_RESET_CLIENT()

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
extern void ds_qmi_fw_common_reset_client
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
    ds_qmi_fw_free_transaction(&x_p );
  }
} /* ds_qmi_fw_common_reset_client() */

