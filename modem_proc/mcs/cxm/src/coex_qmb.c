/*!
  @file
  coex_qmb.c

  @brief
  Coexistance service using COEX IDL v2 over QMB (with transport layer 
  WCI-2 type 2)

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/coex_qmb.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/14/14   btl     Replace QMB with WCI2 service
03/18/14   btl     Add LTE SINR metrics for QMB, deferred push/pop
07/25/13   btl     Initial version

==========================================================================*/

/*=============================================================================

                      INCLUDES

=============================================================================*/
#include <stringl.h>
#include <queue.h>
#include <qmi_idl_lib.h>
#include <wci2_core.h>
#include <wci2_client.h>
#include <wci2_frame.h>
#include <coexistence_service_v02.h>

#include "cxm_utils.h"
#include "coex_algos_v2.h"

/* union of all possible messages we should receive. Used to determine
 * max msg size for rcv queue */
typedef union
{
  qmi_coex_tech_sync_req_msg_v02              sync_req;
  qmi_coex_tech_sync_resp_msg_v02             sync_resp;
  qmi_coex_tech_state_update_req_msg_v02      update_req;
  qmi_coex_tech_sync_update_ind_msg_v02       update_ind;
  qmi_coex_wlan_wifi_state_ind_msg_v02        wifi_state;
  qmi_coex_metrics_lte_sinr_start_ind_msg_v02 lte_sinr_start;
  qmi_coex_metrics_lte_sinr_read_req_msg_v02  lte_sinr_read_req;
  qmi_coex_metrics_lte_sinr_stop_ind_msg_v02  lte_sinr_stop;
  qmi_coex_conflict_params_req_msg_v02        params_req;

} coex_qmb_msg_u;

/*=============================================================================

                      DEFINES, TYPES, & CONSTANTS

=============================================================================*/
/* Maximum number of outstanding delayed/deferred QMB requests */
#define COEX_QMB_MAX_DEFERRED_REQS_ALLOWED   10

/* Max number of type2 RX frames we can have queued before processing */
#define COEX_QMB_MAX_RX_BUFF 20

/*!< CxM Connection Manager's outstanding deferred/delayed REQ structure */
typedef struct {
  boolean      is_valid; /* Validity check */
  unsigned int msg_id;   /* Message Id (QMB message ID) */       
} coex_qmb_deferred_req_s;

/* struct for queueing received frames into RX queue to be processed
 * later by CXM task */
typedef struct
{
  q_link_type      qlink;
  wci2_t2_dyn_msg *ptr;
} coex_qmb_rx_frame_s;

/* receive callback prototype */
void coex_qmb_recv_cb( wci2_t2_dyn_msg *msg_ptr );

/*=============================================================================

                      INTERNAL GLOBAL VARIABLES

=============================================================================*/
/* WCI2 client handle for coex_algos_v2 */
static uint32 coex_qmb_wci2_handle;

/* qmb message recv queue */
static q_type coex_qmb_rx_q;

/* deferred req storrage */
static coex_qmb_deferred_req_s coex_qmb_deferred_reqs[COEX_QMB_MAX_DEFERRED_REQS_ALLOWED];

/* coex_algos_v2 QMI IDL service object */
static qmi_idl_service_object_type coex_qmb_service_object;

/* type 2 message holding buffer */
static coex_qmb_rx_frame_s rx_msgs[COEX_QMB_MAX_RX_BUFF];

