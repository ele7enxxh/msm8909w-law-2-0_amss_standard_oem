/*---------------------------------------------------------------------------
   qmi_si_impl.c - QMI Supported Interfaces Implementation Details
---------------------------------------------------------------------------*/
/*****************************************************************************
  @file    qmi_si_impl.c
  @brief   The QMI Supported Interfaces Implementation Details
 
  DESCRIPTION
  This is the module which defines the interface for the Application Processor
  to retrieve the implementation details of a service on the MPSS
  (Modem Processor Sub-System).

  ---------------------------------------------------------------------------
  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/mproc/qmi/qmi_si/qmi_si_impl.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/10/12    pb     Support for QMI messages to retrieve implementation details 
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "qmi_si.h"
#include <string.h>
#include "common_v01.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------*//**
@brief
  MACRO definition used while checking the TLV fields of request, response and 
  indication for a specific message-id
*//* ------------------------------------------------------------------------*/
#define QMI_SI_BIT_SET(val, bit)   (((val) & (1 << (bit))) ? 1 : 0)

/* -----------------------------------------------------------------------*//**
@brief
  MACRO definition used for setting the corresponding bit of an uint8_t 
  bitmapped array entry
*//* ------------------------------------------------------------------------*/
#define QMI_SI_BIT8_SET(x, bit)     ((x) |= QMI_SI_BIT8_MASK(bit))

/* -----------------------------------------------------------------------*//**
@brief
  MACRO definition which generates the bitmask of 8 bits
*//* ------------------------------------------------------------------------*/
#define QMI_SI_BIT8_MASK(bit)       (1 << (bit % 8 ))  

/*---------------------------------------------------------------------------
  Macro used for retrieving implementation details.
---------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------*//**
@brief
  MACRO definitions used while retrieving the supported TLV fields of request, 
  response and indication for a specific message-id  
*//* ------------------------------------------------------------------------*/
#define TLV_REQUEST               0
#define TLV_RESPONSE              1
#define TLV_INDICATION            2

/*===========================================================================
  FUNCTION  qmi_si_get_supported_msgs_resp()
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Writes all the message-ids supported by a service in 
  qmi_get_supported_msgs_resp_v01 struct

@param[in]      si_tbl                pointer to the list of supported 
                                      message-ids for a given service.
@param[out]     rsp_msg               outgoing response.

@retval         QMI_SI_SUCCESS        Success
@retval         QMI_SI_INTERNAL_ERR   Unknown failure
*//* ------------------------------------------------------------------------*/
qmi_si_error_type qmi_si_get_supported_msgs_resp
(
  const qmi_implemented_messages    *si_tbl,
  qmi_get_supported_msgs_resp_v01   *rsp_msg  
)
{
  qmi_error_type_v01             errval = QMI_ERR_NONE_V01;
  uint16_t                       msg_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( si_tbl != NULL ) && ( rsp_msg != NULL ) )
  {
    memset(rsp_msg, 0, sizeof(qmi_get_supported_msgs_resp_v01));
 
    rsp_msg->supported_msgs_valid = TRUE;

    rsp_msg->supported_msgs_len = ( (si_tbl->max_msg_id / 8) + 1);

    // Checking for boundary cases
    if ( rsp_msg->supported_msgs_len > QMI_GET_SUPPORTED_MESSAGES_ARRAY_MAX_V01 )
    {
      errval = QMI_ERR_INTERNAL_V01;
    }

    if ( errval == QMI_ERR_NONE_V01 )
    {     
      for ( msg_id = 0; msg_id <= si_tbl->max_msg_id; msg_id++ )
      {
        if ( si_tbl->message_array[msg_id] != NULL )
        {  
          QMI_SI_BIT8_SET(rsp_msg->supported_msgs[msg_id/8], msg_id);
        }
      }   
    }

    if (errval != QMI_ERR_NONE_V01)
    {
      /* Empty the message */
      memset(rsp_msg,0,sizeof(qmi_get_supported_fields_resp_v01));
    }
    rsp_msg->resp.error = errval;
    rsp_msg->resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
                                                       : QMI_RESULT_FAILURE_V01);

    return QMI_SI_SUCCESS;
  }
  return QMI_SI_INTERNAL_ERR;
}

