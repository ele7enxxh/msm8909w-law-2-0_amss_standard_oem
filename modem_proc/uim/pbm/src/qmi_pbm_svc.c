/*===========================================================================

                         Q M I _ PBM _ S V C . C

DESCRIPTION

 The QMI PBM Services Source file. Contains routines common to
 all QMI services.

 Note this library assumes caller is running in QMI-PBM task context.

EXTERNALIZED FUNCTIONS

   qmi_svc_prepend_msg_hdr()
     Append the message header type and length to the provided QMI service
     message payload

Copyright (c) 2005-2014 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm_svc.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/09/14    stv    Replace dsm_pushdown_packed with QMI_SVC_PKT_PUSH
07/07/14    NR     Use v2.0 diag macros
04/04/14    stv    Migration to DATA exposed API usage
02/28/14    stv    get_pb_capabilities new tlv addition packing issue
                   addition of encode/decode idl libraries
26/12/11    sk     Initial changes

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "dsm.h"
#include "task.h"
#include "msg.h"
#include "modem_mem.h"
#include "qmi_svc_defs.h"
#include "qmi_svc_utils.h"
#include "qmi_framework.h"
#include "qmi_pbm_svc.h"
#include "qmi_pbm.h"
#include "ps_in.h"
#include "qmi_pbm_svci.h"
#include "pbmutils.h"
#include "qmi_pbm_if.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION QMI_PBM_SVC_INIT()

DESCRIPTION
  This function initializes various qmi services in qmi pbm task.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_pbm_svc_init
(
  void
)
{
  /* Initialize qmi services here */
  qmi_pbm_init();

}/* qmi_pbm_svc_init */


/*===========================================================================
  FUNCTION QMI_PBM_IDL_MESSAGE_DECODE()

  DESCRIPTION
    Decode QMI PBM message into structure from incoming dsm item

  PARAMETERS
    svc         The QMI IDL service object.
    message_id  QMI message id
    msg_ptr     Pointer to dsm item that contains TLVs
    p_req       Pointer to structure to contain decoded message
    p_req_size  Size of structure to contain decoded data

  RETURN VALUE
    QMI_ERR_NONE_V01 is returned if decode is successful,
    otherwise relevant error is returned

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_pbm_idl_message_decode
(
  qmi_idl_service_object_type svc,
  uint16_t message_id,
  dsm_item_type *msg_ptr,
  void *p_req,
  uint32_t p_req_size
)
{
  qmi_error_type_v01 errval = QMI_ERR_NONE_V01;
  int32_t            retval = QMI_IDL_LIB_NO_ERR;

  // We return QMI_ERR_NONE_V01 if msg_ptr is NULL.
  // The function is going to verify the malformed message in case there is no TLVs present.
  if( msg_ptr != NULL )
  {
    retval = qmi_idl_message_decode( svc,
                                     QMI_IDL_REQUEST,     // IDL message type
                                     message_id,          // QMI PBM cmd type
                                     msg_ptr->data_ptr,   // pointer to incoming byte stream
                                     msg_ptr->used,       // pointer to no of bytes to be read
                                     p_req,               // pointer to structure
                                     p_req_size           // max size of request message, don't get this from IDL_ACCESSOR_MAX_LEN function
                                    );

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
      UIM_MSG_ERR_2( "qmi_pbm_idl_message_decode failed %d %d", retval, errval);
    }
  }

  return errval;
}

/*===========================================================================
  FUNCTION QMI_PBM_IDL_MESSAGE_ENCODE()

  DESCRIPTION
    Encode QMI PBM message into dsm item from incoming c structure

  PARAMETERS
    svc          The QMI IDL service object.
    type         QMI IDL type ( QMI_IDL_RESPONSE or QMI_IDL_INDICATION )
    message_id   QMI message id
    p_src        Pointer to structure containing message data
    p_src_size   Size of structure that contains message data
    msg_ptr      Address of pointer to dsm item for storing encoded message data

  RETURN VALUE
    TRUE if success, FALSE if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_pbm_idl_message_encode
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

  if ( eval == QMI_IDL_LIB_NO_ERR )
  {
    rsp_buf = (uint8 *)QMI_PBM_IF_MEM_ALLOC(rsp_buf_len);



    if ( rsp_buf != NULL )
    {
      memset(rsp_buf, 0x00, rsp_buf_len);
      eval2 = qmi_idl_message_encode( svc,
                                      type,         // IDL message type
                                      message_id,   // QMI PBM cmd type
                                      p_src,        // pointer to structure generated by IDL compiler
                                      p_src_size,   // size of structure which is to be encoded
                                      rsp_buf,      // buffer to contain encoded message data
                                      rsp_buf_len,  // size of buffer, should be maximum size of the encoded c structure
                                      &size_buf     // pointer to store length of the encoded message
                                    );

      if ( eval2 == QMI_IDL_LIB_NO_ERR )
      {
        if (! ( QMI_SVC_PKT_PUSH(p_response, &rsp_buf[0],size_buf) ) ) 
        {
          UIM_MSG_ERR_1( "Pushdown Failed for qmi_pbm_idl_message_encode %d", size_buf);
          dsm_free_packet(p_response);
        }
        else
        {
          retval = TRUE;
        }
      }
      else
      {
        UIM_MSG_ERR_2( "qmi_idl_message_encode failed eval2 = %d,  src_size = %d", eval2, p_src_size);
      }
    }
    else
    {
      UIM_MSG_ERR_1( "Not enough memory to allocate for QMI_PBM_IDL_MESSAGE_ENCODE cmd_type = %d", message_id);
    }
  }
  else
  {
    UIM_MSG_ERR_1( "qmi_idl_get_max_message_len failed %d", eval);
  }


  QMI_PBM_IF_MEM_FREE(rsp_buf);

  return retval;
}


