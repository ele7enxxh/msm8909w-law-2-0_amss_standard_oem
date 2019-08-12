#ifndef QMI_UIM_HTTP_H
#define QMI_UIM_HTTP_H

/*===========================================================================

                         Q M I _ U I M _ H T T P . H

DESCRIPTION

 This header file defines data types and functions necessary to access a server
 accessible from a remote device such as an applications processor or another modem
 via the QMI interface.

Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/inc/qmi_uim_http.h#1 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/19/16    ar     Fix to support bigger profile (> 65535) download
05/17/16    bcho   Deregister qmi uim http service on task stop
04/13/16    ks     Initial version of QMI UIM HTTP

===========================================================================*/

/*=============================================================================

                   I N C L U D E S

=============================================================================*/
#include "comdef.h"
#include "qmi_csi.h"
#include "qmi_sap.h"
#include "queue.h"
#include "user_identity_module_http_v01.h"

/*=============================================================================

                   E N U M E R A T E D   D A T A

=============================================================================*/

/*===========================================================================
   ENUM:      QMI_UIM_HTTP_RESULT_TYPE
=============================================================================*/
typedef enum
{
  QMI_UIM_HTTP_SUCCESS    = 0x0,
  QMI_UIM_HTTP_ERROR,
} qmi_uim_http_result_type;

/*===========================================================================
   ENUM:      QMI_UIM_HTTP_TRANSACTION_RESULT_TYPE
=============================================================================*/
typedef enum
{
  QMI_UIM_HTTP_TRANSACTION_SUCCESS    = 0x0,
  QMI_UIM_HTTP_UNKNOWN_ERROR,
  QMI_UIM_HTTP_SERVER_ERROR,
  QMI_UIM_HTTP_TLS_ERROR,
  QMI_UIM_HTTP_NETWORK_ERROR
} qmi_uim_http_transaction_result_type;

/*===========================================================================
   ENUM:      QMI_UIM_HTTP_SERVICE_REGISTRY_ID_TYPE
=============================================================================*/
/* This service ID corresponds to the clients/tasks in modem that would register
   to use QMI UIM HTTP service. 
   Eg - LPA/eUICC for slot1 could register and get QMI_UIM_HTTP_SERVICE_ID_1 
        LPA/eUICC for slot2 could register and get QMI_UIM_HTTP_SERVICE_ID_2 */
typedef enum {
  QMI_UIM_HTTP_SERVICE_ID_NOT_APPLICABLE    = 0x0, /**< Not applicable \n */
  QMI_UIM_HTTP_SERVICE_ID_1                 = 0x1, /**< Service ID 1 \n */
  QMI_UIM_HTTP_SERVICE_ID_2                 = 0x2, /**< Service ID 2 \n */
  QMI_UIM_HTTP_SERVICE_ID_3                 = 0x3, /**< Service ID 3 \n */
  QMI_UIM_HTTP_SERVICE_ID_4                 = 0x4, /**< Service ID 4 \n */
  QMI_UIM_HTTP_SERVICE_ID_5                 = 0x5, /**< Service ID 5 \n */
  QMI_UIM_HTTP_MAX_SERVICE_ID               = 0xFF /**< Service ID MAX \n */
} qmi_uim_http_service_registry_id_type;

/*=============================================================================

      S T R U C T    A N D    C A L L B A C K   D A T A    T Y P E S

=============================================================================*/

/*===========================================================================
   STRUCTURE:      QMI_UIM_HTTP_TRANSACTION_HEADER_TYPE
=============================================================================*/
/* Custom headers that can be specified by the client */
typedef struct
{
  char * name_ptr;
  char * value_ptr;
} qmi_uim_http_transaction_header_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_HTTP_TRANSACTION_RESP_DATA_TYPE
=============================================================================*/
typedef struct
{
  qmi_uim_http_transaction_result_type      result;
  void *                                    client_ref_ptr;
  uint32                                    payload_len;
  uint8 *                                   payload_ptr;
  uint16                                    num_cust_headers;
  qmi_uim_http_transaction_header_type      headers[UIM_HTTP_CUST_HEADER_MAX_COUNT_V01];
} qmi_uim_http_transaction_resp_data_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_HTTP_TRANSACTION_HEADER_INFO_TYPE
=============================================================================*/
/* HTTP header that needs to be specified by modem client */
typedef struct
{
  char                                   content_type[UIM_HTTP_HEADER_VALUE_MAX_LEN_V01+1];
  uint16                                 num_headers;
  qmi_uim_http_transaction_header_type   headers[UIM_HTTP_CUST_HEADER_MAX_COUNT_V01];
} qmi_uim_http_transaction_header_info_type;

