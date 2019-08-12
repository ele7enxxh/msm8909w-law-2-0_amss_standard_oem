/*===========================================================================

                            Q M I _ F W . C

DESCRIPTION

  The QMI Services Framework interface file.
 
EXTERNALIZED FUNCTIONS
 
  qmi_framework_reg_service()
    Register a service to the Framework with a service configuration.

  qmi_framework_dereg_service()
    Deregister the service from the QMI framework.
 
  qmi_framework_svc_send_response()
    Send a response message from service to the QMI Framework. 
 
  qmi_framework_svc_send_ind()
    Send an Indication from service to the QMI Framework. 
 
  qmi_framework_alloc_msg_header()
    Allocates memory for a msg header. 
 
  qmi_framework_dealloc_msg_header()
    Deallocates the Memory for the msg header. 
 
  qmi_framework_svc_send_alloc_clid_result()
    Send a Client ID allocation result to QMI CTL service. 
 
  qmi_framework_svc_send_dealloc_clid_result()
    Send a Client ID deallocation result to QMI CTL service.
 
Copyright (c) 2012-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/src/qmi_framework.c#1 $ 
   $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/25/14    vrk     Fixed a race condition in QMUX_FRAMEWORK_REG_SERVICE
5/15/13     ab      Add QMI SVC available notification feature
09/13/12    ab      Fixed Race Condition in QMUX_FRAMEWORK_REG_SERVICE 
04/12/12    wc      Check client release before sending indication
02/09/12    wc      Add range check to qmi_framework_svc_state 
01/20/11    rk      Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "ps_byte.h"
#include "modem_mem.h"
#include "queue.h"
#include "qmi_modem_task_svc.h"
#include "qmi_modem_task_cmd.h"
#include "qmi_svc_defs.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_svc_ext.h"
#include "qmi_frameworki.h"
#include "ds_qmux.h"
#include "ds_qmi_ctl_ext.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"
#include <stringl/stringl.h>
#include "rcinit_rex.h"

#include "ps_utils.h"
#include "ds_qmux_qcsi_shim.h"
/*===========================================================================

                               DATA TYPES 

===========================================================================*/
extern rex_tcb_type  * rex_qmi_modem_tcb;

/*===========================================================================
TYPEDEF QMI_FRAMEWORK_MSG_BUF_TYPE

DESCRIPTION 
  type for QMI fw message buffer.
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_framework_msg_hdr_type   msg_hdr;
  dsm_item_type *       dsm_item;
}qmi_frameworki_msg_buf_type;

/*---------------------------------------------------------------------------
  Type used to keep track of framework service state for all services
---------------------------------------------------------------------------*/
static qmi_framework_svc_info_type *   qmi_framework_svc_state[QMUX_SERVICE_MAX]; 
static boolean qmi_framework_powerup_inited = FALSE;

/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/
static qmi_frameworki_msg_buf_type *  qmi_frameworki_alloc_msg_buf
(
  void
);

static void qmi_frameworki_dealloc_msg_buf
(
  qmi_frameworki_msg_buf_type **   msg_buf
);

static boolean qmi_frameworki_svc_send
(
  qmi_framework_msg_hdr_type *  msg_hdr,
  uint16                        qmi_cmd,
  dsm_item_type *               msg_ptr
);

static boolean qmi_frameworki_qmux_send
(
  qmi_frameworki_msg_buf_type * msg_buf,
  uint8                         msg_type, 
  uint16                        ind_cmd_id, 
  dsm_item_type *               msg_ptr
);

static boolean qmi_frameworki_validate_msg_hdr
(
  qmi_framework_msg_hdr_type *  msg_hdr,
  uint8                         msg_type
);

static void qmi_frameworki_send_client_reg_result
(
  qmi_result_e_type                     reg_result,
  qmi_error_e_type                      errval,
  qmi_framework_common_msg_hdr_type *   common_msg_hdr,
  uint16                                qmi_cmd
);
/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_FRAMEWORKI_ALLOC_MSG_BUF()

DESCRIPTION
  Allocates a memory for framework message buffer.

PARAMETERS

RETURN VALUE
  msg_buf* - ptr to qmi_frameworki_msg_buf_type.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static qmi_frameworki_msg_buf_type *   qmi_frameworki_alloc_msg_buf
(
  void
)
{
  qmi_frameworki_msg_buf_type *  msg_buf;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  msg_buf = (qmi_frameworki_msg_buf_type *) modem_mem_alloc( sizeof(qmi_frameworki_msg_buf_type),
                                                         MODEM_MEM_CLIENT_QMI_CRIT);
  if (msg_buf == NULL)
  {
    LOG_MSG_ERROR_0 ("Alloc alloc_msg_buf: Can't get memory allocation!");
    ASSERT(msg_buf);
  }
  return msg_buf;
} /* qmi_frameworki_alloc_msg_buf() */

/*===========================================================================
FUNCTION QMI_FRAMEWORKI_DEALLOC_MSG_BUF()

DESCRIPTION
  De-Allocates the framework msg buffer memory.

PARAMETERS
  qmi_frameworki_msg_buf_type* - ptr to qmi_frameworki_msg_buf_type.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void qmi_frameworki_dealloc_msg_buf
(
  qmi_frameworki_msg_buf_type **       msg_buf
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(msg_buf == NULL)
  {
    LOG_MSG_ERROR_0("NULL pointer passed to qmi_frameworki_dealloc_msg_buf");
    return;
  }
  modem_mem_free( *msg_buf,
                  MODEM_MEM_CLIENT_QMI_CRIT);

  *msg_buf = NULL;
  return;

} /* qmi_frameworki_dealloc_msg_buf() */