/*=============================================================================

                      FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  coex_qmb_encode

=============================================================================*/
/*!
    @brief
    Encode the msg using QMI IDL Encode

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_encode(
  unsigned int  msg_id,
  void         *c_struct,
  unsigned int  c_struct_len,
  void         *out_buf,
  unsigned int  out_buf_len,
  unsigned int *out_enc_len    /* length of encoded msg */
)
{
  uint32   idl_c_struct_len = 0;
  uint16   packed_msg_id = (uint16)msg_id;
  uint32   max_msg_len;
  uint32_t len;
  uint8   *out_byte_buf = (uint8*)out_buf;
  int      enc_rc;
  /*-----------------------------------------------------------------------*/
  enc_rc = qmi_idl_get_message_c_struct_len( coex_qmb_service_object, 
                                             QMI_IDL_INDICATION, 
                                             msg_id, &idl_c_struct_len );
  CXM_ASSERT( enc_rc == QMI_IDL_LIB_NO_ERR && 
              idl_c_struct_len == c_struct_len );

  enc_rc = qmi_idl_get_max_message_len( coex_qmb_service_object, 
                                        QMI_IDL_INDICATION, 
                                        msg_id, &max_msg_len );
  CXM_ASSERT( enc_rc == QMI_IDL_LIB_NO_ERR );

  /* copy msg ID to 2B before encoded msg */
  memscpy( out_byte_buf, out_buf_len, &packed_msg_id, 2 );

  /* QMI encode into rest of buffer */
  enc_rc = qmi_idl_message_encode(
                  coex_qmb_service_object,
                  QMI_IDL_INDICATION,
                  msg_id,
                  c_struct,
                  c_struct_len,
                  out_byte_buf + 2,
                  max_msg_len,
                  &len);
  CXM_ASSERT( enc_rc == QMI_IDL_LIB_NO_ERR );
  *out_enc_len = len + 2;
  CXM_ASSERT( *out_enc_len <= out_buf_len );

  return;
}

/*=============================================================================

  FUNCTION:  coex_qmb_decode

=============================================================================*/
/*!
    @brief
    QMI Decode on message received from CXM uart

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_decode(
  void         *buf,         /* IN: input buffer (encoded msg) */
  unsigned int  len,         /* IN: length of encoded msg */
  unsigned int *out_msg_id,  /* OUT: msg id */
  void         *out_buf,     /* OUT: will be filled w/ decoded msg */
  unsigned int  out_buf_len, /* IN: length of output buffer */
  unsigned int *decoded_len  /* OUT: length of decoded msg */
)
{
  uint16_t msg_id;
  int      enc_rc;
  uint32_t c_struct_len;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( len >= 2 && NULL != buf && NULL != out_msg_id && 
              NULL != out_buf && 0 != out_buf_len );

  /* first two bytes of wireformat are msg id */
  memscpy( &msg_id, sizeof(msg_id), buf, 2 );
  len -= 2;
  enc_rc = qmi_idl_get_message_c_struct_len( coex_qmb_service_object,
                                             QMI_IDL_INDICATION,
                                             msg_id, &c_struct_len );
  CXM_ASSERT( enc_rc == QMI_IDL_LIB_NO_ERR && 
              out_buf_len >= c_struct_len );
  enc_rc = qmi_idl_message_decode(
                    coex_qmb_service_object,
                    QMI_IDL_INDICATION,
                    (unsigned int)msg_id,
                    ((uint8_t *)buf) + 2,
                    len,
                    out_buf,
                    c_struct_len );
  CXM_ASSERT( enc_rc == QMI_IDL_LIB_NO_ERR );
  *out_msg_id = msg_id;
  *decoded_len = c_struct_len;

  return;
}

/*=============================================================================

  FUNCTION:  coex_qmb_init

=============================================================================*/
/*!
    @brief
    Register this service as a client with QMB to open a connection to 
    the WCI2 UART transport

    @return
    wci2 client handle
*/
/*===========================================================================*/
uint32 coex_qmb_init( void )
{
  uint8              i;
  wci2_error_e       retval;
  wci2_client_open_s params = {
    WCI2_UART_BAUD_3000000,
    WCI2_CLIENT_MODE_WCI2,
    WCI2_REG_TYPE_2,
    WCI2_FRAME_MODE_HDLC,
    NULL, /* basic_rx_cb */
    NULL, /* dir_rx_cb */
    coex_qmb_recv_cb/* t2_rx_cb */
  };
  /*-----------------------------------------------------------------------*/
  /* get IDL service object from COEX IDL */
  coex_qmb_service_object = coex_get_service_object_v02();

  /* initialize the rx queue for t2 messages */
  q_init( &coex_qmb_rx_q );
  for( i = 0; i < COEX_QMB_MAX_RX_BUFF; i++ )
  {
    rx_msgs[i].ptr = NULL;
  }

  /* create client with wci2 to send/recv t2 over UART */
  retval = wci2_client_register( &params, &coex_qmb_wci2_handle );
  CXM_ASSERT( retval == WCI2_E_SUCCESS );

  return coex_qmb_wci2_handle;
}

