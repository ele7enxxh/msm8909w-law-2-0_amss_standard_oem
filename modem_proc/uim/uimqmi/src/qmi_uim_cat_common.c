/*================================================================================

DESCRIPTION


Copyright (c) 2013 - 2014, 2016 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_uim_cat_common.c#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/11/16    ar     Changes for LPA support
03/27/14    at     Usage of common QMI APIs from DS header
02/12/14   at      Additional check in the free transaction API
01/10/14   df      Use v2.0 diag macros
10/08/13   gm      Removed unused comments
10/04/13   gm      F3 message reduction
06/12/13   kb      Added QMI malloc and free wrapper functions
04/24/13   tkl     Hardening qmi_svc_new_cmd_buf against overflow
09/07/12   at      Initial version
=============================================================================*/


#include "customer.h"
#include "comdef.h"
#include "uim_msg.h"
#include "err.h"
#include "amssassert.h"
#include "qmi_uim_cat_common.h"
#include "modem_mem.h"
#include "ds_qmi_fw_common.h"
#include <stringl/stringl.h>

/*---------------------------------------------------------------------------
  QMI common client state detail for QMI UIM or CAT message
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_common_client_state_type       common; // must be first since we alias
  int16                              instance;
  int16                              service_id;
} uimqmi_common_client_state_type;

/*===========================================================================
  FUNCTION UIMQMI_MALLOC

  DESCRIPTION
    The uimqmi_malloc calls modem_mem_calloc to request memory from the Modem Heap.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void * uimqmi_malloc
(
  uint32 size
)
{
  return modem_mem_calloc(1, (size), MODEM_MEM_CLIENT_UIM);
}/* uimqmi_malloc */


/*===========================================================================
  FUNCTION UIMQMI_FREE

  DESCRIPTION
    The uimqmi_free frees the pointer from the Modem Heap.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_free
(
  void * ptr
)
{
  modem_mem_free((void*)(ptr), MODEM_MEM_CLIENT_UIM);
}/* uimqmi_free */


/*===========================================================================
  FUNCTION UIMQMI_MMGSDI_SLOT_TO_SLOT_INDEX()

  DESCRIPTION
    Determines the slot index from the MMGSDI slot id

  PARAMETERS
    mmgsdi_slot_id      : MMGSDI slot id
    slot_index_ptr      : output parameter containing slot index

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type uimqmi_mmgsdi_slot_to_slot_index
(
  mmgsdi_slot_id_enum_type       mmgsdi_slot_id,
  uint8                        * slot_index_ptr
)
{
  ASSERT(slot_index_ptr);

  switch(mmgsdi_slot_id)
  {
    case MMGSDI_SLOT_1:
      *slot_index_ptr = 0;
      break;

    case MMGSDI_SLOT_2:
      *slot_index_ptr = 1;
      break;

    case MMGSDI_SLOT_3:
      *slot_index_ptr = 2;
      break;

    default:
      UIM_MSG_ERR_1("Invalid slot: 0x%x", mmgsdi_slot_id);
      return QMI_ERR_INTERNAL;
  }

  return QMI_ERR_NONE;
}/* uimqmi_mmgsdi_slot_to_slot_index */


/*===========================================================================
  FUNCTION UIMQMI_DEEP_COPY()

  DESCRIPTION
    This function allocates memory and duplicates the content of a buffer.
    This is a utility function to make deep copies.

  PARAMETERS
    dest : pointer to the destination buffer. Output value.
    src  : pointer to the source buffer
    size : size of the source buffer

  RETURN VALUE
    None. Errors are ignored.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_deep_copy
(
  void       ** dest,
  const void  * src,
  uint32        size
)
{
  if (dest != NULL)
  {
    *dest = NULL;
    if (src != NULL && size > 0)
    {
      *dest = uimqmi_malloc(size);
      if (*dest != NULL)
      {
        (void)memscpy(*dest, size, src, size);
      }
    }
  }
} /* uimqmi_deep_copy */