/*===========================================================================
FUNCTION QMI_FRAMEWORKI_SVC_SEND()

DESCRIPTION
  Consolidate the QMI Service response or indication then send to
  Framework.

PARAMETERS
  msg_hdr   : msg hdr received from QMI Service
  qmi_cmd   : QMI cmd(response or indication)
  msg_ptr   : response to send out

RETURN VALUE
  TRUE if command was sent (or is queued in transaction for later send)
  FALSE if there was an error.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean qmi_frameworki_svc_send
(
  qmi_framework_msg_hdr_type *  msg_hdr,
  uint16                        qmi_cmd,
  dsm_item_type *               msg_ptr
)
{
  qmi_frameworki_msg_buf_type *   qmi_framework_msg_buf;
  qmi_cmd_data_buf_type *  cmd_ptr;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT (msg_hdr);
  ASSERT (msg_ptr);
  
  /*-----------------------------------------------------------------------
    Preparing the fw service msg buffer(payload) to send qmi modem task and it
    be processed and send to qmux.
  -----------------------------------------------------------------------*/
  qmi_framework_msg_buf =  qmi_frameworki_alloc_msg_buf();
  if (qmi_framework_msg_buf == NULL)
  {
    LOG_MSG_ERROR_1 ("No msg_buffer!  Discarding service %x message",
                     msg_hdr->common_hdr.service);
    dsm_free_packet(&msg_ptr);
    return FALSE;
  }

  LOG_MSG_INFO2_5 ("Sending svc %d msg to QMUX: "
                   "clid: %d, tx_id: %d, msg_ctl: %d, msg_len: %d",
                   msg_hdr->common_hdr.service,
                   msg_hdr->common_hdr.client_id,
                   msg_hdr->common_hdr.transaction_id,
                   msg_hdr->msg_ctl_flag,
                   msg_hdr->msg_len);

  /*-----------------------------------------------------------------------
    Copy the payload and msg header. 
  -----------------------------------------------------------------------*/
  memscpy( &qmi_framework_msg_buf->msg_hdr,
           sizeof (*msg_hdr), 
          msg_hdr, 
          sizeof (*msg_hdr) );

  qmi_framework_msg_buf->dsm_item = msg_ptr;

  /*-----------------------------------------------------------------------
    Need a task switch? or directly process the Service message.
    if the message come from a different task, then we need a task switch
    otherwise process the direct response.
  -----------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  if( rex_self() != rex_qmi_modem_tcb )
#else
  if( rex_self() != &qmi_modem_tcb )
#endif
  {
    cmd_ptr = qmi_get_cmd_data_buf();
    if (cmd_ptr == NULL)
    {
      LOG_MSG_ERROR_1 ("No command buffer!  Discarding service %x message",
                       msg_hdr->common_hdr.service);
      dsm_free_packet(&msg_ptr);
      qmi_frameworki_dealloc_msg_buf(&qmi_framework_msg_buf);
      ASSERT(0);
      return FALSE;
    }
    cmd_ptr->cmd.qmi.id = qmi_cmd; 
    cmd_ptr->cmd.qmi.data.fw.user_data_ptr = qmi_framework_msg_buf;
    qmi_send_cmd(QMI_CMD, cmd_ptr);
  }
  else
  {
    LOG_MSG_INFO2_1 ("Sending direct service %d message to QMUX layer",
                     msg_hdr->common_hdr.service );
    return qmi_framework_process_svc_send((void *)qmi_framework_msg_buf, qmi_cmd );
  }
  return TRUE;
} /* qmi_frameworki_svc_send() */

/*===========================================================================
FUNCTION QMI_FRAMEWORKI_QMUX_SEND()

DESCRIPTION
  Send the service message to the QMUX layer

PARAMETERS
  msg_hdr   :msg hdr sent from svc
  msg_type  : message type, whether it is a cmd, response or indication
  int_cmd_id  : cmd id for indication
  msg_ptr   : response to send out

RETURN VALUE
  TRUE if command was sent (or is queued in transaction for later send)
  FALSE if there was an error.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean qmi_frameworki_qmux_send
(
  qmi_frameworki_msg_buf_type *   msg_buf,
  uint8                           msg_type, 
  uint16                          int_cmd_id, 
  dsm_item_type *                 msg_ptr
)
{
  qmi_svc_hdr_type        svc_hdr;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(msg_ptr);
  ASSERT(msg_buf);
  
  /*-------------------------------------------------------------------------
    Perform filtering on messages based on power save mode
  -------------------------------------------------------------------------*/
  if ( qmi_svc_filter_message( (qmi_instance_e_type) msg_buf->msg_hdr.common_hdr.qmi_instance,
                               msg_buf->msg_hdr.common_hdr.service,
                               msg_type,
                               int_cmd_id) )
  {
    LOG_MSG_INFO2_3("Filtered QMI message: type=%d svc=%d ind msg id=%d",
                    msg_type, msg_buf->msg_hdr.common_hdr.service, int_cmd_id);
    
    /*-------------------------------------------------------------------------
    Dropping filtered message; clean buffer 
    -------------------------------------------------------------------------*/
    dsm_free_packet( &msg_ptr ); 
    qmi_frameworki_dealloc_msg_buf(&msg_buf);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    build service header
  -------------------------------------------------------------------------*/
  if (msg_ptr)
  {
    svc_hdr.ctl_flags = msg_buf->msg_hdr.msg_ctl_flag;
    
    (void) hput16( svc_hdr.transaction_id, 
                           msg_buf->msg_hdr.common_hdr.transaction_id );
    
    if ( sizeof(svc_hdr) != PS_UTILS_DSM_PUSHDOWN_PACKED( &msg_ptr,
                                       &svc_hdr,
                                       sizeof (svc_hdr),
                                       DSM_DS_SMALL_ITEM_POOL) )
    {
      dsm_free_packet (&msg_ptr);
      qmi_frameworki_dealloc_msg_buf(&msg_buf);
      return FALSE;
    }
    /*---------------------------------------------------------------------
     send the transaction out via QMUX
    ---------------------------------------------------------------------*/
    else
    {
      LOG_MSG_INFO2_3 ("Send QMI service: %d tx_id: %d via QMUX - SIO to client: %d",
                       msg_buf->msg_hdr.common_hdr.service,
                       msg_buf->msg_hdr.common_hdr.transaction_id,
                       msg_buf->msg_hdr.common_hdr.client_id);
      
      qmux_sio_send ( (qmi_instance_e_type)msg_buf->msg_hdr.common_hdr.qmi_instance, 
                      msg_buf->msg_hdr.common_hdr.service, 
                      msg_buf->msg_hdr.common_hdr.client_id, 
                      msg_ptr);

      /*---------------------------------------------------------------------
       free the cmd and msg ptrs
      ---------------------------------------------------------------------*/
      msg_ptr = NULL;
      qmi_frameworki_dealloc_msg_buf(&msg_buf);
    }
  }
  return TRUE;

} /* qmi_frameworki_qmux_send() */

/*===========================================================================
FUNCTION QMI_FRAMEWORKI_VALIDATE_MSG_HDR()

DESCRIPTION
  Validates the QMI Framework message header sent by the service.
  
PARAMETERS
  msg_hdr - message header
  msg_type - indicates resp or indication

RETURN VALUE
  TRUE : if the valid header.
  FALSE : otherwise.

DEPENDENCIES
  None. 
 
SIDE EFFECTS
  None.
===========================================================================*/
static boolean qmi_frameworki_validate_msg_hdr
(
  qmi_framework_msg_hdr_type *    msg_hdr,
  uint8                           msg_type
)
{
  qmi_framework_svc_info_type *  svc_info;
/*-----------------------------------------------------------------*/
  
  ASSERT (msg_hdr);
  ASSERT (msg_hdr->common_hdr.qmi_instance < QMI_INSTANCE_MAX   );
  ASSERT (msg_hdr->common_hdr.service      < QMUX_SERVICE_MAX);   
  
  /*---------------------------------------------------------------------
   Validate message is an indication for broadcast client id.
  ---------------------------------------------------------------------*/
  if (msg_hdr->common_hdr.client_id == QMI_SVC_CLID_BROADCAST)
  {
    if (msg_type != QMI_FLAG_MSGTYPE_IND)
    {
      LOG_MSG_ERROR_2 ("Got QMI service %x message for broadcast client msg type %d! discarding",
                       msg_hdr->common_hdr.service, msg_type);
      return FALSE;
    }
    return TRUE;
  }
  
  svc_info = qmi_framework_svc_state[msg_hdr->common_hdr.service];
  if (svc_info == NULL)
  {
    LOG_MSG_ERROR_1("Unknown service [%d]", msg_hdr->common_hdr.service);
    return FALSE;
  }

  /*---------------------------------------------------------------------  
    verify client id is valid
  ---------------------------------------------------------------------*/
  if ( (msg_hdr->common_hdr.client_id == 0) || 
       (msg_hdr->common_hdr.client_id >= QMI_FRAMEWORK_SVC_MAX_CLIENTS ) ||
       (svc_info->client_instance_map[msg_hdr->common_hdr.client_id -1] != 
                    (qmi_instance_e_type)msg_hdr->common_hdr.qmi_instance))
  {
    LOG_MSG_ERROR_3 ("Got QMI service %x message for unassigned client %d instance %d! discarding",
                     msg_hdr->common_hdr.service,
                     msg_hdr->common_hdr.client_id,
                     msg_hdr->common_hdr.qmi_instance);
    return FALSE;
  }

  return TRUE;

} /* qmi_frameworki_validate_msg_hdr() */