/*=============================================================================

  FUNCTION:  coex_qmb_deinit

=============================================================================*/
/*!
    @brief
    Close connection to QMI MB and release handle

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_deinit( void )
{
  uint8 i;
  /*-----------------------------------------------------------------------*/
  wci2_client_deregister( coex_qmb_wci2_handle );

  /* free any still-queued rx msgs */
  for( i = 0; i < COEX_QMB_MAX_RX_BUFF; i++ )
  {
    if( rx_msgs[i].ptr != NULL )
    {
      wci2_client_free_t2( rx_msgs[i].ptr );
    }
  }
  q_destroy( &coex_qmb_rx_q );

  return;
}

/*=============================================================================

  FUNCTION:  coex_qmb_recv_cb

=============================================================================*/
/*!
    @brief
    Receive a type2 frame from wci2 CXM_UART

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_recv_cb( wci2_t2_dyn_msg *msg_ptr )
{
  static unsigned int drop_count = 0;
  unsigned int        i;
  /*-----------------------------------------------------------------------*/
  /* find empty queue item from static pool */
  for( i = 0; i < COEX_QMB_MAX_RX_BUFF; i++ )
  {
    if( rx_msgs[i].ptr == NULL )
    {
      /* queue message up for processing by task later */
      q_link( &rx_msgs[i], &rx_msgs[i].qlink );
      rx_msgs[i].ptr = msg_ptr;
      q_put( &coex_qmb_rx_q, &rx_msgs[i].qlink );

      /* signal task for processing */
      (void) rex_set_sigs( &cxm_tcb, CXM_COEX_QMB_SIG );
      break;
    }
  }

  if( i == COEX_QMB_MAX_RX_BUFF )
  {
    /* we dropped a message! couldn't find an empty slot */
    drop_count++;
    CXM_MSG_1( ERROR, "QMB RX buff full, dropped frame. Cnt=%d", drop_count );
    wci2_client_free_t2( msg_ptr );
  }

  return;
}

/*=============================================================================

  FUNCTION:  coex_qmb_send_msg

=============================================================================*/
/*!
    @brief
    Send message using WCI2 type2 HDLC framing

    @detail
    Broadcasts to the bus. At this point the bus is a UART with one endpoint,
    so this is the same as sending a message to one recipient (doesn't 
    support addressing ). QMI will encode for us.

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_publish_msg( unsigned int msg_id, void* msg, unsigned int len )
{
  /* need extra 2B for msg id */
  uint8        enc_buff[sizeof(coex_qmb_msg_u) + 2];
  unsigned int enc_buff_len = sizeof(coex_qmb_msg_u) + 2;
  unsigned int enc_len = 0;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( LOW, "QMB: Sending msg id=0x%x", msg_id );
  coex_qmb_encode( msg_id, msg, len, enc_buff, enc_buff_len, &enc_len );
  wci2_client_t2_frame_and_send( 
      coex_qmb_wci2_handle,
      enc_buff, 
      enc_len, 
      WCI2_TYPE2_ADDRESS_QMB_COEX );

  return;
}