/*===========================================================================
FUNCTION UIMQMI_SEND_RESPONSE()

DESCRIPTION
  This function calls QMI Framework API to send a QMI UIM or CAT Service
  response to a client. It always frees cmd_buf_p and when successful frees
  the transaction.

PARAMETERS
  cmd_buf_p   : command buffer
  msg_ptr     : dsm item
  service_type: service type

RETURN VALUE
  TRUE: If responses send to Framework
  FALSE: otherwise

DEPENDENCIES
  QMI UIM or CAT service must be initialized and registered with Framework

SIDE EFFECTS
  Frees cmd_buf_p

===========================================================================*/
static boolean uimqmi_send_response
(
  qmi_cmd_buf_type           * cmd_buf_p,
  dsm_item_type              * msg_ptr,
  qmux_service_e_type          service_type
)
{
  qmi_transaction_type             * x_p          = NULL;
  uimqmi_common_client_state_type  * common_cl_sp = NULL;
  qmi_framework_msg_hdr_type         msg_hdr;

  ASSERT (msg_ptr);
  ASSERT (cmd_buf_p);
  ASSERT (cmd_buf_p->x_p);
  ASSERT (cmd_buf_p->x_p->cl_sp);

  memset(&msg_hdr,0, sizeof(msg_hdr));
  common_cl_sp = (uimqmi_common_client_state_type*)cmd_buf_p->x_p->cl_sp;

  if(FALSE == qmi_svc_prepend_msg_hdr(&msg_ptr, cmd_buf_p->cmd_type))
  {
    UIM_MSG_ERR_1("Unable to send Response for cmd %d", cmd_buf_p->cmd_type);
    /* Freeing msg_ptr is done in qmi_svc_prepend_msg_hdr for failure */
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
    return FALSE;
  }

  /* Get the transaction from the command buffer*/
  x_p = cmd_buf_p->x_p;

  /* We do not support bundled commands only check the first command in list
     make sure we only have one command and be sure it's expecting a resp.
     Validate service_id as well*/
  if( (x_p->n_cmds != 1) ||
      (cmd_buf_p->cmd_no != 0) ||
      (cmd_buf_p != x_p->cmd_list[cmd_buf_p->cmd_no]) ||
      (x_p->resp_list[cmd_buf_p->cmd_no] != QMI_SVC_RESPONSE_PENDING) ||
      (common_cl_sp->service_id != (uint8)service_type) ||
      (common_cl_sp->common.clid == QMI_SVC_CLID_UNUSED) ||
      (common_cl_sp->common.clid > UIMQMI_SVC_MAX_CLIDS))
  {
    UIM_MSG_ERR_0("Command is linked to transaction it doesn't belong to! discarding");
    dsm_free_packet(&msg_ptr);
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
    return FALSE;
  }

  /* Freeup the transaction and the command buffer as it no longer used */
  ds_qmi_fw_free_cmd_buf(&cmd_buf_p);

  msg_hdr.common_hdr.client_id      = common_cl_sp->common.clid;
  msg_hdr.common_hdr.qmi_instance   = common_cl_sp->instance;
  msg_hdr.common_hdr.service        = (qmux_service_e_type)common_cl_sp->service_id;
  msg_hdr.common_hdr.transaction_id = x_p->x_id;

  msg_hdr.msg_ctl_flag = QMI_FLAG_MSGTYPE_RESP;

  msg_hdr.msg_len  = (uint16) dsm_length_packet(msg_ptr);

  /* qmi_framework_svc_send_response takes care of freeing msg_ptr */
  (void)qmi_framework_svc_send_response(&msg_hdr, msg_ptr);

  /* free the transaction buffer */
  ds_qmi_fw_free_transaction(&x_p);
  return TRUE;
} /* uimqmi_send_response */