/*===========================================================================
FUNCTION QMI_FRAMEWORKI_SEND_CLIENT_REG_RESULT()

DESCRIPTION
  Consolidate the clid alloc/dealloc result then send to Framework.

PARAMETERS
  reg_result   : Client ID alloc/dealloc result.
  common_hdr   : common msg hdr type.
  qmi_cmd      : QMI cmd type(Alloc or dealloc)

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void qmi_frameworki_send_client_reg_result
(
  qmi_result_e_type                     reg_result,
  qmi_error_e_type                      errval,
  qmi_framework_common_msg_hdr_type *   common_msg_hdr,
  uint16                                qmi_cmd
)
{
  qmi_framework_svc_info_type *   svc_info;
  qmi_cmd_data_buf_type *  cmd_ptr;
/*-------------------------------------------------------------------------*/

  if (common_msg_hdr == NULL || common_msg_hdr->service >= QMUX_SERVICE_MAX)
  {
    LOG_MSG_ERROR_0("Invalid service or common_msg_hdr!");
    return;
  }

  svc_info = qmi_framework_svc_state[common_msg_hdr->service];
  if (svc_info == NULL)
  {
    LOG_MSG_ERROR_1("Unknown service [%d]", common_msg_hdr->service);
    return;
  }
  
  cmd_ptr = qmi_get_cmd_data_buf();
  if (cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_2 ("No cmd buffer! service %d client ID:%d response message ",
                     common_msg_hdr->service, common_msg_hdr->client_id);
    ASSERT(0);
    return;
  }
  /*-------------------------------------------------------------------------
    Sending client ID response 
  -------------------------------------------------------------------------*/
  cmd_ptr->cmd.qmi.id = qmi_cmd;
  cmd_ptr->cmd.qmi.data.fw_cl_resp.result = reg_result;
  cmd_ptr->cmd.qmi.data.fw_cl_resp.cl_reg_info.service_id 
                                        = common_msg_hdr->service;
  cmd_ptr->cmd.qmi.data.fw_cl_resp.cl_reg_info.inst_id  
                                        = (qmi_instance_e_type)common_msg_hdr->qmi_instance;
  cmd_ptr->cmd.qmi.data.fw_cl_resp.cl_reg_info.cl_id    
                                        = common_msg_hdr->client_id;
  cmd_ptr->cmd.qmi.data.fw_cl_resp.cl_reg_info.tx_id    
                                        = common_msg_hdr->transaction_id;
  cmd_ptr->cmd.qmi.data.fw_cl_resp.errval = errval;
  
  qmi_send_cmd(QMI_CMD, cmd_ptr);

} /* qmi_frameworki_send_client_reg_result() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_RECV()

DESCRIPTION
  This function passes the QMUX SDU to QMI Service cmd handler. The SVC
  cmd handler should be registered during the service registration. 
  
PARAMETERS
  qmi_instance  : qmi instance.
  service       : Individual service to the coresponding message.
  client_id     : Client ID corresponding the message. 
  sdu_in        : DSM item containing incoming msg.
  
RETURN VALUE
  TRUE  : Received QMUX SDU send to SVC hdlr successful.
  FALSE : if there was an error.

DEPENDENCIES
  QMI service must be registered previously to recive QMUX SDU. 

SIDE EFFECTS
  None.
===========================================================================*/
boolean qmi_framework_svc_recv
( 
  qmi_instance_e_type     qmi_instance,
  qmux_service_e_type     service,
  uint8                   client_id, 
  dsm_item_type *         sdu_in
)
{
  uint16                    msg_x_id;
  uint8                     msg_ctl;
  int32                     temp;
  uint16                    remaining_bytes;
  qmi_framework_msg_hdr_type       msg_hdr;
  qmi_framework_svc_info_type *    svc_info;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT (sdu_in);
  ASSERT (qmi_instance < QMI_INSTANCE_MAX   );
  ASSERT (service      < QMUX_SERVICE_MAX);

  /*-----------------------------------------------------------------------
    Validate the client id.
  -----------------------------------------------------------------------*/
  if ((client_id == QMI_SVC_CLID_UNUSED ) ||
      (client_id >= QMI_FRAMEWORK_SVC_MAX_CLIENTS ))
  {
    LOG_MSG_ERROR_2 ("Got QMI service %x message for unassigned client %d! discarding",
                     service, client_id);
    dsm_free_packet (&sdu_in);
    return FALSE;
  }

  svc_info = qmi_framework_svc_state[service];
  if (svc_info == NULL)
  {
    LOG_MSG_ERROR_1("Unknown service [%d]", service);
    dsm_free_packet (&sdu_in);
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Validate the instance map.
  -----------------------------------------------------------------------*/
  if ( svc_info->client_instance_map[client_id -1] != qmi_instance)
  {
    LOG_MSG_ERROR_2 ("Got QMI service %x message for invaid instance: %d",
                     service, qmi_instance);
    dsm_free_packet (&sdu_in);
    return FALSE;
  }
  
  /*-------------------------------------------------------------------------
    Extract Service header - ctl flags and transaction ID apply to 
    all SDUs contained in the message.
  -------------------------------------------------------------------------*/
  temp = dsm_pull8(&sdu_in);
  if (temp == -1)
  {
    dsm_free_packet( &sdu_in ); 
    return FALSE;
  }
  msg_ctl  = (uint8) temp;
  temp = dsm_pull16(&sdu_in);
  if (temp == -1)
  {
    dsm_free_packet( &sdu_in ); 
    return FALSE;
  }
  msg_x_id = ps_ntohs(temp);
  LOG_MSG_INFO3_2 ("QMI service %x receives QMI service request(transaction id = %x, ctl = %x)",
                   msg_x_id, msg_ctl);

  /*-------------------------------------------------------------------------                  
    Parse the incoming service message
  -------------------------------------------------------------------------*/
  remaining_bytes = (uint16) dsm_length_packet(sdu_in);
  
  /*-----------------------------------------------------------------------
    Preparing service msg header and send to QMI service command hdlr.
  -----------------------------------------------------------------------*/
  msg_hdr.common_hdr.service = service;
  msg_hdr.common_hdr.client_id = client_id;
  msg_hdr.common_hdr.qmi_instance = (int32)qmi_instance;
  msg_hdr.common_hdr.transaction_id = msg_x_id;
  msg_hdr.msg_ctl_flag =  msg_ctl;
  msg_hdr.msg_len = remaining_bytes;

  /*-----------------------------------------------------------------------
    calling QMI Service command hdlr callback function..
  -----------------------------------------------------------------------*/
  if(svc_info->cfg.cbs.cmd_hdlr)
  {
    LOG_MSG_INFO2_3 ("Calling QMI Service:%d cmd hdlr. clid:%d, tx_id:%d",
                     msg_hdr.common_hdr.service,
                     msg_hdr.common_hdr.client_id,
                     msg_hdr.common_hdr.transaction_id);
    svc_info->cfg.cbs.cmd_hdlr(&msg_hdr,&sdu_in);
  }
  else
  {
    LOG_MSG_ERROR_3 ("Invalid QMI service:%d cmd hdlr. Request is dropped. clid:%d, tx_id:%d",
                     msg_hdr.common_hdr.service,
                     msg_hdr.common_hdr.client_id,
                     msg_hdr.common_hdr.transaction_id);
    dsm_free_packet (&sdu_in);
    return FALSE;
  }
  return TRUE;

} /* qmi_framework_svc_recv() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_PROCESS_SVC_SEND()

DESCRIPTION
  This function process the QMI service response or indication and
  then send to QNUX layer.

PARAMETERS
  user_data_ptr : user data
  qmi_cmd       : QMI cmd for Response or Indication.

RETURN VALUE
  TRUE if command was sent.
  FALSE if there was an error.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean qmi_framework_process_svc_send
(
  void *        user_data_ptr,
  uint16        qmi_cmd
)
{
  qmi_frameworki_msg_buf_type *      msg_buf;
  dsm_item_type *             msg_ptr;
  qmi_framework_msg_hdr_type *       msg_hdr;
  uint8                       msg_type;
  qmi_framework_svc_info_type *      svc_info;
  int32                              temp;
  uint16                             ind_cmd_id = 0;
  boolean                            result = FALSE;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(user_data_ptr);

  /*-----------------------------------------------------------------------
    Extract the msg header and sdu from incoming payload. 
  -----------------------------------------------------------------------*/
  msg_buf  = (qmi_frameworki_msg_buf_type*)(user_data_ptr);
  msg_hdr     = &msg_buf->msg_hdr;
  msg_ptr     = msg_buf->dsm_item;

  ASSERT(msg_hdr);
  ASSERT(msg_ptr);
  ASSERT(msg_hdr->common_hdr.service      < QMUX_SERVICE_MAX);   

  svc_info = qmi_framework_svc_state[msg_hdr->common_hdr.service];
  if (svc_info == NULL)
  {
    LOG_MSG_ERROR_1("Unknown service [%d]", msg_hdr->common_hdr.service);
    goto func_end;
  }

  if((qmi_cmd_id_e_type)qmi_cmd == QMI_CMD_FRAMEWORK_SEND_RESPONSE )
  {
    msg_type = QMI_FLAG_MSGTYPE_RESP;
  }
  else if((qmi_cmd_id_e_type)qmi_cmd == QMI_CMD_FRAMEWORK_SEND_IND)
  {
    msg_type = QMI_FLAG_MSGTYPE_IND;

    /*-----------------------------------------------------------------------
      Framework would fill up the instance field in the msg_hdr if its not set
      during the unicast indication to a clid. 
    -----------------------------------------------------------------------*/
    if((msg_hdr->common_hdr.client_id > QMI_SVC_CLID_UNUSED) &&
       (msg_hdr->common_hdr.client_id < QMI_FRAMEWORK_SVC_MAX_CLIENTS ) &&
       (svc_info->client_instance_map[msg_hdr->common_hdr.client_id -1] != 
                    (qmi_instance_e_type)msg_hdr->common_hdr.qmi_instance))
    {
      if (svc_info->client_instance_map[msg_hdr->common_hdr.client_id -1] == QMI_INSTANCE_MAX)
      {
        LOG_MSG_INFO2_1("Client %d has been released, drop indication",
                        msg_hdr->common_hdr.client_id);
        goto func_end;
      }

      msg_hdr->common_hdr.qmi_instance = 
          svc_info->client_instance_map[msg_hdr->common_hdr.client_id -1];
    }
    /*-----------------------------------------------------------------------
      Finding the command type for filtering the Indication
    -----------------------------------------------------------------------*/
    temp = dsm_pull16( &msg_ptr ); 
    if (temp == -1)
    {
      LOG_MSG_ERROR_0 ("Couldn't extract the ind_cmd_id %d from msg_ptr");
      goto func_end;
    }

    ind_cmd_id = ps_ntohs(temp);

    /*-----------------------------------------------------------------------
      prepend the command type back to msg_ptr
    -----------------------------------------------------------------------*/
    if( (QMI_SVC_PKT_PUSH(&msg_ptr, &ind_cmd_id, sizeof(uint16))) == FALSE)
    {
      LOG_MSG_ERROR_0 ("Couldn't append the ind_cmd_id to msg_ptr");
      goto func_end;
    }
  }
  else
  {
    LOG_MSG_ERROR_1 ("Invalid QMI svc cmd to process cmd %d", qmi_cmd);
    goto func_end;
  }

  /*-----------------------------------------------------------------------
    validate the message header 
  -----------------------------------------------------------------------*/   
  if (FALSE == qmi_frameworki_validate_msg_hdr(msg_hdr, msg_type ))
  {
    LOG_MSG_ERROR_1 ("Msg header validation failed - unable to send Response for cmd %d",
                     msg_hdr->msg_ctl_flag);

    goto func_end;
  }

  if (msg_buf->msg_hdr.common_hdr.qmi_instance == QMI_INSTANCE_SHIM)
  {
    result = qmux_qcsi_shim_send_resp_ind(msg_buf->msg_hdr.common_hdr, 
                                          msg_ptr, 
                                        msg_type);
    qmi_frameworki_dealloc_msg_buf(&msg_buf);
    return result;   
  }
  else
  {
    return qmi_frameworki_qmux_send(msg_buf, msg_type, ind_cmd_id, msg_ptr);
  }

func_end:
    dsm_free_packet(&msg_ptr);
    qmi_frameworki_dealloc_msg_buf(&msg_buf);
    return FALSE;
} /* qmi_framework_process_svc_send() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_PROCESS_CLIENT_REGISTRATION_RESULT()

DESCRIPTION
  This function process the external QMI service client registration result.

PARAMETERS
  result            : Client ID allocation result.
  client_reg_result : Client reg result.
  qmi_cmd           : reg/dereg.
  errval            : err value

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_framework_process_client_registration_result
(
  qmi_result_e_type                 reg_result,
  qmi_ctl_client_reg_result_type *  client_reg_result,
  uint16                            qmi_cmd,
  qmi_error_e_type                  errval
)
{
  qmi_framework_svc_info_type *   svc_info;

  ASSERT(client_reg_result->service_id < QMUX_SERVICE_MAX);

  svc_info = qmi_framework_svc_state[client_reg_result->service_id];

  if ( (reg_result != QMI_RESULT_SUCCESS) &&
       (qmi_cmd    == QMI_CMD_GET_CLIENT_ID_RESP) &&
       (svc_info != NULL) ) /* svc_info could be NULL if legacy QMUX registration*/
  {
    /*-------------------------------------------------------------------------
      Clean up the client id as it is not honored by the QMI Service
    -------------------------------------------------------------------------*/
    svc_info->client_instance_map[client_reg_result->cl_id -1] = QMI_INSTANCE_MAX;
  }

  /* If source of client allocation is QCSI shim, send result back to it,
     else it needs to be processed by QMI_CTL */
  if (client_reg_result->inst_id == QMI_INSTANCE_SHIM) 
  {
    /* Shim layer is only interested in the client allocation result */
    if (qmi_cmd == QMI_CMD_GET_CLIENT_ID_RESP)
    {
      qmux_qcsi_shim_process_cl_alloc_result(client_reg_result->service_id,
                                             client_reg_result->cl_id,
                                             reg_result);
    }
  }
  else
  {
    qmi_ctl_process_client_registration_result( 
                                      reg_result, 
                                      client_reg_result,
                                      qmi_cmd,
                                      errval);
  }
}

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_ALLOC_CLID()

