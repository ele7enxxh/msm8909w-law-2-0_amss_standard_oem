 /*===========================================================================

                         Q M I _ S V C _ I M P L . C

DESCRIPTION

This file defines the interface for the Terminal Equipment to retrieve the  
 implementation details of a service on the MPSS (Modem Processor Sub-System).
 
EXTERNALIZED FUNCTIONS

  qmi_svc_impl_get_supported_msgs_req()
    service supported message-ids with return if it is available  
  
  qmi_svc_impl_get_supported_fields_req()
    service supported TLV fields for request, response and indication messages
    for corresponding message id with return if it is available
             
Copyright (c) 2012 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/src/qmi_svc_impl.c#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
09/06/13    sah    Fix for KW errors
07/30/13    vrk    Fix for KW errors
01/10/13    svj    Fix for KW warnings 
07/10/12    pb     Ability to retrieve the supported interfaces / TLVs per 
                   service from the implementation 
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include <stdio.h>
#include <string.h>
#include "dsm_init.h" 
#include "ds_qmi_svc.h"
#include "dsm_item.h" 
#include "modem_mem.h"
#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "qmi_svc_impl.h"
#include "common_v01.h"
#include "ps_utils.h"

/*===========================================================================

                                DEFINITIONS

===========================================================================*/

/*=========================================================================== 
  FUNCTION QMI_SVC_IMPL_GET_SUPPORTED_MSGS_RESP()    

  DESCRIPTION 
    service supported message-ids with return if it is available
 
  PARAMETERS
    si_tbl           : pointer to the list of supported message-ids for a
                       given service
    svc_obj          : handle to the static service object

  RETURN VALUE 
    ptr to response or NULL if none.

  DEPENDENCIES 
    None 

  SIDE EFFECTS 
    None 
  =========================================================================*/ 