/*===========================================================================
  FUNCTION  qmi_si_get_supported_fields_resp()
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  writes TLV fields for request, response and indication messages for 
  corresponding message-id in qmi_get_supported_fields_resp_v01 struct

@param[in]      si_tbl                   pointer to the list of supported 
                                         message-ids for a given service.
@param[in]      msg_id                   message-id requested for implementation 
                                         details.
@param[out]     rsp_msg                  outgoing response.

@retval         QMI_SI_SUCCESS           Success
@retval         QMI_SI_INTERNAL_ERR      Unknown failure
*//* ------------------------------------------------------------------------*/
qmi_si_error_type qmi_si_get_supported_fields_resp
(
  const qmi_implemented_messages      *si_tbl,
  uint16_t                            msg_id, 
  qmi_get_supported_fields_resp_v01   *rsp_msg
)
{
  qmi_error_type_v01              errval = QMI_ERR_NONE_V01;
  uint32_t                        i = 0,
                                  j = 0,
                                  *length = 0,
                                  list_entry = 0;
  uint8_t                         *valid = NULL,
                                  *value = NULL ;
                       
  const qmi_implemented_optionals *tlv_list_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( si_tbl != NULL ) && ( rsp_msg != NULL ) )
  {
    // If message-id is either not supported by the service or is out of range
    if ( ( si_tbl->message_array[msg_id] == NULL ) || ( msg_id > si_tbl->max_msg_id ) )
    {
      errval = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
    }
   
    if ( errval == QMI_ERR_NONE_V01 )
    {    
      memset(rsp_msg, 0, sizeof(qmi_get_supported_fields_resp_v01));

      /* The data structure has an array of 3 entries for each message with entry
         0=request TLVs, 1=response TLVs and 2=indication TLVs */
      for ( list_entry = TLV_REQUEST; list_entry <= TLV_INDICATION; list_entry++ ) 
      {
        switch (list_entry)
        {
          case TLV_REQUEST:
            tlv_list_ptr = si_tbl->message_array[msg_id]->req;
            valid = &rsp_msg->request_fields_valid;
            length = &rsp_msg->request_fields_len;
            value = rsp_msg->request_fields;   
            break;
      
          case TLV_RESPONSE:
            tlv_list_ptr = si_tbl->message_array[msg_id]->resp;
            valid = &rsp_msg->response_fields_valid;
            length = &rsp_msg->response_fields_len;
            value = rsp_msg->response_fields;
            break;
      
          case TLV_INDICATION:
            tlv_list_ptr = si_tbl->message_array[msg_id]->ind;
            valid = &rsp_msg->indication_fields_valid;
            length = &rsp_msg->indication_fields_len;
            value = rsp_msg->indication_fields;
            break;
      
          default:
            tlv_list_ptr = NULL;
            break;
        }
      
        if ( tlv_list_ptr != NULL )
        {
          *valid = TRUE;      

          for ( i = 0; i < tlv_list_ptr->array_size; i++ )
          {
            // TLV's are checked in a cycle of 32
            for ( j = 0; j < 32; j++ )
            {
              if ( QMI_SI_BIT_SET(tlv_list_ptr->tlv_array[i], j) )
              {
                QMI_SI_BIT8_SET(value[( ( j / 8 ) + ( 4 * i ) ) ], j);
              }    
            }
          }

          *length = ( 4 * i );
          if( *length != 0 )
          {
            // Remove leading or trailing zeros from an array
            while( *length != 0 && value[(*length)-1] == 0)
            {
              (*length)--;
            }
          }
       
          // Checking for boundary cases
          if ( *length > QMI_GET_SUPPORTED_FIELDS_ARRAY_MAX_V01 )
          {
            errval = QMI_ERR_INTERNAL_V01;            
          }
        }
      }
    }

    if (errval != QMI_ERR_NONE_V01)
    {
      /* Empty the message */
      memset(rsp_msg,0,sizeof(qmi_get_supported_fields_resp_v01));
    }
    rsp_msg->resp.error = errval;
    rsp_msg->resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
                                                       : QMI_RESULT_FAILURE_V01);
    return QMI_SI_SUCCESS; 
  }
  return QMI_SI_INTERNAL_ERR;   
}