DESCRIPTION
  Allocate a new Client ID to control point. This function is called by QMI_CTL
  service to allocate a client id and client ID allocation is asynchronous.
  On successfull allocation, the service sends a response to ctl by calling
  qmi_framework_svc_send_alloc_clid_result.

PARAMETERS
  common_hdr : framework common msg hdr type.

RETURN VALUE
  TRUE if command was sent to service.
  FALSE if there was an error.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
qmi_error_e_type  qmi_framework_svc_alloc_clid
(
  qmi_framework_common_msg_hdr_type *  common_hdr
)
{
  uint8                    idx;
  qmi_framework_svc_info_type *   svc_info;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(common_hdr);
  
  if (common_hdr->service >= QMUX_SERVICE_MAX)
  {
    LOG_MSG_ERROR_1 ("Invalid service %d", common_hdr->service);
    return QMI_ERR_INVALID_SERVICE_TYPE;
  }

  svc_info = qmi_framework_svc_state[common_hdr->service];
  if ( (svc_info == NULL) || (!svc_info->registered) )
  {
    LOG_MSG_INFO1_1 ("Service %d not registered with QMI Framework!",
                     common_hdr->service);
    return QMI_ERR_INVALID_SERVICE_TYPE;
  }
    
  /*-------------------------------------------------------------------------
    The clid allocation is done by Framework. The framework finds an available
    client ID, fills it into common_hdr and calls the callback function for
    the service to use this..
   
    Every QMI service which uses the new QMI framework supports 254 clients.
   
    The client ID is chosen based on the index of client_instance_map[] array.
   
    client_instance_map[] array
    -- ----------------------------------------------------------------------
    Index               | 0 | 1 | 2 | 3 | 4 | 5 | 6 | ...........|252|253|   
    --------------------------------------------------------------------------
    value(QMI Instance) | 0 | 1 | 5 | 0 |MAX|MAX|MAX| ...........|MAX|MAX|
    --------------------------------------------------------------------------
    MAX = QMI_INSTANCE_MAX
 
    The framework traverses through the client_instance _map_array to find the
    first element whose value is QMI_INSTANCE_MAX. It calls the alloc_clid
    callback function with Client ID = that index+1 and then sets the value of
    that array element to the qmi instance (which can be retreived from the
    common message header).
   
    When services send responses/indications, the framework validates the instance
    sent in the msg header against the instance stored in the client_instance_map_array
    before sending it out to qmux.
   
    The mapped instance will be freed(assigned back to QMI_INSTANCE_MAX) when the
    client is deallocated.
  -------------------------------------------------------------------------*/
  for (idx = 0; idx < (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1); idx++)
  {
    if(svc_info->client_instance_map[idx] == QMI_INSTANCE_MAX)
    {
      break;
    }
  }
  
  if (idx >= (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1))
  {
    LOG_MSG_ERROR_1 ("Maximum clients reached for service",
                     svc_info->service_id);
    return QMI_ERR_CLIENT_IDS_EXHAUSTED;
  }

  common_hdr->client_id = idx + 1;

  if(svc_info->cfg.cbs.alloc_clid)
  {
    /*-------------------------------------------------------------------------
      Allocating the new client id and updating the client mask
    -------------------------------------------------------------------------*/
    if (svc_info->cfg.cbs.alloc_clid(common_hdr))
    {
    /*-------------------------------------------------------------------------
      Initialise sequence number for the allocated client
    -------------------------------------------------------------------------*/
      svc_info->seq_no[common_hdr->client_id-1] = 0;
    
      svc_info->client_instance_map[common_hdr->client_id-1] = 
                                (qmi_instance_e_type)common_hdr->qmi_instance;
      LOG_MSG_INFO2_3 ("Alloc new Framework Clid: %d Service: %d and Instance: %x",
                       common_hdr->client_id,
                       common_hdr->service,
                       common_hdr->qmi_instance);
      return QMI_ERR_NONE;
    }
  }
  else
  {
    LOG_MSG_ERROR_0 ("Clid allocation failed due to invalid svc cb");
  }

  return QMI_ERR_INTERNAL;
} /* qmi_framework_svc_alloc_clid() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_DEALLOC_CLID()

DESCRIPTION
  Deallocate the new Client ID.This function is called by QMI_CTL service to
  deallocate the client id and client ID de allocation is asynchronous. On
  successfull deallocation, the service sends a response to ctl by calling
  qmi_framework_svc_send_dealloc_clid_result.
  
PARAMETERS
  common_hdr : common header.

RETURN VALUE
  TRUE if command was sent to service.
  FALSE if there was an error.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
qmi_error_e_type qmi_framework_svc_dealloc_clid
(
  qmi_framework_common_msg_hdr_type *    common_hdr
)
{
  qmi_framework_svc_info_type*  svc_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT(common_hdr);
  
  if (common_hdr->service >= QMUX_SERVICE_MAX)
  {
    LOG_MSG_ERROR_1 ("Invalid service %d", common_hdr->service);
    return QMI_ERR_INVALID_SERVICE_TYPE;
  }

  /*-----------------------------------------------------------------------
    Take the qmux service. client id , check how many clids are assigned for 
    each instance and if it is greater than some max return unused clid
    else allocate a client id and call the call back for the tech.
  -----------------------------------------------------------------------*/
  svc_info = qmi_framework_svc_state[common_hdr->service];
  if (svc_info == NULL)
  {
    LOG_MSG_ERROR_1("Unknown service [%d]", common_hdr->service);
    return QMI_ERR_INVALID_SERVICE_TYPE;
  }
  
  
  if( common_hdr->client_id > QMI_SVC_CLID_UNUSED &&
      common_hdr->client_id < QMI_FRAMEWORK_SVC_MAX_CLIENTS &&
      common_hdr->qmi_instance == svc_info->client_instance_map[common_hdr->client_id-1])
  {
      svc_info->client_instance_map[common_hdr->client_id-1] = QMI_INSTANCE_MAX;
  }
  else
  {
    LOG_MSG_ERROR_1 ("Invalid clid:%d to deallocate ", common_hdr->client_id );
    return QMI_ERR_INVALID_CLIENT_ID;
  }

  /*-------------------------------------------------------------------------
    Deallocating the client id.
  -------------------------------------------------------------------------*/
  if ( svc_info->cfg.cbs.dealloc_clid )
  {
    svc_info->cfg.cbs.dealloc_clid(common_hdr);
    /*-------------------------------------------------------------------------
      Reset sequence number for the de-allocated client
    -------------------------------------------------------------------------*/
    svc_info->seq_no[common_hdr->client_id-1] = 0;
    LOG_MSG_INFO2_3 ("Dealloc the Framework Clid: %d Service: %d and Instance: %x",
                     common_hdr->client_id,
                     common_hdr->service,
                     common_hdr->qmi_instance);

    return QMI_ERR_NONE;
  }
  else
  {
    LOG_MSG_ERROR_0 ("Clid deallocation failed due to invalid svc cb");
  }

  return QMI_ERR_INTERNAL;

} /* qmi_framework_svc_deallocate_clid() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_POWERUP_INIT()

DESCRIPTION
  QMI Framework power up initialization.

PARAMETERS
  None

RETURN VALUE
  TRUE on successful initialization.
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean qmi_framework_powerup_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize the QMI state structure.
  -------------------------------------------------------------------------*/
  memset( qmi_framework_svc_state, 0, sizeof(qmi_framework_svc_state) );

  qmi_framework_powerup_inited = TRUE;
  return TRUE;

} /* qmi_framework_powerup_init() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_QMI_LINK_CLOSED()

DESCRIPTION
  Free all existing service clients, as QMI link is closed.

PARAMETERS
  instance - the QMI instance whose link was disconnected

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void  qmi_framework_svc_qmi_link_closed
(
  qmi_instance_e_type  instance
)
{
  qmi_framework_svc_info_type*  svc_info;
  int i;
  int j;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(instance < QMI_INSTANCE_MAX);
  LOG_MSG_INFO1_1 ("QMI Framework clean instance %d resources", instance);

  for (i = 0; i < QMUX_SERVICE_MAX; i++)
  {
    svc_info = qmi_framework_svc_state[i];
    if (svc_info == NULL)
    {
      continue;
    }

    if (svc_info->registered == TRUE)
    {
      for (j = 0 ; j < (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1); j++)
      {
        if(svc_info->client_instance_map[j] == instance) 
        {
          qmi_framework_common_msg_hdr_type common_msg_hdr;

          LOG_MSG_INFO1_3 ("QMI link closed, do service %d cleanup- instance:%d - clid:%d",
                           svc_info->service_id, instance, j + 1 );

          memset(&common_msg_hdr, 0, sizeof(common_msg_hdr));
          common_msg_hdr.service        = svc_info->service_id; 
          common_msg_hdr.client_id      = j + 1;
          /*-----------------------------------------------------------------------
            When the Transaction starts from a Client(or Control point), it  must
            have a Non zero and valid Transaction ID but here the Transaction has
            starting from framework itself. So the framework use 0 as a TransactionID
            here and it will discard the asynchronous response received from QMI
            service to the below "qmi_framework_svc_dealloc_clid" call.
          -------------------------------------------------------------------------*/
          common_msg_hdr.transaction_id = 0;
          common_msg_hdr.qmi_instance   = (int32)instance;
          qmi_framework_svc_dealloc_clid(&common_msg_hdr);
        }
      }
    }
  }
} /* qmi_svc_qmi_link_closed() */ 