/*===========================================================================
   FUNCTION PROTOTYPE:      QMI_UIM_HTTP_TRASACTION_CALLBACK_TYPE
=============================================================================*/
typedef void qmi_uim_http_transaction_callback_type
(
  const qmi_uim_http_transaction_resp_data_type    * rsp_ptr
);

/*===========================================================================
   STRUCTURE:      QMI_UIM_HTTP_TRANSACTION_REQ_DATA_TYPE
=============================================================================*/
typedef struct
{
  void *                                      client_ref_ptr;
  qmi_uim_http_service_registry_id_type       service_id;
  char *                                      url_ptr; 
  qmi_uim_http_transaction_header_info_type  *header_ptr;
  uint32                                      payload_len;
  uint8*                                      payload_ptr;
  qmi_uim_http_transaction_callback_type     *cb_ptr;
} qmi_uim_http_transaction_req_data_type;

/*=============================================================================

                   F U N C T I O N S

=============================================================================*/

/*===========================================================================
  FUNCTION QMI_UIM_HTTP_SERVICE_INIT

  DESCRIPTION
    Called to start the QMI UIM HTTP service when the LPA modem task is up

  PARAMETERS
    TCB and SIG

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_http_service_init(
  rex_tcb_type    * tcb_ptr,
  rex_sigs_type     sig
);

/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_SERVICE_REGISTER

   DESCRIPTION:
     This function registers the calling task with the QMI UIM HTTP service.
     The function provides/fills the service_id in the input parameter received.
     This id needs to passed for every send http request.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_http_result_type qmi_uim_http_service_register (
  qmi_uim_http_service_registry_id_type    *service_id
);

/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_SERVICE_RELEASE

   DESCRIPTION:
     This function releases the calling task(service ID) with the QMI UIM HTTP service.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_http_result_type qmi_uim_http_service_release (
  qmi_uim_http_service_registry_id_type    service_id
);

/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_SEND_TRANSACTION_REQUEST

   DESCRIPTION:
     This function transmits a http request from the calling task to the server.
     For each request, the calling task can indicate a response callback,
     used to receive the response from the server.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
qmi_uim_http_result_type qmi_uim_http_send_transaction_request (
  const qmi_uim_http_transaction_req_data_type  * qmi_uim_http_transaction_ptr
);

/*===========================================================================
   FUNCTION:      QMI_UIM_HTTP_CMD_Q_SIG_HANDLER

  DESCRIPTION:
    QMI UIM HTTP CMD Q signal handler

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     Returns TRUE if command queue is empty
     FALSE if there are more commands to process

   SIDE EFFECTS:
     None
===========================================================================*/
boolean qmi_uim_http_cmd_q_sig_handler (
  void
);

/*===========================================================================
  FUNCTION QMI_UIM_HTTP_HANDLE_QCSI_SIG

  DESCRIPTION
    Function is called when a QCSI signal is received by the QMI UIM HTTP
    service. It will result in the calling of one of the QMI UIM HTTP
    request handlers.

  PARAMETERS
    tcb
    sig

  RETURN VALUE
    int

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_http_handle_qcsi_sig (
   rex_tcb_type *tcb, rex_sigs_type sig
);

/*=========================================================================== 
  FUNCTION QMI_UIM_HTTP_DEREG 
  
  DESCRIPTION 
    Called to deregister the QMI UIM http as QMI service and as SAP client on 
    reception of task stop signal 
  
  PARAMETERS 
    None 
 
  RETURN VALUE 
    None 
  
  DEPENDENCIES 
    None 
  
  SIDE EFFECTS 
    None 
===========================================================================*/ 
void qmi_uim_http_dereg( 
  void 
); 
#endif /* QMI_UIM_HTTP_H */