dsm_item_type* qmi_svc_impl_get_supported_msgs_resp
(
  const qmi_implemented_messages *  si_tbl, 
  qmi_idl_service_object_type       svc_obj
)
{
  dsm_item_type *                    response = NULL;
  qmi_error_type_v01                 errval = QMI_ERR_NONE_V01; 
  qmi_error_type_v01                 eval = QMI_IDL_LIB_NO_ERR;
  uint32                             rsp_enc_len = 0;
  uint32                             rsp_buf_len = 0;
  uint32                             rsp_msg_len = 0;
  uint8 *                            rsp_buf = NULL;
  qmi_get_supported_msgs_resp_v01 *  rsp_msg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  do
  {
    /*-------------------------------------------------------------------------
      Accessor function for getting the maximum message length of a response
      message for QMI_GET_SUPPORTED_MESSAGES_ID. Message length includes c
      struct size, size of type and size of length.
    --------------------------------------------------------------------------*/
    eval = qmi_idl_get_max_message_len(svc_obj, 
                                       QMI_IDL_RESPONSE, 
                                       QMI_GET_SUPPORTED_MESSAGES_ID, 
                                       &rsp_buf_len);
    if ( eval != QMI_IDL_LIB_NO_ERR )
    {
      errval = QMI_ERR_INTERNAL_V01;
      break;
    }

    /*-------------------------------------------------------------------------
      Accessor function for getting the c struct size of a response message
      for QMI_GET_SUPPORTED_MESSAGES_ID.
    --------------------------------------------------------------------------*/
    eval = qmi_idl_get_message_c_struct_len(svc_obj, 
                                            QMI_IDL_RESPONSE, 
                                            QMI_GET_SUPPORTED_MESSAGES_ID, 
                                            &rsp_msg_len);
    if ( eval != QMI_IDL_LIB_NO_ERR )
    {
      errval = QMI_ERR_INTERNAL_V01;
      break;
    }    
  } while ( 0 );

  if ( errval == QMI_ERR_NONE_V01 )
  {
    if ( !(rsp_msg = (qmi_get_supported_msgs_resp_v01 *)
          modem_mem_calloc(1, rsp_msg_len, MODEM_MEM_CLIENT_QMI)) ||
         !(rsp_buf = (uint8 *)
          modem_mem_calloc(1, rsp_buf_len, MODEM_MEM_CLIENT_QMI)) )
    {
      LOG_MSG_ERROR_1( "Not enough memory to allocate for "
                       "QMI_GET_SUPPORTED_MSGS, MsgID:0x%x",
                       QMI_GET_SUPPORTED_MESSAGES_ID );
      errval = QMI_ERR_NO_MEMORY_V01;
      goto sendresponse;
    }
  }

  if ( errval == QMI_ERR_NONE_V01 )
  {
    /*-----------------------------------------------------------------------
     Generic function to load the implementation details in rsp_msg struct
    -----------------------------------------------------------------------*/
    if( qmi_si_get_supported_msgs_resp(si_tbl, rsp_msg) != QMI_SI_SUCCESS )
    {
      errval = QMI_ERR_INFO_UNAVAILABLE_V01;
    }   
  }

  /*-------------------------------------------------------------------------
    If qmi_si_get_supported_msgs_resp() completes without error, it also
    fills in the rsp_msg->resp fields. Otherwise they are filled in here with
    the appropriate error value.
  -------------------------------------------------------------------------*/
  if ( errval != QMI_ERR_NONE_V01 )
  {
    if(rsp_msg != NULL)
    {
      rsp_msg->resp.error = errval;
      rsp_msg->resp.result = QMI_RESULT_FAILURE_V01;
    }
  }
  
  /*-------------------------------------------------------------------------
    Encodes the body of a QMI message from the C data structure to the wire
    format
  -------------------------------------------------------------------------*/
  if (errval == QMI_ERR_NONE_V01)
  {
    if ( QMI_IDL_LIB_NO_ERR != qmi_idl_message_encode(svc_obj,
                                                 QMI_IDL_RESPONSE,
                                                 QMI_GET_SUPPORTED_MESSAGES_ID, 
                                                 rsp_msg,
                                                 rsp_msg_len,
                                                 rsp_buf,
                                                 rsp_buf_len,
                                                 &rsp_enc_len) )
    {
      LOG_MSG_ERROR_2( "Message Encode Failed for QMI_GET_SUPPORTED_MSGS, "
                       "MsgID:0x%x, Msg:0x%x",
                       QMI_GET_SUPPORTED_MESSAGES_ID, &rsp_msg );
      rsp_enc_len = 0; /* incase of failure */
    }

    /*-------------------------------------------------------------------------
      Inserts the specified amount of data in the dsm_item_type pointer from
      the buffer
     -------------------------------------------------------------------------*/
    if ( rsp_enc_len != PS_UTILS_DSM_PUSHDOWN_PACKED(&response,
                                       &rsp_buf[0],
                                       (uint16)rsp_enc_len,
                                       DSM_DS_SMALL_ITEM_POOL) )
    {
      dsm_free_packet(&response);
    }
  }
  
  sendresponse:

  if(rsp_buf != NULL)
  {
     modem_mem_free((uint8*) rsp_buf, MODEM_MEM_CLIENT_QMI);
  }

  if(rsp_msg != NULL)
  {
     modem_mem_free((qmi_get_supported_msgs_resp_v01*) rsp_msg, 
                    MODEM_MEM_CLIENT_QMI);
  }

  return response;
} /* qmi_svc_impl_get_supported_msgs_resp() */


/*=========================================================================== 
  FUNCTION QMI_SVC_IMPL_GET_SUPPORTED_FIELDS_RESP()

  DESCRIPTION 
    service supported TLV fields for request, response and indication
    messages for corresponding message id with return if it is available
 
  PARAMETERS
    sdu_in           : incoming request
    si_tbl           : pointer to the list of supported message-ids for a
                       given service
    svc_obj          : handle to the static service object

  RETURN VALUE 
    ptr to response or NULL if none.

  DEPENDENCIES 
    None 

  SIDE EFFECTS 
    None 
  =========================================================================*/ 