/*===========================================================================
  FUNCTION QMI_FRAMEWORK_GET_SERVICE_CFG()

  DESCRIPTION
    Get Service configuration

  PARAMETERS
    svc_type    : type of qmux service    

  RETURN VALUE
    qmi_framework_svc_config_type
    NULL if input service handle or service type is invalid

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_framework_svc_config_type *  qmi_framework_get_service_cfg
(
  qmux_service_e_type  svc_type
) 
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*---------------------------------------------------------------------
     First determine if svc_type is referring to QC internal services or 
     external services and then return appropriate cfg.
  ---------------------------------------------------------------------*/
  if ( svc_type < QMUX_SERVICE_MAX )
  {  
    if ( (qmi_framework_svc_state[svc_type] != NULL) &&
         (qmi_framework_svc_state[svc_type]->registered == TRUE))
    {
      return &qmi_framework_svc_state[svc_type]->cfg;
    }
  }
  /*---------------------------------------------------------------------
    Return NULL if:
     - svc_type is not Internal QC service range or not registered with
     Framework
  ---------------------------------------------------------------------*/
  return NULL;

}/* qmi_framework_get_service_cfg */

/*===========================================================================

                              EXTERNAL FUNCTION

===========================================================================*/

/*===========================================================================
FUNCTION QMI_FRAMEWORK_REG_SERVICE()

DESCRIPTION
  Register a new QMI service to the Framework with a service configuration.
  Once registration is successful subsequently the service init callback
  function gets called to initialize its service specific configuration.

PARAMETERS
  service      : service type
  cfg          : configuration parameters for the service instance

RETURN VALUE
  qmi_framework_err_e_type  : QMI_FRAMEWORK_ERR_NONE on successful registration. 

DEPENDENCIES 
  None

SIDE EFFECTS
  None
===========================================================================*/
qmi_framework_err_e_type qmi_framework_reg_service
(
  qmux_service_e_type               service,
  qmi_framework_svc_config_type *   cfg
)
{
  qmi_framework_svc_info_type*  svc_info;
  qmi_framework_err_e_type err;
  int32  ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT (service < QMUX_SERVICE_MAX);
  ASSERT (cfg);
  
  do
  {
  if (!qmi_framework_powerup_inited)
  {
      err = QMI_FRAMEWORK_ERR_FW_NOT_READY;
      break;
  }
  
  svc_info = qmi_framework_svc_state[service];

  /* Alloc service info memory */
  if (svc_info == NULL)
  {
    PS_SYSTEM_HEAP_MEM_ALLOC(svc_info, sizeof(qmi_framework_svc_info_type),
                             qmi_framework_svc_info_type*);

    if (svc_info == NULL)
    {
        err = QMI_FRAMEWORK_ERR_MEMORY;
        break;
    }

    memset(svc_info, 0, sizeof(qmi_framework_svc_info_type));

    memset( svc_info->client_instance_map, 
            QMI_INSTANCE_MAX, 
            sizeof(svc_info->client_instance_map) );
    svc_info->service_id = service;      /* QMUX service type */
    (void) memscpy(&svc_info->cfg, 
                   sizeof(svc_info->cfg), 
                   cfg, 
                   sizeof(svc_info->cfg));  /* store the provided config */
    qmi_framework_svc_state[service] = svc_info;
  }


  /*-------------------------------------------------------------------------
    validate the Service is already registered. 
  -------------------------------------------------------------------------*/
  if (svc_info->registered == TRUE)
  {
      err = QMI_FRAMEWORK_ERR_SVC_ALREADY_REGISTERED;
      break;
  }
  
  /*-------------------------------------------------------------------------
    validate the mandatory callback on the passed service config. 
  -------------------------------------------------------------------------*/
  if ((cfg->cbs.alloc_clid == NULL  ) ||
      (cfg->cbs.dealloc_clid == NULL)||       
      (cfg->cbs.cmd_hdlr == NULL) || 
      (cfg->cbs.init_cback == NULL) 
  )
  {
      err = QMI_FRAMEWORK_ERR_SVC_REG_MISSING_CBACK;
      break;
  }
  
  /*-------------------------------------------------------------------------
    Store the Service provided config in the Framework state cache. 
  -------------------------------------------------------------------------*/
  svc_info->registered = TRUE;

  /*-------------------------------------------------------------------------
    Call service init call back function to initialize service and
    client state 
  -------------------------------------------------------------------------*/
  svc_info->cfg.cbs.init_cback(QMI_INSTANCE_MAX);

  /*-------------------------------------------------------------------------
    Service registration successful
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
  /*Notify Control Point about service availability*/
  qmux_notify_service_registration
                 (svc_info->service_id, 
                  0, // This field is ignored for new QUMX based services
                  FALSE);// Send FALSE here as this is a new QMUX based Service
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/
    /*Register the service with QCSI shim layer*/
    qmux_qcsi_shim_reg(svc_info->service_id);
    ret_val = 0;
  }while(0);

  if(ret_val == -1)
  {
    LOG_MSG_INFO1_2("qmi_framework_reg_service failed for service = %d err = %d",service,err);
    return err;
  }
  else
  {
    LOG_MSG_INFO1_1("service %d successfully registered",service);
    return QMI_FRAMEWORK_ERR_NONE;
  }

} /* qmi_framework_reg_service() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_DEREG_SERVICE()

DESCRIPTION
  Deregister the already registered QMI service from framework.Once
  deregistration is successful subsequently the service dealloc callback
  function gets called to clean up the service specific pending transaction.
  
PARAMETERS
  service      : service type

RETURN VALUE
  None. 

DEPENDENCIES
  Service should be registered previously.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_framework_dereg_service
(
  qmux_service_e_type     service
)
{
  qmi_framework_svc_info_type *  svc_info;
  int i;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT (service < QMUX_SERVICE_MAX);
  
  svc_info = qmi_framework_svc_state[service];
  if (svc_info == NULL)
  {
    LOG_MSG_ERROR_1("Unknown service [%d]", service);
    return;
  }
  
  if ( !svc_info->registered )
  {
    LOG_MSG_ERROR_1 ("Service %d not registered with QMI Framework!", service);
    return;
  }

  for (i = 0 ; i < (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1); i++)
  {
    if(svc_info->client_instance_map[i] != QMI_INSTANCE_MAX) 
    {
      qmi_framework_common_msg_hdr_type common_msg_hdr;

      LOG_MSG_INFO1_3 ("QMI Framework dereg service, do service %d cleanup- instance:%d - clid:%d",
                       svc_info->service_id,
                       svc_info->client_instance_map[i],
                       i + 1 );
  
      memset(&common_msg_hdr, 0, sizeof(common_msg_hdr));
      common_msg_hdr.service        = svc_info->service_id; 
      common_msg_hdr.client_id      = i + 1;
      /*-----------------------------------------------------------------------
        When the Transaction starts from a Client(or Control point), it  must
        have a Non zero and valid Transaction ID but here the Transaction has
        starting from framework itself. So the framework use 0 as a TransactionID
        here and it will discard the asynchronous response received from QMI
        service to the below "qmi_framework_svc_dealloc_clid" call.
      -------------------------------------------------------------------------*/
      common_msg_hdr.transaction_id = 0;
      common_msg_hdr.qmi_instance   = svc_info->client_instance_map[i];
      qmi_framework_svc_dealloc_clid(&common_msg_hdr);
    }
  }
  
  LOG_MSG_INFO2_1 ("Deregistering service %d from QMI Framework",
                   svc_info->service_id);
  /*-------------------------------------------------------------------------
  Clear service info block
  -------------------------------------------------------------------------*/
  memset (svc_info, 0, sizeof(qmi_framework_svc_info_type));
} /* qmi_framework_dereg_service() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_SEND_RESPONSE()

DESCRIPTION
  External QMI service modules use this function to send a response message
  to the QMI Framework. The QMI framework processes the response and then
  sends to QMUX layer. If the response come from an external task, then it
  performs a task switch to QMI task.

PARAMETERS
  msg_hdr   : msg header sent from svc
  msg_ptr   : response to send out

RETURN VALUE
  TRUE if Framework was able to start processing the message.
  FALSE otherwise.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean qmi_framework_svc_send_response
(
  qmi_framework_msg_hdr_type *  msg_hdr,
  dsm_item_type *               msg_ptr
)
{
  boolean     status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT (msg_ptr);
  ASSERT (msg_hdr);
 
  /* msg_hdr->common_hdr.transaction_id must be non zero value */
  if(msg_hdr->common_hdr.transaction_id != 0)
  {
    status = qmi_frameworki_svc_send( msg_hdr, 
                               QMI_CMD_FRAMEWORK_SEND_RESPONSE,
                               msg_ptr );
  }
  else
  {
    LOG_MSG_INFO2_1 ("Invalid Transaction ID: %d, Discarding the Service response",
                     msg_hdr->common_hdr.transaction_id);
    return FALSE;
  }

  return status;
} /* qmi_framework_svc_send_response() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_SEND_IND()

DESCRIPTION
  External QMI service modules use this function to send an Indication message
  to the QMI Framework. The QMI framework processes the Indication and then
  sends to QMUX layer. If the Indication come from an external task, then it
  performs a task switch to QMI task.

PARAMETERS
  msg_hdr   : msg header sent from svc
  msg_ptr   : Indication to send out

RETURN VALUE
  TRUE if Framework was able to start processing the message.
  FALSE otherwise.
  
DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean qmi_framework_svc_send_ind   
(
  qmi_framework_msg_hdr_type *   msg_hdr,
  dsm_item_type *                ind
)
{
  boolean     status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT (msg_hdr);
  ASSERT (ind);

  if (msg_hdr->common_hdr.service >= QMUX_SERVICE_MAX)
  {
    LOG_MSG_ERROR_1(" Invalid service %d or cl id %d ",
                    msg_hdr->common_hdr.service);
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Increment sequence num for every indication.
    Unique per service/per client.
  -------------------------------------------------------------------------*/
  if ( qmi_framework_svc_state[msg_hdr->common_hdr.service] == NULL || 
       msg_hdr->common_hdr.client_id == QMI_SVC_CLID_UNUSED || 
       msg_hdr->common_hdr.client_id >= QMI_FRAMEWORK_SVC_MAX_CLIENTS ) 
  {
    LOG_MSG_ERROR_2(" Invalid service %d or cl id %d ",
                    msg_hdr->common_hdr.service, 
                    msg_hdr->common_hdr.client_id);
  }
  else
  {
    qmi_framework_svc_state[msg_hdr->common_hdr.service]->seq_no[msg_hdr->common_hdr.client_id-1]++;
    msg_hdr->common_hdr.transaction_id =
      qmi_framework_svc_state[msg_hdr->common_hdr.service]->seq_no[msg_hdr->common_hdr.client_id-1];
  }

  LOG_MSG_INFO2_3("Sending service %d Indication to QMI Framework: clid: %d, tx_id: %d", 
                  msg_hdr->common_hdr.service, 
                  msg_hdr->common_hdr.client_id, 
                  msg_hdr->common_hdr.transaction_id);

  /*msg_hdr->common_hdr.transaction_id is packed with seq no. for indications*/
  status = qmi_frameworki_svc_send( msg_hdr, 
                                    QMI_CMD_FRAMEWORK_SEND_IND,
                                    ind );

  return status;

} /* qmi_framework_svc_send_ind() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_ALLOC_MSG_HEADER()

DESCRIPTION
  Allocates memory for a framework msg header.

PARAMETERS
  None.

RETURN VALUE
 qmi_framework_msg_hdr_type * : ptr to framework msg header type.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
qmi_framework_msg_hdr_type * qmi_framework_alloc_msg_header
(
  void
)
{
  qmi_framework_msg_hdr_type *   msg_hdr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_hdr = modem_mem_alloc( sizeof( qmi_framework_msg_hdr_type ),
                             MODEM_MEM_CLIENT_QMI_CRIT );
  if (msg_hdr == NULL)
  {
    LOG_MSG_ERROR_0 ("Alloc msg_header: No Can't get memory allocation!");
    ASSERT(msg_hdr);
  }

  return msg_hdr;
} /* qmi_framework_alloc_msg_header() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_DEALLOC_MSG_HEADER()

DESCRIPTION
  Deallocates Memory for the framework msg header. 

PARAMETERS
  msg_hdr: pointer pointer to message header.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_framework_dealloc_msg_header
(
  qmi_framework_msg_hdr_type **    msg_hdr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(msg_hdr == NULL)
  {
    LOG_MSG_ERROR_0("NULL pointer passed to qmi_framework_dealloc_msg_header");
    return;
  }
  modem_mem_free( *msg_hdr ,
                   MODEM_MEM_CLIENT_QMI_CRIT);

  *msg_hdr = NULL;
  return;

} /* qmi_framework_dealloc_msg_header() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_SEND_ALLOC_CLID_RESULT()

DESCRIPTION
  QMI service calls this function to report client id allocation success /
  failure to QMI_CTL service.
  This API will no longer be supported. Going forward all services
  are expected to use the QMI_FRAMEWORK_SVC_SEND_ALLOC_CLID_RESULT_EX()
  function instead.

PARAMETERS
  result     : Client ID allocation result.
  common_hdr : common msg hdr type.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_framework_svc_send_alloc_clid_result
(
  qmi_result_e_type                     result,
  qmi_framework_common_msg_hdr_type *   common_msg_hdr
)
{
 qmi_error_e_type    errval = QMI_ERR_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(common_msg_hdr);

  if (result != QMI_RESULT_SUCCESS)
  {
    errval = QMI_ERR_INTERNAL;
  }


  LOG_MSG_INFO2_3 ("Sending service %d alloc clid %d result %d to framework",
                   common_msg_hdr->service, common_msg_hdr->client_id, result);

  /*-----------------------------------------------------------------------
    The Transaction ID is 0 in the common_msg_hdr->transaction_id means the
    transaction has started from Framework itself and no need to send a
    response to QCTL
  -------------------------------------------------------------------------*/
  if(common_msg_hdr->transaction_id != 0 )
  {
    qmi_frameworki_send_client_reg_result( result,
                                           errval,
                                               common_msg_hdr,
                                               QMI_CMD_GET_CLIENT_ID_RESP );
  }
  
} /* qmi_framework_svc_send_alloc_clid_result() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_SEND_DEALLOC_CLID_RESULT()

DESCRIPTION
  QMI service calls this function to report client id deallocation success /
  failure to QMI_CTL service. 
  This API will no longer be supported. Going forward all services
  are expected to use the QMI_FRAMEWORK_SVC_SEND_DEALLOC_CLID_RESULT_EX()
  function instead.

PARAMETERS
  result     : Client ID deallocation result.
  common_hdr : common msg hdr type.

RETURN VALUE
  Void.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
==========================================================================*/
void qmi_framework_svc_send_dealloc_clid_result
(
  qmi_result_e_type                     result,
  qmi_framework_common_msg_hdr_type *   common_msg_hdr
)
{
 qmi_error_e_type    errval = QMI_ERR_NONE;
/*-------------------------------------------------------------------------*/
  ASSERT(common_msg_hdr);

  if (result != QMI_RESULT_SUCCESS)
  {
    errval = QMI_ERR_INTERNAL;
  }

  LOG_MSG_INFO2_3 ("Sending service %d dealloc clid %d result %d to framework",
                   common_msg_hdr->service, common_msg_hdr->client_id, result);

  /*-----------------------------------------------------------------------
    The Transaction ID is 0 in the common_msg_hdr->transaction_id means the
    transaction has started from Framework itself and no need to send a
    response to QCTL
  -------------------------------------------------------------------------*/
  if(common_msg_hdr->transaction_id != 0 )
  {
    qmi_frameworki_send_client_reg_result( result,
                                           errval,
                                               common_msg_hdr,
                                               QMI_CMD_RELEASE_CLIENT_ID_RESP );
  }

} /* qmi_framework_svc_send_dealloc_clid_result() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_SEND_ALLOC_CLID_RESULT_EX()

DESCRIPTION
  QMI service calls this function to report client id allocation success /
  failure to QMI_CTL service.
  Going forward all services are expected to use this API instead of
  qmi_framework_svc_send_alloc_clid_result()

PARAMETERS
  result     : Client ID allocation result.
  common_hdr : common msg hdr type.
  errval     : Err value 

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_framework_svc_send_alloc_clid_result_ex
(
  qmi_result_e_type                     result,
  qmi_framework_common_msg_hdr_type *   common_msg_hdr,
  qmi_error_e_type                      errval
)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(common_msg_hdr);

  LOG_MSG_INFO2_3("Sending alloc clid %d result to Framework. service %d", 
                  common_msg_hdr->client_id, 
                  common_msg_hdr->service, 
                  errval);

  /*-----------------------------------------------------------------------
    The Transaction ID is 0 in the common_msg_hdr->transaction_id means the
    transaction has started from Framework itself and no need to send a
    response to QCTL
  -------------------------------------------------------------------------*/
  if(common_msg_hdr->transaction_id != 0 )
  {
    qmi_frameworki_send_client_reg_result( result,
                                           errval,
                                           common_msg_hdr,
                                           QMI_CMD_GET_CLIENT_ID_RESP );
  }
  
} /* qmi_framework_svc_send_alloc_clid_result_ex() */

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_SEND_DEALLOC_CLID_RESULT_EX()