/*=============================================================================

  FUNCTION:  coex_qmb_push_deferred_req

=============================================================================*/
/*!
    @brief
    Helper API to push delayed/deferred QMB request onto the publisher's stack

    @return
    void
*/
/*===========================================================================*/
errno_enum_type coex_qmb_push_deferred_req (
  uint32           *deferred_req_id,   /*!< Deferred Request ID*/
  unsigned int      msg_id             /*!< Message Id */
)
{
  errno_enum_type retval = E_SUCCESS;
  uint32          loop_counter = 0;
  /*-----------------------------------------------------------------------*/
  if( NULL == deferred_req_id )
  {
    retval = E_INVALID_ARG;
  }
  else
  {
    /* Look for empty/available slot in the service's outstanding deferred/delayed
       QMI request table */
    for( loop_counter = 0; loop_counter < COEX_QMB_MAX_DEFERRED_REQS_ALLOWED; ++loop_counter )
    {
      if( FALSE == coex_qmb_deferred_reqs[loop_counter].is_valid )
      {
        break;
      }
    }

    /* Check to see if space is available in the table */
    CXM_ASSERT( COEX_QMB_MAX_DEFERRED_REQS_ALLOWED > loop_counter );

    /* Save details of the current deferred/delayed QMI request */
    coex_qmb_deferred_reqs[loop_counter].is_valid = TRUE;
    coex_qmb_deferred_reqs[loop_counter].msg_id = msg_id;

    /* Update requester with the 'ID' */
    *deferred_req_id = loop_counter;
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_qmb_pop_deferred_req

=============================================================================*/
/*!
    @brief
    Helper API to pop delayed/deferred QMB request from the publisher's stack

    @return
    void
*/
/*===========================================================================*/
errno_enum_type coex_qmb_pop_deferred_req (
  uint32            deferred_req_id,   /*!< Deferred Request ID*/
  unsigned int      msg_id             /*!< Message Id */
)
{
  errno_enum_type retval = E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  if( COEX_QMB_MAX_DEFERRED_REQS_ALLOWED <= deferred_req_id )
  {
    retval = E_INVALID_ARG;
  }
  else
  {
    /* Validate the requested 'deferred_req_id' from the table */
    if( TRUE == coex_qmb_deferred_reqs[deferred_req_id].is_valid &&
        msg_id == coex_qmb_deferred_reqs[deferred_req_id].msg_id )
    {
      /* Invalidate/Empty the slot in the table for reuse later */
      coex_qmb_deferred_reqs[deferred_req_id].is_valid = FALSE;
    }
    else
    {
      /* No entry found for the deferred/delayed REQ requested here */
      retval = E_FAILURE;
    }
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_qmb_process_sig

=============================================================================*/
/*!
    @brief
    Get a message off the queue/buffer and process it/route it appropriately

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_process_sig( void )
{
  unsigned int         msg_id;
  wci2_t2_dyn_msg     *frame_ptr;
  coex_qmb_rx_frame_s *q_item;
  uint8               *enc_msg_ptr;
  uint16               enc_msg_len;
  uint16               addr;
  coex_qmb_msg_u       msg;
  void                *msg_ptr = (void*)&msg;
  unsigned int         msg_len = sizeof(msg);
  unsigned int         c_struct_len = 0;
  wci2_error_e         unframe_retval;
  /*-----------------------------------------------------------------------*/
  /* pull received messages off queue and process them */
  while( NULL != (q_item = (coex_qmb_rx_frame_s*) q_get( &coex_qmb_rx_q )) )
  {
    /* unqueue msg, attempt to unframe. If valid, process */
    frame_ptr = q_item->ptr;
    unframe_retval = wci2_hdlc_unframe( frame_ptr->buff, frame_ptr->len, 
                                        &enc_msg_ptr, &enc_msg_len, &addr );
    if( addr == WCI2_TYPE2_ADDRESS_QMB_COEX && 
        unframe_retval == WCI2_E_SUCCESS )
    {
      /* if address and unframe good, unencode the msg. */
      coex_qmb_decode( (void*)enc_msg_ptr, (unsigned int)enc_msg_len, 
                       &msg_id, msg_ptr, msg_len, &c_struct_len );

      /* finally, process the decoded msg */
      CXM_MSG_2( LOW, "QMB: received MSG ID 0x%x len=%d", msg_id, c_struct_len );
      switch( msg_id )
      {
        #ifndef REMOVE_QMI_COEX_TECH_SYNC_UPDATE_IND_V02
        case QMI_COEX_TECH_SYNC_UPDATE_IND_V02:
          coex_handle_sync_update_ind_v02( msg_ptr, msg_len );
          break;
        #endif /* REMOVE_QMI_COEX_TECH_SYNC_UPDATE_IND_V02 */

        #ifndef REMOVE_QMI_COEX_TECH_SYNC_REQ_V02
        case QMI_COEX_TECH_SYNC_REQ_V02:
          coex_handle_sync_state_req_v02( msg_ptr, msg_len );
          break;
        #endif /* REMOVE_QMI_COEX_TECH_SYNC_REQ_V02 */

        #ifndef REMOVE_QMI_COEX_TECH_SYNC_RESP_V02
        case QMI_COEX_TECH_SYNC_RESP_V02:
          coex_handle_sync_state_resp_v02( msg_ptr, msg_len );
          break;
        #endif /* REMOVE_QMI_COEX_TECH_SYNC_RESP_V02 */

        #ifndef REMOVE_QMI_COEX_TECH_STATE_UPDATE_REQ_V02
        case QMI_COEX_TECH_STATE_UPDATE_REQ_V02:
          coex_handle_update_req_v02( msg_ptr, msg_len );
          break;
        #endif /* REMOVE_QMI_COEX_TECH_STATE_UPDATE_REQ_V02 */

        #ifndef REMOVE_QMI_COEX_WLAN_WIFI_STATE_IND_V02
        case QMI_COEX_WLAN_WIFI_STATE_IND_V02:
          coex_handle_wlan_wifi_state_ind_v02( msg_ptr, msg_len );
          break;
        #endif /* REMOVE_QMI_COEX_WLAN_WIFI_STATE_IND_V02 */

        #ifndef REMOVE_QMI_COEX_METRICS_LTE_SINR_START_IND_V02
        case QMI_COEX_METRICS_LTE_SINR_START_IND_V02:
          coex_handle_metrics_lte_sinr_start_ind_v02( msg_ptr, msg_len );
          break;
        #endif /* REMOVE_QMI_COEX_METRICS_LTE_SINR_START_IND_V02 */

        #ifndef REMOVE_QMI_COEX_METRICS_LTE_SINR_READ_REQ_V02
        case QMI_COEX_METRICS_LTE_SINR_READ_REQ_V02:
          coex_handle_metrics_lte_sinr_read_req_v02( msg_ptr, msg_len );
          break;
        #endif /* REMOVE_QMI_COEX_METRICS_LTE_SINR_READ_REQ_V02 */

        #ifndef REMOVE_QMI_COEX_METRICS_LTE_SINR_STOP_IND_V02
        case QMI_COEX_METRICS_LTE_SINR_STOP_IND_V02:
          coex_handle_metrics_lte_sinr_stop_ind_v02( msg_ptr, msg_len );
          break;
        #endif /* REMOVE_QMI_COEX_METRICS_LTE_SINR_STOP_IND_V02 */

        #ifndef REMOVE_QMI_COEX_CONFLICT_PARAMS_REQ_V02
        case QMI_COEX_CONFLICT_PARAMS_REQ_V02:
          coex_handle_conflict_params_req_v02( msg_ptr, msg_len );
          break;
        #endif /* REMOVE_QMI_COEX_CONFLICT_PARAMS_REQ_V02 */

        default:
          CXM_MSG_1( ERROR, "QMB: unrecognized msg id 0x%x", msg_id );
          break;
      }
    }
    else
    {
      /* unframe unsuccessful or address match */
      CXM_MSG_2( ERROR, "Drop bad frame, retval=%d addr=%d", unframe_retval, addr );
    }

    /* free the message when done processing */
    wci2_client_free_t2( frame_ptr );
    q_item->ptr = NULL;
  }

  return;
}