dsm_item_type* qmi_svc_impl_get_supported_fields_resp
(
  dsm_item_type **                  sdu_in, 
  const qmi_implemented_messages *  si_tbl, 
  qmi_idl_service_object_type       svc_obj
)
{
  dsm_item_type *                       response = NULL;
  qmi_error_type_v01                    errval = QMI_ERR_NONE_V01;
  qmi_error_type_v01                    eval = QMI_IDL_LIB_NO_ERR;
  int32                                 qmi_result = 0;
  uint32                                rsp_enc_len = 0;
  uint32                                req_buf_len = 0;
  uint32                                req_msg_len = 0;
  uint32                                rsp_buf_len = 0;
  uint32                                rsp_msg_len = 0;
  uint8 *                               rsp_buf = NULL;
  uint8 *                               req_buf = NULL;
  qmi_get_supported_fields_resp_v01 *   rsp_msg = NULL;
  qmi_get_supported_fields_req_v01 *    req_msg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-------------------------------------------------------------------------
      Accessor function for getting the maximum message length of a request
      message for QMI_GET_SUPPORTED_FIELDS_ID. Message length includes c
      struct size, size of type and size of length.
    --------------------------------------------------------------------------*/
    eval = qmi_idl_get_max_message_len(svc_obj, 
                                       QMI_IDL_REQUEST, 
                                       QMI_GET_SUPPORTED_FIELDS_ID, 
                                       &req_buf_len); 
    if ( eval != QMI_IDL_LIB_NO_ERR )
    {
      errval = QMI_ERR_INTERNAL_V01;
      break;
    }

    /*-------------------------------------------------------------------------
      Accessor function for getting the c struct size of a request message
      for QMI_GET_SUPPORTED_FIELDS_ID.
    --------------------------------------------------------------------------*/
    eval = qmi_idl_get_message_c_struct_len(svc_obj, 
                                            QMI_IDL_REQUEST, 
                                            QMI_GET_SUPPORTED_FIELDS_ID, 
                                            &req_msg_len);
    if ( eval != QMI_IDL_LIB_NO_ERR )
    {
      errval = QMI_ERR_INTERNAL_V01;
      break;
    }

    /*-------------------------------------------------------------------------
      Accessor function for getting the maximum message length of a response
      message for QMI_GET_SUPPORTED_FIELDS_ID. Message length includes c
      struct size, size of type and size of length.
    --------------------------------------------------------------------------*/
    eval = qmi_idl_get_max_message_len(svc_obj, 
                                       QMI_IDL_RESPONSE, 
                                       QMI_GET_SUPPORTED_FIELDS_ID, 
                                       &rsp_buf_len);
    if ( eval != QMI_IDL_LIB_NO_ERR )
    {
      errval = QMI_ERR_INTERNAL_V01;
      break;
    }

    /*-------------------------------------------------------------------------
      Accessor function for getting the c struct size of a response message
      for QMI_GET_SUPPORTED_FIELDS_ID.
    --------------------------------------------------------------------------*/
    eval = qmi_idl_get_message_c_struct_len(svc_obj, 
                                            QMI_IDL_RESPONSE, 
                                            QMI_GET_SUPPORTED_FIELDS_ID, 
                                            &rsp_msg_len);
    if ( eval != QMI_IDL_LIB_NO_ERR )
    {
      errval = QMI_ERR_INTERNAL_V01;
      break;
    }
  } while ( 0 );

  if ( errval == QMI_ERR_NONE_V01 )
  {
    if ( !(rsp_buf = (uint8 *)
         modem_mem_calloc(1, rsp_buf_len, MODEM_MEM_CLIENT_QMI)) ||
         !(req_buf = (uint8 *)
         modem_mem_calloc(1, req_buf_len, MODEM_MEM_CLIENT_QMI)) ||
         !(rsp_msg = (qmi_get_supported_fields_resp_v01 *)
         modem_mem_calloc(1, rsp_msg_len, MODEM_MEM_CLIENT_QMI)) ||
         !(req_msg = (qmi_get_supported_fields_req_v01 *)
         modem_mem_calloc(1, req_msg_len, MODEM_MEM_CLIENT_QMI)) )
    {
      LOG_MSG_ERROR_1( "Not enough memory to allocate for "
                       "QMI_GET_SUPPORTED_FIELDS, MsgID:0x%x",
                       QMI_GET_SUPPORTED_FIELDS_ID );
      errval = QMI_ERR_NO_MEMORY_V01;
      goto sendresponse;
    }
  }

  /*-------------------------------------------------------------------------
    Transfers data from the beginning of DSM chain into a buffer
  -------------------------------------------------------------------------*/
  if ( ( errval == QMI_ERR_NONE_V01 ) && ( req_buf_len != 
                                           PS_UTILS_DSM_PULLUP(sdu_in, 
                                           &req_buf[0], 
                                           (uint16)req_buf_len) ) )
  {
    errval = QMI_ERR_MALFORMED_MSG_V01;
  }

  if ( errval == QMI_ERR_NONE_V01 )
  {
    /*-----------------------------------------------------------------------
      Decodes a QMI message body from the wire format to the C structure
    -----------------------------------------------------------------------*/
    qmi_result = qmi_idl_message_decode(svc_obj,
                                        QMI_IDL_REQUEST,
                                        QMI_GET_SUPPORTED_FIELDS_ID,
                                        req_buf,
                                        req_buf_len,
                                        req_msg,
                                        req_msg_len);
    if ( 0 != qmi_result )
    {
      LOG_MSG_ERROR_1( "Message Decode Failed for QMI_GET_SUPPORTED_FIELDS,"
                       "MsgID:0x%x", QMI_GET_SUPPORTED_FIELDS_ID );
      errval = QMI_ERR_MALFORMED_MSG_V01;
    } 
  }  

  if ( errval == QMI_ERR_NONE_V01 )
  {
    /*-----------------------------------------------------------------------
      Generic function to load the implementation details in rsp_msg struct
    -----------------------------------------------------------------------*/
    if( qmi_si_get_supported_fields_resp(si_tbl, 
                                         req_msg->msg_id, 
                                         rsp_msg) != QMI_SI_SUCCESS )
    {
      errval = QMI_ERR_INFO_UNAVAILABLE_V01;
    }
  }

  /*-------------------------------------------------------------------------
    If qmi_si_get_supported_fields_resp() completes without error, it also
    fills in the rsp_msg->resp fields. Otherwise they are filled in
    here with the appropriate error value.
  -------------------------------------------------------------------------*/
  if ( errval != QMI_ERR_NONE_V01 )
  {
    if(rsp_msg != NULL)
    {
      rsp_msg->resp.error = errval;
      rsp_msg->resp.result = QMI_RESULT_FAILURE_V01;
    }
  }

  if (errval == QMI_ERR_NONE_V01)
    {
    /*-------------------------------------------------------------------------
      Encodes the body of a QMI message from the C data structure to the wire
      format
    -------------------------------------------------------------------------*/
    if ( QMI_IDL_LIB_NO_ERR != qmi_idl_message_encode(svc_obj,
                                                   QMI_IDL_RESPONSE,
                                                   QMI_GET_SUPPORTED_FIELDS_ID, 
                                                   rsp_msg,
                                                   rsp_msg_len,
                                                   rsp_buf,
                                                   rsp_buf_len,
                                                   &rsp_enc_len) )
    {
      LOG_MSG_ERROR_2( "Message Encode Failed for QMI_GET_SUPPORTED_FIELDS,"
                       "MsgID:0x%x, Msg:0x%x",
                       QMI_GET_SUPPORTED_FIELDS_ID, &rsp_msg );
      rsp_enc_len = 0;
    }

    /*-------------------------------------------------------------------------
      Inserts the specified amount of data in the dsm_item_type pointer from
      the buffer
    -------------------------------------------------------------------------*/
    if ( rsp_enc_len != PS_UTILS_DSM_PUSHDOWN_PACKED(&response,
                                            &rsp_buf[0],
                                           (uint16)rsp_enc_len,
                                            DSM_DS_SMALL_ITEM_POOL) )
    {
      dsm_free_packet(&response);
    }
  }

  sendresponse:

  if(req_buf != NULL)
  {
     modem_mem_free((uint8*) req_buf, MODEM_MEM_CLIENT_QMI);
  }

  if(rsp_buf != NULL)
  {
     modem_mem_free((uint8*) rsp_buf, MODEM_MEM_CLIENT_QMI);
  }
  if(req_msg != NULL)
  {
     modem_mem_free((qmi_get_supported_fields_req_v01*) req_msg, 
                    MODEM_MEM_CLIENT_QMI);
  }

  if(rsp_msg != NULL)
  {
     modem_mem_free((qmi_get_supported_fields_resp_v01*) rsp_msg, 
                    MODEM_MEM_CLIENT_QMI);
  }
 
  return response;
} /* qmi_svc_impl_get_supported_fields_resp() */