DESCRIPTION
  QMI service calls this function to report client id deallocation success /
  failure to QMI_CTL service.
  Going forward all services are expected to use this API instead of
  qmi_framework_svc_send_dealloc_clid_result()

PARAMETERS
  result     : Client ID deallocation result.
  common_hdr : common msg hdr type.
  errval     : Error value
RETURN VALUE
  Void.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
==========================================================================*/
void qmi_framework_svc_send_dealloc_clid_result_ex
(
  qmi_result_e_type                     result,
  qmi_framework_common_msg_hdr_type *   common_msg_hdr,
  qmi_error_e_type                      errval
)
{
 /*-------------------------------------------------------------------------*/
  ASSERT(common_msg_hdr);

  LOG_MSG_INFO2_3("Sending dealloc clid %d result to Framework. service %d", 
                  common_msg_hdr->client_id, 
                  common_msg_hdr->service, 
                  errval);

  /*-----------------------------------------------------------------------
    The Transaction ID is 0 in the common_msg_hdr->transaction_id means the
    transaction has started from Framework itself and no need to send a
    response to QCTL
  -------------------------------------------------------------------------*/
  if(common_msg_hdr->transaction_id != 0 )
  {
    qmi_frameworki_send_client_reg_result( result,
                                           errval,
                                           common_msg_hdr,
                                           QMI_CMD_RELEASE_CLIENT_ID_RESP );
  }

} /* qmi_framework_svc_send_dealloc_clid_result_ex() */