/*===========================================================================
  FUNCTION UIMQMI_SEND_INDICATION()

  DESCRIPTION
    This function calls the QMI Framework API to send out the UIM or CAT
    service indication to client.

  PARAMETERS
    client_id   : client ID
    cmd_type    : type of indication
    ind_ptr     : dsm item
    service_type: service type

  RETURN VALUE
    TRUE  - For success
    FALSE - For qmi svc failure

  DEPENDENCIES
    QMI UIM or CAT service must be initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
boolean uimqmi_send_indication
(
  uint8                        client_id,
  uint16                       cmd_type,
  dsm_item_type              * ind_ptr,
  qmux_service_e_type          service_type
)
{
  qmi_framework_msg_hdr_type framework_msg_hdr;

  ASSERT(ind_ptr);

  if (client_id == QMI_SVC_CLID_UNUSED || client_id > UIMQMI_SVC_MAX_CLIDS)
  {
    UIM_MSG_ERR_1("Invalid Client ID: 0x%x", client_id);
    return FALSE;
  }

  /* For indication the transaction ID can be ignored */
  framework_msg_hdr.common_hdr.transaction_id = 0;
  /* If unicast indication, qmi framework map the instance and no need to fill*/
  framework_msg_hdr.common_hdr.qmi_instance   = UIMQMI_SVC_INVALID_INSTANCE;
  framework_msg_hdr.common_hdr.client_id      = client_id;
  framework_msg_hdr.common_hdr.service        = service_type;
  framework_msg_hdr.msg_ctl_flag              = QMI_FLAG_MSGTYPE_IND;
  framework_msg_hdr.msg_len                   = (uint16)dsm_length_packet(ind_ptr);

  UIM_MSG_HIGH_2("Sending QMI Indication to QMI Framework: clid:0x%x, ind_id:0x%x",
                 client_id, cmd_type);

  if(FALSE == qmi_svc_prepend_msg_hdr(&ind_ptr, cmd_type))
  {
    UIM_MSG_ERR_1("Unable to build indication for cmd 0x%x", cmd_type);
    return FALSE;
  }

  if(FALSE == qmi_framework_svc_send_ind(&framework_msg_hdr, ind_ptr))
  {
    UIM_MSG_ERR_0("Unable to send QMI indication to QMI Framework" );
    return FALSE;
  }
  return TRUE;
} /* uimqmi_send_indication */


/*===========================================================================
  FUNCTION UIMQMI_PUT_RESULT_SEND_RESPONSE_IND

  DESCRIPTION
    This function copies the result TLV and send response or indication to
    the client.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_put_result_send_response_ind
(
  qmi_cmd_buf_type *       cmd_buf_ptr,
  dsm_item_type *          response_ptr,
  qmi_error_e_type         errval,
  qmux_service_e_type      service_type,
  boolean                  ind_required,
  uint8                    indication_clid,
  uint16                   indication_id
)
{
  qmi_result_e_type        result           = QMI_RESULT_FAILURE;
  boolean                  retval           = TRUE;

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response_ptr, result, errval);
  if(FALSE == retval)
  {
    dsm_free_packet(&response_ptr);
    if(cmd_buf_ptr != NULL)
    {
      ds_qmi_fw_free_cmd_buf(&cmd_buf_ptr);
    }	
    return;
  }

  /* Send response or indication */
  if (ind_required)
  {
    retval = uimqmi_send_indication(indication_clid,
                                    indication_id,
                                    response_ptr,
                                    service_type);
  }
  else
  {
    retval = uimqmi_send_response(cmd_buf_ptr, response_ptr, service_type);
  }
  if (FALSE == retval)
  {
    dsm_free_packet(&response_ptr);
    if(cmd_buf_ptr != NULL)
    {
      ds_qmi_fw_free_cmd_buf(&cmd_buf_ptr);
    }
  }
}/* uimqmi_put_result_send_response_ind */
