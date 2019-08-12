
/*============================================================================
  @file:       qmi_bit.c

  OVERVIEW:    This module is adapts the bit-api to QCSI framework. This
               file glues bit-api, enabling it to be a provider of the 
               QMI-Bit Service. It has a few sections, any changes to the 
               file should be done in the appropriate sections to maintain 
               readability of the file. 

               UTILITY FUNCTIONS
                 All utility functions that are needed shall go here
                   
               BIT-API CALLBACKS
                 All functions that are registered with the BIT-API shall be 
                 defined in this section
 
               MESSAGE HANDLERS 
                 All the messages handlers should be added to this section. 
                
               QCSI CALLBACKS
                 All functions that are registered with QCSI framework shall be
                 defined in this section
                
               EXTERNALIZED Functions
                 All functions that are exposed through the header file shall be
                 implemented here.
  
                
  DEPENDENCIES: QCSI Framework
 
  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/src/qmi_bit.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2011-03-08  spn  Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "customer.h"
#include "comdef.h"


/* Module Header include */
#include "qmi_bit.h"

/* Non-GPS Includes */
#include "dsm.h"
#include "dsmutil.h"
#include "qmi_csi_target_ext.h"
#include "qmi_csi.h"
#include "qmi_sap.h"
#include "qmi_idl_lib.h"
#include "mqcsi_log.h"
#include "msg.h"
#include "queue.h"
#include "common_v01.h"
#include "bearer_independent_transport_v01.h"


/* GPS Includes */
#include "aries_os_api.h"
#include "gpsone_bit_api.h"
#include "gnss_common.h"
#include "gps_common.h"

/*****************************************************************************
 * Preprocessor Definitions and Constants
 * **************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
  Signal that is used by the QMI Framework to notify the task in case there 
  are pending messages for the task to handle. 
*//* ------------------------------------------------------------------------*/
#define QMI_BIT_SIGNAL             C_OS_FLAG_QMI_EVENT

/* -----------------------------------------------------------------------*//**
@brief
  Maximum number of BIT-Services that can be accomodated. Use-cases of how to 
  manage multiple QMI-BIT-Daemons is not yet defined. So limiting the number of 
  BIT-Daemons to 1. 
@warning
  Increasing this number is NOT STRAIGHTFORWARD. If this is increased, we have 
  to figure out how do we arbitrate connect requests.  The framework though 
  present, some design level decisions on usage of multiple BIT-Services has 
  to be made.

  Besides, what does it mean if there are multiple services, usually there 
  is one Service and multiple Clients. 
*//* ------------------------------------------------------------------------*/
#define QMI_BIT_MAX_BIT_SERVICE             (1)

/* -----------------------------------------------------------------------*//**
@brief
  Maximum number of outstanding Transactions that can be saved.
*//* ------------------------------------------------------------------------*/
#define QMI_BIT_MAX_PENDING_TRANSACTION_IDS             (255)



/* -----------------------------------------------------------------------*//**
@brief
  Thresholds for the Watermarks that will buffer the incoming data from the 
  BIT-Service.
  Note: Each watermark corresponds to a session. 

@warning
  The thresholds should be less than the max value supported by uint32, as 
  watermark uses uint32's
*//* ------------------------------------------------------------------------*/
#define QMI_BIT_LO_WMK_THRESHOLD          (2*BIT_CONST_PAYLOAD_LEN_MAX_V01)
#define QMI_BIT_HI_WMK_THRESHOLD          (10*BIT_CONST_PAYLOAD_LEN_MAX_V01)
#define QMI_BIT_DNE_WMK_THRESHOLD         (2*QMI_BIT_HI_WMK_THRESHOLD)

/* -----------------------------------------------------------------------*//**
@brief
  Message Macros to be used in this module.
*//* ------------------------------------------------------------------------*/
#define QMI_BIT_MSG_LOW( str, a, b, c )                                          \
                  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, str,  a, b, c )
#define QMI_BIT_MSG_MED( str, a, b, c )                                          \
                  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, str, a, b, c )
#define QMI_BIT_MSG_HIGH( str, a, b, c )                                         \
                  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, str, a, b, c )
#define QMI_BIT_MSG_ERROR( str, a, b, c )                                        \
                  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, str, a, b, c )
#define QMI_BIT_ERR_FATAL( str, a, b, c )                                        \
                  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_FATAL, str, a, b, c )

          

/* -----------------------------------------------------------------------*//**
 @brief 
  Transaction IDs for messages. These are transaction IDs that are sent to the 
  client. BIT-API does not provide Transaction IDs for some messages. 
*//* ------------------------------------------------------------------------*/
#define  QMI_BIT_TRANSACTION_ID_OPEN         ( 0x01010101 )
#define  QMI_BIT_TRANSACTION_ID_CLOSE        ( 0x0F0F0F0F )
#define  QMI_BIT_TRANSACTION_ID_RTR          ( 0xF0F0F0F0 )
#define  QMI_BIT_TRANSACTION_ID_DATA_RECEIVED ( 0x10101010 )

/* -----------------------------------------------------------------------*//**
 @brief 
  The time ( in milliseconds ) after which a Connection Item can be flushed.
*//* ------------------------------------------------------------------------*/
#define  QMI_BIT_DISCONNECT_RESPONSE_TIMEOUT       ( 1000 ) /* 1Second */



/* -----------------------------------------------------------------------*//**
 @brief 
  Macro definition to set the value of a simple data type and its validity
  to TRUE. Simple macro-utility to update the QMI-Bit data structures for 
  simple data-types.
  
@param[in]  variable    the variable which needs to be set
@param[in]  value       the value that it needs to be set to
*//* ------------------------------------------------------------------------*/
#define QMI_BIT_VARIABLE_VALID(variable) variable ## _valid
#define QMI_BIT_SET_OPTIONAL_VALUE(variable, value)                               \
           QMI_BIT_VARIABLE_VALID(variable) = 1;                      \
           (variable)= (value)


/*****************************************************************************
 * Type Declarations
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
 @brief 
  Generic Structure to pass around Message information.
  
@warning
  The memory for p_Msg needs to be allocated by the caller. The caller will 
  need to free the memory. 
*//* ------------------------------------------------------------------------*/
typedef struct
{
  uint32         q_MsgId; /*< Message ID as allocated by the service definition */
  void*          p_Msg; /*< The message itself */
  uint32         q_MsgLen; /*< Length of the message */
} qmiBitMsgInfoStructT;


/* -----------------------------------------------------------------------*//**
 @brief 
  Enum Definitions to identify the current state of the Client
  
*//* ------------------------------------------------------------------------*/
typedef enum
{
  QMI_BIT_CONNECTION_STATE_NONE,      /**< Connection context allocated but not yet ready */
  QMI_BIT_CONNECTION_STATE_INITIALIZED, /**< Connection is initialized and ready */
  QMI_BIT_CONNECTION_STATE_DISCONNECTING  /**< Connection is disconnecting */
} qmiBitConnectionStateEnumT;

/* -----------------------------------------------------------------------*//**
@brief
  Context information for each client is stored in this structure. Elements of
  this type will be stored in a queue, where context information for all 
  clients will be stored. An instance of this 
  structure is created whenever a new client connects, and destroyed when a 
  client deregisters. This structure is used as the connection handle that 
  will be given to the QCSI-Framework for it to identify a client to the 
  service provider.

  Note: The name is "Client Info" as the remote entity is a QMI-Client, and 
  as far as the QMI-Framework is concerned, it is a QMI-Client. However the 
  QMI-Client is a Logical-Service, providing its services.
  
@warning
  QMI_BIT_MAX_BIT_SERVICE defines the maximum instances of this structure. 
*//* ------------------------------------------------------------------------*/
typedef struct
{
  q_link_type                  z_Link; /*< Data Structure required by the queue 
                                           library to add this as part of a 
                                           queue */
                                             
  void                        *p_QmiClientHandle;/*< Handle identifying the 
                                           client of QMI-Bit to identify the  
                                           client */
  q_type                       z_ConnectionQ; /*< Queue of items, each of which
                                           has information specific to an  
                                           active data connection */
  gpsone_bit_register_params_type  z_BitRegParams; /**< Parameters required to
                                 register with BIT-API */
  gpsone_bit_transport_handle_type z_BitTransportHandle; /**< Transport handle
                                 allocated by the BIT-API when we 
                                 register with it */
  boolean                      b_RegisteredWithBitApi; /**< Indicates if 
                                 the client has already registerd with BIT-API */
  uint16                       w_NextConnectionId; /**< The next session ID that is 
                                 to be allocated. */
} qmiBitCltInfoStructT;

/* -----------------------------------------------------------------------*//**
 @brief 
  Structure to hold information related to a particular data-connection. This
  has a mapping from the 64-bit session handle that is allocated but the 
  BIT-Service to the 16-bit that is allocated by this module. The 16-bit 
  ConnectinId is what is sent to the BIT-API. 
  Each Connection has a Watermark that is used for buffering data that is being
  received from the network. 
*//* ------------------------------------------------------------------------*/
typedef struct 
{
  q_link_type    z_Link; /*< Link data type required by the Queue library */
  uint16         w_ConnectionId; /*< Session Id allocated by this module, 
                         a unique identifier identifying the Session 
                         ( or Data Connection ). This is passed down to the
                         BIT-API. */
  qmiBitConnectionStateEnumT  e_ConnectionState; /**< The current state of the 
                         connection */
  uint64         t_DisconnectSentTime; /**< Time stamp of when the disconnect
                         was sent to BIT-Provider */
  uint32         q_DisconnectTransactionId; /**< Transaction ID used to send the 
                         disconnect */
  uint64         t_SessionHandle; /*< Session Handle that is sent from the
                         BIT Service which is mapped to w_ConnectionId */
  dsm_watermark_type  z_Wmk; /*< Fifo Data Queue to buffer the data */
  q_type              z_DataQ; /*< Queue that is used by the Watermark */
  dsm_item_type       *pz_DsmItem; /*< A DSM Item that is dequeued, and 
                         partially sent, this will hold the dsm-item */
  qmiBitCltInfoStructT *pz_CltInfo; /*< Handle to the Client Info */
  uint64               q_ExpectedSeqNum; /*< The expected Sequence number */                         
} qmiBitConnectionInfoStructT;


/* -----------------------------------------------------------------------*//**
@brief
  A Generic int32 maps to map values from BITApi to QMIBIT. The reason chose
  int32 instead of uint32 as enums can be negative. 

@warning Used casting to (int32) so a generic function can be written to 
         Map these values. 
@warning Ensure that the first element are the default values of either 
         values.
*//* ------------------------------------------------------------------------*/
typedef struct 
{
  int32                                     i_GpsBitVal;
  int32                                     i_QmiBitVal;
}qmiBitGenericMapStructT;


/* -----------------------------------------------------------------------*//**
 @brief 
  Structure, which holds a union of all requests that are sent from the 
  BIT-Client ( QCSI-Service ) to the BIT-Daemon ( QCCI-Client ).

  This structure will be part of a queue either a free-Q or an allocated-Q. And 
  this structure is allocated on heap on an as-need-basis.  
*//* ------------------------------------------------------------------------*/
typedef struct
{
  q_link_type                     z_Link;
  qmiBitCltInfoStructT           *pz_CltInfo;
  qmiBitMsgInfoStructT            z_Msg;
  union
  {
    bit_open_req_msg_v01                  z_OpenReq;
    bit_close_req_msg_v01                 z_CloseReq;
    bit_connect_req_msg_v01               z_ConnectReq;
    bit_disconnect_req_msg_v01            z_DisconnectReq;
    bit_ready_to_receive_req_msg_v01      z_Rtr;
    bit_data_received_status_req_msg_v01  z_DataReceivedStatusReq;
    bit_send_req_msg_v01                  z_SendReq;
    bit_set_dormancy_req_msg_v01          z_SetDormancyReq;
    bit_get_local_host_info_req_msg_v01   z_GetLocalHostInfoReq;
  } z_Req;
} qmiBitReqInfoStructT;

/* -----------------------------------------------------------------------*//**
@brief
  Generic Structure to carry all the Acknowledgements that need to be sent 
  to the BIT-Service.
*//* ------------------------------------------------------------------------*/
typedef struct
{
  uint32             q_MsgId; /*< Message ID as allocated by the service 
                                  definition */
  bit_ack_msg_v01    z_GenAck; /*< The Generic Ack message that has to be 
                                     sent */
  uint32             q_MsgLen; /*< Length of the message */
}qmiBitAckInfoStructT;


/* -----------------------------------------------------------------------*//**
@brief
  Structure used as part of an array to store pending Transaction IDs
*//* ------------------------------------------------------------------------*/
typedef struct
{
  boolean             b_IsValid; /*< Identifies if this record is valid  */
  uint32              q_TransactionId; /*< Transaction Id*/
}qmiBitTransactionRecordT;



/* -----------------------------------------------------------------------*//**
@brief
  Function prototype for message handlers, to handle the messages coming from
  the BIT-Service. 

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
                                      In some message handlers this is updated.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_Ack                Response that needs to be filled in case 
                                      the message cannot be handled
*//* ------------------------------------------------------------------------*/
typedef void (*qmiBitMessageHandlerFuncT)
(
  qmiBitCltInfoStructT                   *pz_CltInfo,
  const qmiBitMsgInfoStructT             *pz_MsgInfo,
  qmiBitAckInfoStructT                   *pz_Ack
);



/*============================================================================
 * Definitions required to declare a function table of QMI-BIT Message Handlers
 * =========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
 Forward-Function Declarations of Message Handlers. Detailed description of 
 functions are provided along with function-definitions later. 
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleOpenResp (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );
static void qmiBitHandleOpenStatusInd (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );

static void qmiBitHandleCloseResp (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );
static void qmiBitHandleCloseStatusInd (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );

static void qmiBitHandleConnectResp (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );
static void qmiBitHandleConnectStatusInd (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );

static void qmiBitHandleDisconnectResp (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );
static void qmiBitHandleDisconnectStatusInd (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );

static void qmiBitHandleSendResp (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );
static void qmiBitHandleSendStatusInd (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );

static void qmiBitHandleReadyToReceiveResp (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );

static void qmiBitHandleDataReceivedInd (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );

static void qmiBitHandleDataReceivedStatusResp (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );

static void qmiBitHandleSetDormancyResp (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );
static void qmiBitHandleSetDormancyStatusInd (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );

static void qmiBitHandleGetLocalHostInfoResp (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );
static void qmiBitHandleGetLocalHostInfoStatusInd (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );

static void qmiBitHandleServiceReadyInd (  
  qmiBitCltInfoStructT*, 
  const qmiBitMsgInfoStructT*, 
  qmiBitAckInfoStructT* );


/*****************************************************************************
 * Global Data Definitions
 * *************************************************************************/

/*****************************************************************************
 * Static Variable Definitions
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
  Declaration of lookup table for Message Handlers that will handle messages
  from the BIT-Service. 

  This information SHALL NOT BE EXPOSED OUT OF THIS FILE  
*//* ------------------------------------------------------------------------*/
static struct
{
  uint32                     q_MsgId; /*< Message ID of incoming Message */
  qmiBitMessageHandlerFuncT  pz_MsgHdlrFunc; /*< Pointer to a converter 
                                     function */
} pz_qmiBitMsgHandlerTable[] = 
{
  { QMI_BIT_OPEN_RESP_V01,
    qmiBitHandleOpenResp                                                      },
  { QMI_BIT_OPEN_STATUS_IND_V01,
    qmiBitHandleOpenStatusInd                                                 },

  { QMI_BIT_CLOSE_RESP_V01,
    qmiBitHandleCloseResp                                                     },
  { QMI_BIT_CLOSE_STATUS_IND_V01,
    qmiBitHandleCloseStatusInd                                                },

  { QMI_BIT_CONNECT_RESP_V01,
    qmiBitHandleConnectResp                                                   },
  { QMI_BIT_CONNECT_STATUS_IND_V01,
    qmiBitHandleConnectStatusInd                                              },

  { QMI_BIT_DISCONNECT_RESP_V01,
    qmiBitHandleDisconnectResp                                                },
  { QMI_BIT_DISCONNECT_STATUS_IND_V01,
    qmiBitHandleDisconnectStatusInd                                           },

  { QMI_BIT_SEND_RESP_V01,
    qmiBitHandleSendResp                                                      },
  { QMI_BIT_SEND_STATUS_IND_V01,
    qmiBitHandleSendStatusInd                                                 },

  { QMI_BIT_READY_TO_RECEIVE_RESP_V01,
    qmiBitHandleReadyToReceiveResp                                            },

  { QMI_BIT_DATA_RECEIVED_IND_V01,
    qmiBitHandleDataReceivedInd                                               },

  { QMI_BIT_DATA_RECEIVED_STATUS_RESP_V01,
    qmiBitHandleDataReceivedStatusResp                                        },

  { QMI_BIT_SET_DORMANCY_RESP_V01,
    qmiBitHandleSetDormancyResp                                               },
  { QMI_BIT_SET_DORMANCY_STATUS_IND_V01,
    qmiBitHandleSetDormancyStatusInd                                          },

  { QMI_BIT_GET_LOCAL_HOST_INFO_RESP_V01,
    qmiBitHandleGetLocalHostInfoResp                                          },
  { QMI_BIT_GET_LOCAL_HOST_INFO_STATUS_IND_V01,
    qmiBitHandleGetLocalHostInfoStatusInd                                     },

  { QMI_BIT_SERVICE_READY_IND_V01,
    qmiBitHandleServiceReadyInd                                               },
};


/* -----------------------------------------------------------------------*//**
@brief
  Map to map qmi_csi_error to qmi_csi_cb_error
*//* ------------------------------------------------------------------------*/
static struct 
{
  qmi_csi_error       e_QcsiErr;
  qmi_csi_cb_error    e_QcsiCbErr;
}pz_qmiBitQcsiErrToCbErrMap[] =
{
    { QMI_CSI_NO_ERR,           QMI_CSI_CB_NO_ERR           },
    { QMI_CSI_CONN_REFUSED,     QMI_CSI_CB_CONN_REFUSED     },
    { QMI_CSI_CONN_BUSY,        QMI_CSI_CB_INTERNAL_ERR     },
    { QMI_CSI_INVALID_HANDLE,   QMI_CSI_CB_INTERNAL_ERR     },
    { QMI_CSI_INVALID_ARGS,     QMI_CSI_CB_INTERNAL_ERR     },
    { QMI_CSI_ENCODE_ERR,       QMI_CSI_CB_INTERNAL_ERR     },
    { QMI_CSI_DECODE_ERR,       QMI_CSI_CB_INTERNAL_ERR     },
    { QMI_CSI_NO_MEM,           QMI_CSI_CB_NO_MEM           },
    { QMI_CSI_INTERNAL_ERR,     QMI_CSI_CB_INTERNAL_ERR     },
};

/* -----------------------------------------------------------------------*//**
@brief
  Map to map gpsone_bit_status_e_type to qmi_error_type_v01

@warning
  The first element is the default value 
*//* ------------------------------------------------------------------------*/
static qmiBitGenericMapStructT pz_qmiBitStatusToQmiErrorMap[] =
{
  { (int32)GPSONE_BIT_STATUS_FAIL,                  (int32)QMI_ERR_GENERAL_V01           },
  { (int32)GPSONE_BIT_STATUS_SUCCESS,               (int32)QMI_ERR_NONE_V01              },
  { (int32)GPSONE_BIT_STATUS_WAIT,                  (int32)QMI_ERR_GENERAL_V01           },
  { (int32)GPSONE_BIT_STATUS_INVALID_PARAM,         (int32)QMI_ERR_INVALID_ARG_V01       },
  { (int32)GPSONE_BIT_STATUS_NOT_REGISTERED,        (int32)QMI_ERR_INVALID_CLIENT_ID_V01 },
  { (int32)GPSONE_BIT_STATUS_IPC_FAILURE,           (int32)QMI_ERR_INTERNAL_V01          },
  { (int32)GPSONE_BIT_STATUS_FATAL_ERROR,           (int32)QMI_ERR_INTERNAL_V01          },
  { (int32)GPSONE_BIT_STATUS_NOT_ALLOWED,           (int32)QMI_ERR_DISABLED_V01          },
  { (int32)GPSONE_BIT_STATUS_OPEN_FAILED,           (int32)QMI_ERR_GENERAL_V01           },
  { (int32)GPSONE_BIT_STATUS_NO_MORE_DATA,          (int32)QMI_ERR_GENERAL_V01           },
  { (int32)GPSONE_BIT_STATUS_NO_RESOURCES,          (int32)QMI_ERR_INSUFFICIENT_RESOURCES_V01},
  { (int32)GPSONE_BIT_STATUS_OUT_OF_MEMORY,         (int32)QMI_ERR_NO_MEMORY_V01         },
  { (int32)GPSONE_BIT_STATUS_MEMORY_ACCESS_ERROR,   (int32)QMI_ERR_INTERNAL_V01          },
  { (int32)GPSONE_BIT_STATUS_NETWORK_NOT_AVAILABLE, (int32)QMI_ERR_NO_NETWORK_FOUND_V01  },
  { (int32)GPSONE_BIT_STATUS_NOT_INTIALIZED,        (int32)QMI_ERR_INTERNAL_V01          },
  { (int32)GPSONE_BIT_STATUS_NOT_IMPLEMENTED,       (int32)QMI_ERR_NOT_SUPPORTED_V01     },
};

/* -----------------------------------------------------------------------*//**
@brief
  Map to map gpsone_bit_link_e_type to  bit_link_enum_type_v01

@warning
  The first element is the default value 
*//* ------------------------------------------------------------------------*/
static qmiBitGenericMapStructT pz_qmiBitLinkMap[] =
{
    { (int32)GPSONE_BIT_LINK_TYPE_MAX,   (int32)BIT_LINK_ENUM_TYPE_MAX_ENUM_VAL_V01},
    { (int32)GPSONE_BIT_LINK_HTTP,       (int32)BIT_ENUM_LINK_HTTP_V01},
    { (int32)GPSONE_BIT_LINK_TCP,        (int32)BIT_ENUM_LINK_TCP_V01},
    { (int32)GPSONE_BIT_LINK_SMS,        (int32)BIT_ENUM_LINK_SMS_V01},
    { (int32)GPSONE_BIT_LINK_UDP,        (int32)BIT_ENUM_LINK_UDP_V01},
};


/* -----------------------------------------------------------------------*//**
@brief
  Map to map gpsone_bit_protocol_e_type to  bit_protocol_enum_type_v01

@warning
  The first element is the default value 
*//* ------------------------------------------------------------------------*/
static qmiBitGenericMapStructT pz_qmiBitProtocolMap[] =
{
  { (int32)GPSONE_BIT_PROTOCOL_TYPE_NONE,        (int32)BIT_PROTOCOL_ENUM_TYPE_MIN_ENUM_VAL_V01},
  { (int32)GPSONE_BIT_PROTOCOL_TYPE_ANY,         (int32)BIT_ENUM_PROTOCOL_ANY_V01},
  { (int32)GPSONE_BIT_PROTOCOL_TYPE_SUPL,        (int32)BIT_ENUM_PROTOCOL_AGPS_SUPL_V01},
  { (int32)GPSONE_BIT_PROTOCOL_TYPE_IS801,       (int32)BIT_ENUM_PROTOCOL_AGPS_IS801_V01},
  { (int32)GPSONE_BIT_PROTOCOL_TYPE_V1,          (int32)BIT_ENUM_PROTOCOL_AGPS_V1_V01},
  { (int32)GPSONE_BIT_PROTOCOL_TYPE_V2,          (int32)BIT_ENUM_PROTOCOL_AGPS_V2_V01},
  { (int32)GPSONE_BIT_PROTOCOL_TYPE_JCDMA,       (int32)BIT_ENUM_PROTOCOL_JCDMA_V01},
};



/* -----------------------------------------------------------------------*//**
@brief
  Singleton instance of module information. This is the service handle 
  that will be given to the framework to identify the service. And contains 
  the state information of the shim layer. 
*//* ------------------------------------------------------------------------*/
struct
{
  q_type                 z_ClientQ; /**< Information about all the clients that
                               have connected to the service. This is queue 
                               of type locQmiShimCltInfoStructT */
  void*                  p_ServiceHandle;  /**< Handle that the framework
                               provides to represent this service connection */
  qmi_sap_client_handle  z_QSAPUserHandle; /**< QSAP user handle to register with 
                                QSAP.*/
  q_type                 z_RequestFreeQ; /**< Queue of free items that are 
                                needed for constructing a Request message
                                to the BIT-Service */
  q_type                 z_RequestQ; /**< Queue of items that are currently
                                allocated and being serviced to send the 
                                request out. This is more for accounting than
                                use in code. Helps in Debugging */
  q_type                 z_ConnectionFreeQ; /**< Queue of free items that are 
                                used for holding context pertaining to a 
                                connection */  
  qmiBitTransactionRecordT pz_PendingTransactions[QMI_BIT_MAX_PENDING_TRANSACTION_IDS];
                                /*< All pending transactions that are required to
                                    be cached for validation are stored here */
} z_QmiBit;

/*****************************************************************************
 * Internal/Static Function Declarations and Definitions
 * *************************************************************************/


/*============================================================================
 *                    U T I L I T Y    F U N C T I O N S 
 * =========================================================================*/

/* -----------------------------------------------------------------------*//**
 @brief 
  Inline function to set the qmi_response_type_v01.  It also sets the result 
  based on the error
  
@param[in|out] pz_Resp       The variable which needs to be set
@param[in]     e_ErrVal      The Error that needs to be set. 
*//* ------------------------------------------------------------------------*/
static inline void qmiBitSetResponse
(
  qmi_response_type_v01 *pz_Resp, 
  qmi_error_type_v01     e_ErrVal
)
{
    pz_Resp->error = e_ErrVal;                                               
    pz_Resp->result =                                                     
       ( QMI_ERR_NONE_V01 == e_ErrVal ) ? 
        QMI_RESULT_SUCCESS_V01:
        QMI_RESULT_FAILURE_V01;
}


/* -----------------------------------------------------------------------*//**
@brief
  A helper utility provided to Convert GPSBitApi Values to QMIBitValues
  
@param[in]  pz_Array           The Array in question
@param[in]  i_GpsBitValue      The Source GPSOne Bit Val


@retval    Corresponding QmiBitValue
*//* ------------------------------------------------------------------------*/
#define QMI_BIT_GET_QMI_BIT_VAL(pz_Array, i_GpsBitValue)                 \
        qmiBitGetQMIBITVal( pz_Array, GNSS_ARRAY_SIZEOF(pz_Array), i_GpsBitValue)
static int32 qmiBitGetQMIBITVal
( 
  qmiBitGenericMapStructT *pz_Array, 
  uint32                   q_SizeOfArray,
  int32                    i_GpsBitValue
)
{
  uint32  q_Cntr;
  int32   i_RetVal = pz_Array[0].i_QmiBitVal;  /* Set the Default Value, 
                              which is the first element */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  for ( q_Cntr = 0; q_Cntr < q_SizeOfArray; q_Cntr++ )
  {
    if (  i_GpsBitValue == pz_Array[q_Cntr].i_GpsBitVal)
    { 
      i_RetVal = pz_Array[q_Cntr].i_QmiBitVal;
      break;
    }
  }
  return i_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  A helper utility provided to Convert QMIBitValues Values to GPSBitApi
  
@param[in]  pz_Array           The Array in question
@param[in]  i_QmiBitValue      The Source GPSOne Bit Val

@retval    Corresponding GpsBitValue
*//* ------------------------------------------------------------------------*/
#define QMI_BIT_GET_GPS_BIT_VAL(pz_Array, i_QmiBitValue)                 \
          qmiBitGetGpsBITVal( pz_Array, GNSS_ARRAY_SIZEOF(pz_Array), i_QmiBitValue)
static int32 qmiBitGetGpsBITVal
( 
  qmiBitGenericMapStructT *pz_Array, 
  uint32                   q_SizeOfArray,
  int32                    i_QmiBitValue
)
{
  uint32  q_Cntr;
  int32   i_RetVal = pz_Array[0].i_GpsBitVal; /* Set the Default Value, 
                              which is the first element */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  for ( q_Cntr = 0; q_Cntr < q_SizeOfArray; q_Cntr++ )
  {
    if (  i_QmiBitValue == pz_Array[q_Cntr].i_QmiBitVal)
    { 
      i_RetVal = pz_Array[q_Cntr].i_GpsBitVal;
      break;
    }
  }
  return i_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  A helper utility provided to the queue library. This function is intended to 
  see if the  CltInfo represented by "item_ptr" holds information for a 
  QMI-Client represented by "p_compareVal" which is 
  CltInfo->p_QmiClientHandle
  
@param[in]   p_ItemPtr         A potential item of z_QmiBit.z_ClientQ
@param[in]   p_CompareVal      Qmi-Client handle that is being considered

@retval    0          Item was not the one we are searching for
@retval    1          Item was the one we are searching for
*//* ------------------------------------------------------------------------*/
static int qmiBitCompareQmiClientHandle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
  qmiBitCltInfoStructT *pz_CltInfo = (qmiBitCltInfoStructT*)p_ItemPtr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is The Client Handle we get from QCSI Framework */
  return ( (pz_CltInfo->p_QmiClientHandle == p_CompareVal) ? 1 : 0 );
}

/* -----------------------------------------------------------------------*//**
@brief
  A helper utility provided to the queue library. This function is intended to 
  see if the  CltInfo represented by "item_ptr" holds information for a 
  QMI-Client represented by "p_compareVal" which is also a CltInfo 
  structure.
  
@param[in]   p_ItemPtr         A potential item of z_QmiBit.z_ClientQ
@param[in]   p_CompareVal      Qmi-Client handle that is being considered

@retval    0          Item was not the one we are searching for
@retval    1          Item was the one we are searching for
*//* ------------------------------------------------------------------------*/
static int qmiBitCompareCltInfoHandle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is also a Connection Handle */
  return ( (p_ItemPtr == p_CompareVal) ? 1 : 0 );
}

/* -----------------------------------------------------------------------*//**
@brief
  A helper utility provided to the queue library. This function is intended to 
  see if the  CltInfo represented by "item_ptr" corresponds to the transport 
  handle allocated by BIT-API "p_compareVal" which is 
  CltInfo->z_BitTransportHandle
  
@param[in]   p_ItemPtr         A potential item of z_QmiBit.z_ClientQ
@param[in]   p_CompareVal      Transport Handle that is being considered

@retval    0          Item was not the one we are searching for
@retval    1          Item was the one we are searching for
*//* ------------------------------------------------------------------------*/
static int qmiBitCompareTransportHandle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
  qmiBitCltInfoStructT *pz_CltInfo = (qmiBitCltInfoStructT*)p_ItemPtr;
  gpsone_bit_transport_handle_type *pz_TransportHandle = 
                          (gpsone_bit_transport_handle_type*)p_CompareVal;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val Is the transport handle allocated by GPS-BIT-API */
  return ( (pz_CltInfo->z_BitTransportHandle == (*pz_TransportHandle)) ? 1 : 0 );
}


/* -----------------------------------------------------------------------*//**
@brief
  A helper utility provided to the queue library. This function is intended to 
  see if the ConnectionItem represented by "item_ptr" corresponds to the 
  session handle that is allocated by the BIT-Service "p_CompareVal" which is
  a 64-bit unsigned integer
  
@param[in]   p_ItemPtr         A potential item of z_QmiBit.z_ClientQ
@param[in]   p_CompareVal      Session handle that is being considered

@retval    0          Item was not the one we are searching for
@retval    1          Item was the one we are searching for
*//* ------------------------------------------------------------------------*/
static int qmiBitCompareConnectionItemQMISessionHandle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
  qmiBitConnectionInfoStructT *pz_ConnInfo = (qmiBitConnectionInfoStructT*)p_ItemPtr;
  uint64  *pt_SessionHandle = 
                          (uint64  *)p_CompareVal;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is the 64-Bit Session handle allocated by the
     BIT-Service */
  return ( (pz_ConnInfo->t_SessionHandle == (*pt_SessionHandle)) ? 1 : 0 );
}

/* -----------------------------------------------------------------------*//**
@brief
  A helper utility provided to the queue library. This function is intended to 
  see if the ConnectionItem represented by "item_ptr" corresponds to the 
  session handle that is allocated by the BIT-API "p_CompareVal" which is
  a 16-bit unsigned integer
  
@param[in]   p_ItemPtr         A potential item of z_QmiBit.z_ClientQ
@param[in]   p_CompareVal      Session handle that is being considered

@retval    0          Item was not the one we are searching for
@retval    1          Item was the one we are searching for
*//* ------------------------------------------------------------------------*/
static int qmiBitCompareConnectionItemBITSessionHandle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
  qmiBitConnectionInfoStructT *pz_ConnInfo = (qmiBitConnectionInfoStructT*)p_ItemPtr;
  gpsone_bit_session_handle_type  *pt_SessionHandle = 
                          (gpsone_bit_session_handle_type*)p_CompareVal;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is the 16-Bit Session handle allocated by the
     QMI-Bit Shim Layer */
  return ( (pz_ConnInfo->w_ConnectionId == (uint16)(*pt_SessionHandle)) ? 1 : 0 );
}


/* -----------------------------------------------------------------------*//**
@brief
  A wrapper to send the Acknowledgement. This would increase the readability, 
  as the Ack is infact a QMI-Response, and we need to send this in response to 
  any message i.e. QMI-Requests that come from the BIT-Service 
  ( i.e. a QMI-Client)
  
@param[in]   pz_AckInfo         Handle to the information that needs to be 
                               sent out. 

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB.....       Look into the enumeration qmi_csi_error for
                                 the error values.
*//* ------------------------------------------------------------------------*/
static qmi_csi_cb_error qmiBitSendAck
(
  qmiBitCltInfoStructT  *pz_CltInfo,
  qmi_req_handle         z_ReqHandle,
  qmiBitAckInfoStructT  *pz_AckInfo 
)
{
  qmi_csi_cb_error e_RetVal = QMI_CSI_CB_INTERNAL_ERR; /*< If error cannot be 
                       mapped, INTERNAL_ERR is returned */
  qmi_csi_error    e_qcsiError = QMI_CSI_NO_ERR;
  uint32           q_Cntr = 0;
  uint32     q_SizeOfArray = GNSS_ARRAY_SIZEOF(pz_qmiBitQcsiErrToCbErrMap);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* NOT DOING parameter check, this is called internally from this module */

  /* Msg Id for Req and Resp are the same, hence sending response to the 
     incoming message id */
  e_qcsiError = mqcsi_send_rsp ( 
                              pz_CltInfo->p_QmiClientHandle,
                              z_ReqHandle, 
                              pz_AckInfo->q_MsgId,  
                              &(pz_AckInfo->z_GenAck),
                              pz_AckInfo->q_MsgLen );

  if ( QMI_CSI_NO_ERR != e_qcsiError )
  {
    QMI_BIT_MSG_LOW( "Error From QCSI:%d while trying to send Ack",
                     e_qcsiError,0,0);
  }

  for ( q_Cntr = 0; q_Cntr < q_SizeOfArray; q_Cntr++ )
  {
    if (  e_qcsiError == pz_qmiBitQcsiErrToCbErrMap[q_Cntr].e_QcsiErr)
    { 
      e_RetVal = pz_qmiBitQcsiErrToCbErrMap[q_Cntr].e_QcsiCbErr;
      break;
    }
  }

  return e_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  A wrapper to send the Request. This would increase the readability, 
  as the Request is infact a QMI-Indication, and we need to send such an
  indication to the BIT-Service ( over QCCI ) from the BIT-Client ( over QCSI )

@param[in]   pz_CltInfo         The Client to which this message needs to be 
                                sent.                                
@param[in]   pz_Msg             Handle to the information that needs to be 
                                sent out. 

@retval    TRUE                 Success
@retval    FALSE                Look into the enumeration qmi_csi_error for
                                the error values.
*//* ------------------------------------------------------------------------*/
static boolean qmiBitSendReq
(
  qmiBitCltInfoStructT  *pz_CltInfo,
  qmiBitMsgInfoStructT  *pz_Msg
)
{
  qmi_csi_error   e_QcsiErr;
  boolean b_RetVal = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* NOT DOING parameter check, this is called internally from this module */
  e_QcsiErr = mqcsi_send_ind (    pz_CltInfo->p_QmiClientHandle,
                                  pz_Msg->q_MsgId,
                                  pz_Msg->p_Msg,
                                  pz_Msg->q_MsgLen );

  b_RetVal = (e_QcsiErr == QMI_CSI_NO_ERR)?TRUE:FALSE;

  return b_RetVal;

}


/* -----------------------------------------------------------------------*//**
@brief
  A utility function to allocate RequestItems 

@param[in]   pz_CltInfo   The Client for whom this item is allocated
  
@retval    NULL       Request Item could not be allocated
@retval    non-NULL   Pointer to the Request Item
*//* ------------------------------------------------------------------------*/
static qmiBitReqInfoStructT *qmiBitAllocateRequestItem
( 
  qmiBitCltInfoStructT *pz_CltInfo
)
{
  qmiBitReqInfoStructT* pz_RetVal = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* NOT DOING parameter check, this is called internally from this module */

  /* Try to get an item from the FreeQ */
  pz_RetVal = q_get(&z_QmiBit.z_RequestFreeQ);

  if ( NULL == pz_RetVal )
  {
    /* If Heap allocation fails pz_RetVal will remain NULL */
    pz_RetVal = (qmiBitReqInfoStructT*)os_MemAlloc( 
                            sizeof(qmiBitReqInfoStructT),
                            OS_MEM_SCOPE_TASK );    
  }

  if ( NULL != pz_RetVal )
  {
    /* Zero out the ReqItem to get rid of any unknown information */
    memset(pz_RetVal, 0, sizeof(*pz_RetVal) );
    
    /* Initialize the Link Data Structure */
    q_link( pz_RetVal, &pz_RetVal->z_Link);
  
    /* Store the Client Info */
    pz_RetVal->pz_CltInfo = pz_CltInfo;

    /* Now queue it up into the Request Q */
    q_put(&z_QmiBit.z_RequestQ, &pz_RetVal->z_Link);
  }
  return pz_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  A utility function to free the RequestItems. This function does not free
  up the item from the heap. It stores it in a queue for future use, so 
  we dont waste time in allocation over heap and cause fragmentation by 
  repeated alloc/free of memory on heap.

@param[in|out]    ppz_ReqItem   The Item that needs to be released. The handle
                                is cleared so the caller cannot use this
                                handle by mistake after the call

@retval    TRUE    Successfully DeAllocated/Freed
@retval    FALSE   Could not DeAllocate item 
*//* ------------------------------------------------------------------------*/
static boolean qmiBitDeallocateRequestItem
( 
  qmiBitReqInfoStructT   **ppz_ReqItem
)
{
  boolean b_RetVal = FALSE;
  qmiBitReqInfoStructT* pz_Buf= NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NOT DOING parameter check, this is called internally from this module */

  pz_Buf = (qmiBitReqInfoStructT*)(*ppz_ReqItem);
  
  if ( FALSE  != q_delete_ext ( &(z_QmiBit.z_RequestQ), 
                                 &(pz_Buf->z_Link) ) )
  {
    q_put ( &(z_QmiBit.z_RequestFreeQ), &(pz_Buf->z_Link) );
    (*ppz_ReqItem) = NULL;
    b_RetVal = TRUE;
  }

  if ( FALSE == b_RetVal )
  {
    QMI_BIT_MSG_ERROR("Could not Deallocate Message Buffer",0,0,0);
  }
  return b_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  A utility function to count the number of bytes in the buffer for the 
  connection

@param[in]    pz_ConnectionItem  Handle to the connection

@retval    The number of bytes in the buffer.
*//* ------------------------------------------------------------------------*/
static uint32 qmiBitGetCurrBuffCnt
(
  qmiBitConnectionInfoStructT *pz_ConnectionItem 
)
{
  uint32 q_RetVal = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL != pz_ConnectionItem )
  {
    /* Find the number of bytes in the Buffer */
    q_RetVal = dsm_queue_cnt(&(pz_ConnectionItem->z_Wmk));
    
    /* We might have a temporary buffer with data */
    if ( NULL != pz_ConnectionItem->pz_DsmItem )
    {
      q_RetVal += dsm_length_packet(pz_ConnectionItem->pz_DsmItem);
    }
  }

  return q_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  Callback function that is registered with the Watermark library. This will
  be called whenever the amount of data in the Watermark falls below the 
  Lo-Wmk-Threshold

  A Macro is provided internally if the module needs to generate Ready to 
  Receive to let the BIT-Service, know that the BIT-Client can recieve 
  data.
  
@param[in]   pz_Wmk         The Watermark that is in context.
@param[in]   pz_ConnItem    The Connection Item is sent in as the user-data.
*//* ------------------------------------------------------------------------*/
#define  QMI_BIT_NOTIFY_CLIENT_READY_TO_RECEIVE(pz_Wmk, pz_ConnectionItem)    \
      qmiBitLoWmkCb(pz_Wmk, (void*)pz_ConnectionItem)
static void qmiBitLoWmkCb
(
  dsm_watermark_type *pz_Wmk, 
  void               *p_ConnectionItem
)
{
  uint32 q_CheckPt = 0;
  qmiBitReqInfoStructT *pz_ReqItem = NULL;
  qmiBitConnectionInfoStructT *pz_ConnInfo = 
              (qmiBitConnectionInfoStructT*)p_ConnectionItem;
  uint32 q_WmkCurrCnt = 0; 
  uint32 q_MaxBytesCanRecv = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {

    if ( NULL == pz_ConnInfo )
    {
      q_CheckPt = 1;
      break;
    }

    q_WmkCurrCnt = qmiBitGetCurrBuffCnt(pz_ConnInfo);
    
    if ( q_WmkCurrCnt >= QMI_BIT_HI_WMK_THRESHOLD )
    {
      q_CheckPt = 2;
      break;
    }

    pz_ReqItem = qmiBitAllocateRequestItem(pz_ConnInfo->pz_CltInfo);
    if ( NULL == pz_ReqItem )
    {
      q_CheckPt = 3; 
      break;
    }

    q_MaxBytesCanRecv = MIN ( BIT_CONST_PAYLOAD_LEN_MAX_V01,
                              (QMI_BIT_HI_WMK_THRESHOLD - q_WmkCurrCnt) );
    
    
    pz_ReqItem->z_Req.z_Rtr.rtr = TRUE;
    pz_ReqItem->z_Req.z_Rtr.session_handle = pz_ConnInfo->t_SessionHandle;
    pz_ReqItem->z_Req.z_Rtr.transaction_id = QMI_BIT_TRANSACTION_ID_RTR;
    QMI_BIT_SET_OPTIONAL_VALUE( pz_ReqItem->z_Req.z_Rtr.max_recv_payload_size,
                                q_MaxBytesCanRecv );
    
    pz_ReqItem->z_Msg.q_MsgId = QMI_BIT_READY_TO_RECEIVE_REQ_V01;
    pz_ReqItem->z_Msg.p_Msg = (void*)&(pz_ReqItem->z_Req.z_Rtr);
    pz_ReqItem->z_Msg.q_MsgLen = sizeof(pz_ReqItem->z_Req.z_Rtr);
    
    if ( FALSE == qmiBitSendReq(
                              pz_ConnInfo->pz_CltInfo,
                              &(pz_ReqItem->z_Msg ) ) )
    {
      q_CheckPt = 4;

      /** NOTE: Fall through, if there is some code that is added below after
      the If-statment, and we end up breaking from here, we need to 
      deallocate the Request Item */
    }
    
    /* Free the Handle to the ReqItem, as it is not needed now. We sent the 
       message */
    qmiBitDeallocateRequestItem(&pz_ReqItem);
  }while(0);


  if ( 0 != q_CheckPt )
  {
    QMI_BIT_MSG_ERROR("Could not send RTR:TRUE CheckPt:%d",q_CheckPt,0,0);
  }
}


/* -----------------------------------------------------------------------*//**
@brief
  Callback function that is registered with the Watermark library. This will
  be called whenever the amount of data in the Watermark goes above the 
  Hi-Wmk-Threshold
  
  A Macro is provided internally if the module needs to generate Ready to 
  Receive to let the BIT-Service, know that the BIT-Client cannot recieve 
  data.

@param[in]   pz_Wmk         The Watermark that is in context.
@param[in]   pz_ConnItem    The Connection Item is sent in as the user-data.
*//* ------------------------------------------------------------------------*/
#define  QMI_BIT_NOTIFY_CLIENT_NOT_READY_TO_RECEIVE(pz_Wmk, pz_ConnectionItem)    \
        qmiBitHiWmkCb(pz_Wmk, (void*)pz_ConnectionItem)
static void qmiBitHiWmkCb
(
  dsm_watermark_type *pz_Wmk, 
  void               *p_ConnectionItem
)
{
  uint32 q_CheckPt = 0;
  qmiBitReqInfoStructT *pz_ReqItem = NULL;
  qmiBitConnectionInfoStructT *pz_ConnInfo = 
              (qmiBitConnectionInfoStructT*)p_ConnectionItem;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {

    if ( NULL == pz_ConnInfo )
    {
      q_CheckPt = 1;
      break;
    }

    pz_ReqItem = qmiBitAllocateRequestItem(pz_ConnInfo->pz_CltInfo);
    if ( NULL == pz_ReqItem )
    {
      q_CheckPt = 2; 
      break;
    }

    pz_ReqItem->z_Req.z_Rtr.rtr = FALSE;
    pz_ReqItem->z_Req.z_Rtr.session_handle = pz_ConnInfo->t_SessionHandle;
    pz_ReqItem->z_Req.z_Rtr.transaction_id = QMI_BIT_TRANSACTION_ID_RTR;
    
    pz_ReqItem->z_Msg.q_MsgId = QMI_BIT_READY_TO_RECEIVE_REQ_V01;
    pz_ReqItem->z_Msg.p_Msg = (void*)&(pz_ReqItem->z_Req.z_Rtr);
    pz_ReqItem->z_Msg.q_MsgLen = sizeof(pz_ReqItem->z_Req.z_Rtr);
    
    if ( FALSE == qmiBitSendReq(
                              pz_ConnInfo->pz_CltInfo,
                              &(pz_ReqItem->z_Msg ) ) )
    {
      q_CheckPt = 3;
      /** NOTE: Fall through, if there is some code that is added below after
      the If-statment, and we end up breaking from here, we need to 
      deallocate the Request Item */
    }
    
    /* Free the Handle to the ReqItem, as it is not needed now. We sent the 
       message */
    qmiBitDeallocateRequestItem(&pz_ReqItem);
  }while(0);


  if ( 0 != q_CheckPt )
  {
    QMI_BIT_MSG_ERROR("Could not send RTR:TRUE CheckPt:%d",q_CheckPt,0,0);
  }
}

/* -----------------------------------------------------------------------*//**
@brief
  Callback function that is registered with the Watermark library. This will
  be called whenever the first data is queued into the watermark, after the 
  watermark becomes empty. ( ie when it becomes nonEmpty )
  
@param[in]   pz_Wmk         The Watermark that is in context.
@param[in]   pz_ConnItem    The Connection Item is sent in as the user-data.
*//* ------------------------------------------------------------------------*/
static void qmiBitNonEmptyWmkCb
(
  dsm_watermark_type *pz_Wmk, 
  void               *p_ConnectionItem
)
{
  qmiBitConnectionInfoStructT *pz_ConnInfo = 
              (qmiBitConnectionInfoStructT*)p_ConnectionItem;
  gpsone_bit_event_payload_type z_GpsEventPayload; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&z_GpsEventPayload, 0, sizeof(z_GpsEventPayload) );

  z_GpsEventPayload.event = GPSONE_BIT_EVENT_DATA_READY;
  z_GpsEventPayload.result = GPSONE_BIT_STATUS_SUCCESS;
  z_GpsEventPayload.arg.bytes_available = qmiBitGetCurrBuffCnt(pz_ConnInfo);

  if ( GPSONE_BIT_STATUS_SUCCESS != 
               gpsone_bit_notify( pz_ConnInfo->pz_CltInfo->z_BitTransportHandle,
                       (gpsone_bit_session_handle_type)pz_ConnInfo->w_ConnectionId,
                       0, /* No need for transaction Id in this call */
                       &z_GpsEventPayload) )
  {
    QMI_BIT_MSG_ERROR( "Could not notify BIT-API DATA-READY QMISession:%d, BitSession:%d", 
                       pz_ConnInfo->t_SessionHandle,
                       pz_ConnInfo->w_ConnectionId,
                       0);
  }
}

/* -----------------------------------------------------------------------*//**
@brief
  A utility function to allocate ConnectionInfo Items 

@param[in|out]   pz_CltInfo         The Client for whom this item is allocated
@param[in]       t_SessionHandle    Session Handle allocated but BIT Service 
                                    for which a ConnectionInfoItem has to
                                    be allocated
  
@retval    GPSONE_BIT_INVALID_SESSION_HANDLE   
                                    Request Item could not be allocated
@retval    ValidSessionId           Request Item was allocated
*//* ------------------------------------------------------------------------*/
static void qmiBitFlushUnrespondedDisconnects(qmiBitCltInfoStructT*); 
        /**< Forward declaration Required */
        
static gpsone_bit_session_handle_type qmiBitAllocateConnectionInfoItem
( 
  qmiBitCltInfoStructT *pz_CltInfo,
  uint64                t_QmiBitSessionHandle
)
{
  qmiBitConnectionInfoStructT* pz_ConnItem = NULL;
  uint32                       q_CheckPt = 0;
  gpsone_bit_session_handle_type z_RetVal = GPSONE_BIT_INVALID_SESSION_HANDLE;
  boolean b_ValidConnectionId = FALSE;
  uint16 w_Cntr = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  do
  {

    /* Flush out the Connection Items which have timed out, 
       ie. The Disconnect Request to the BIT-Provide were not responded */
    qmiBitFlushUnrespondedDisconnects(pz_CltInfo);
    
    /* Ensure that a duplicate Session Handle is not allocated by the 
       BIT-Provider */
    pz_ConnItem = (qmiBitConnectionInfoStructT*)q_linear_search(
                    &(pz_CltInfo->z_ConnectionQ),
                    qmiBitCompareConnectionItemQMISessionHandle,
                    (void*) &t_QmiBitSessionHandle );
    if ( NULL != pz_ConnItem )
    {
      q_CheckPt = 1;
      break;
    }
     
    /* Try to get an item from the FreeQ */
    pz_ConnItem = q_get(&z_QmiBit.z_ConnectionFreeQ);

    if ( NULL == pz_ConnItem )
    {
      /* If Heap allocation fails pz_RetVal will remain NULL */
      pz_ConnItem = (qmiBitConnectionInfoStructT*)os_MemAlloc( 
                              sizeof(qmiBitConnectionInfoStructT),
                              OS_MEM_SCOPE_TASK );    
    }

    if ( NULL == pz_ConnItem )
    {
      q_CheckPt = 2;
      break;
    }
    
    /* Zero out the ReqItem to get rid of any unknown information */
    memset(pz_ConnItem, 0, sizeof(*pz_ConnItem) );
    
    /* Initialize the Link Data Structure */
    q_link( pz_ConnItem, &pz_ConnItem->z_Link);

    /* Set the Session Handle Allocated by the BIT-Service */
    pz_ConnItem->t_SessionHandle = t_QmiBitSessionHandle;

    /* Initialize the connection State */
    pz_ConnItem->e_ConnectionState = QMI_BIT_CONNECTION_STATE_NONE;
    
    /* Set the Session Id */
    pz_ConnItem->w_ConnectionId = pz_CltInfo->w_NextConnectionId;
    /* Update the Next Session ID */

    /* Counter to ensure that we are not in an infinite loop */
    w_Cntr = 0;
    do
    {
      b_ValidConnectionId = FALSE;

      pz_CltInfo->w_NextConnectionId++;
      if ( GPSONE_BIT_INVALID_SESSION_HANDLE == pz_CltInfo->w_NextConnectionId )
      {
        /* The first valid Session ID */
        pz_CltInfo->w_NextConnectionId = 0;
      }

      /* Ensure that the next Connection ID is not taken */
      if ( NULL == q_linear_search(
                      &(pz_CltInfo->z_ConnectionQ),
                      qmiBitCompareConnectionItemBITSessionHandle,
                      (void*) &(pz_CltInfo->w_NextConnectionId) ) )
      {
        b_ValidConnectionId = TRUE;
      }
      w_Cntr ++;;
    } while ( ( FALSE == b_ValidConnectionId ) &&
                         ( w_Cntr < 0xFFFF ) ); /* [0, 0xFFFF] is the range, if 
                         we have searched the entire range lets bail out */

    /* Could not allocate a connection ID */
    if (  FALSE == b_ValidConnectionId )
    {
      q_CheckPt = 3;
      os_MemFree((void**)&pz_ConnItem);
      break;
    }
                         

    /* Remember the client for which this is allocated */
    pz_ConnItem->pz_CltInfo = pz_CltInfo;

    /* Set the first sequence number that is expected */
    pz_ConnItem->q_ExpectedSeqNum = 1;

    /* Initialize the WaterMark for this connection.
       NOTE: This call also initializes the Q. */
    dsm_queue_init( &(pz_ConnItem->z_Wmk),
                    QMI_BIT_DNE_WMK_THRESHOLD,
                    &(pz_ConnItem->z_DataQ));

    pz_ConnItem->z_Wmk.lo_watermark = QMI_BIT_LO_WMK_THRESHOLD;
    pz_ConnItem->z_Wmk.lowater_func_data = (void*)pz_ConnItem;
    pz_ConnItem->z_Wmk.lowater_func_ptr = qmiBitLoWmkCb;
    
    pz_ConnItem->z_Wmk.hi_watermark = QMI_BIT_HI_WMK_THRESHOLD;
    pz_ConnItem->z_Wmk.hiwater_func_data = (void*)pz_ConnItem;
    pz_ConnItem->z_Wmk.hiwater_func_ptr = qmiBitHiWmkCb;

    pz_ConnItem->z_Wmk.non_empty_func_data = (void*)pz_ConnItem;
    pz_ConnItem->z_Wmk.non_empty_func_ptr = qmiBitNonEmptyWmkCb;

    /* The return value is the 16-bit session-handle that will be sent 
       to the BIt-API */
    z_RetVal = (gpsone_bit_session_handle_type)pz_ConnItem->w_ConnectionId;
    
    /* Now queue it up into Connection Info Q for a BIT-Service */
    q_put(&pz_CltInfo->z_ConnectionQ, &pz_ConnItem->z_Link);
  }while(0);

  if ( 0 != q_CheckPt )
  {
    QMI_BIT_MSG_ERROR("Could not allocate a new Connection Item: %d",
                      q_CheckPt,0,0);
  }
  return z_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  A utility function to free the ConnectionInfoItems. This function does not free
  up the item from the heap. It stores it in a queue for future use, so 
  we dont waste time in allocation over heap and cause fragmentation by 
  repeated alloc/free of memory on heap.

@param[in|out]    pz_CltInfo    The BIT-Service which is terminating a 
                                Connection. The Connection Q is updated. 
@param[in]       t_SessionHandle    Session Handle allocated by BIT Service 
                                    for which a ConnectionInfoItem has to
                                    be allocated

@retval    Session Handle that was dellocated. If the Session Handle was not 
           found an invalid Session Handle is returned;
*//* ------------------------------------------------------------------------*/
static gpsone_bit_session_handle_type qmiBitDeallocateConnectionInfoItem
( 
  qmiBitCltInfoStructT           *pz_CltInfo,
  uint64                          t_QmiBitSessionHandle
)
{
  gpsone_bit_session_handle_type z_RetVal = GPSONE_BIT_INVALID_SESSION_HANDLE;
  qmiBitConnectionInfoStructT* pz_Buf= NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NOT DOING parameter check, this is called internally from this module */

  pz_Buf = (qmiBitConnectionInfoStructT*)q_linear_search(
                  &(pz_CltInfo->z_ConnectionQ),
                  qmiBitCompareConnectionItemQMISessionHandle,
                  (void*) &t_QmiBitSessionHandle );
  
  if (  ( NULL != pz_Buf ) &&
        ( FALSE  != q_delete_ext ( &(pz_CltInfo->z_ConnectionQ), 
                                 &(pz_Buf->z_Link) ) ))
  {
    /* Free the Data-Buffers and its contents */

    /* NOTE: This API also destroys the Queue */
    dsm_queue_destroy(&(pz_Buf->z_Wmk));

    dsm_free_packet(&(pz_Buf->pz_DsmItem));
    
    /* Return value is the session Handle that was deallocated */
    z_RetVal = (gpsone_bit_session_handle_type)pz_Buf->w_ConnectionId;
    
    q_put ( &(z_QmiBit.z_ConnectionFreeQ), &(pz_Buf->z_Link) );
  }

  if ( GPSONE_BIT_INVALID_SESSION_HANDLE == z_RetVal )
  {
    QMI_BIT_MSG_ERROR("Could not Deallocate ConnectionInfo Buffer",0,0,0);
  }
  
  return z_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  Function to be called when a Request is Aborted. 

@param[in]    pz_ReqItem   The Item will be aborted. Any handling of requests
                           that need to be aborted needs to go in here. 

@warning
  Should not reach here, as RequestItems are allocated and immediately released
  after the request is sent to the BIT-Service. This is just present to raise a
  flag if we reach here.
*//* ------------------------------------------------------------------------*/
static void qmiBitAbortRequestItem
(
  qmiBitReqInfoStructT* pz_ReqItem 
)
{
  /* For now there is nothing to do */
  QMI_BIT_ERR_FATAL("No Logic to handle Abort-Request-Item",0,0,0);
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to be called when a Connection is Aborted

@param[in]    pz_ReqItem   The Item will be aborted. Any handling of requests
                           that need to be aborted needs to go in here. 
*//* ------------------------------------------------------------------------*/
static void qmiBitAbortConnection
(
  qmiBitConnectionInfoStructT* pz_ConnectionItem
)
{
  /* For now there is nothing to do */
  QMI_BIT_MSG_HIGH( "Abort-Connection/Session QmiSess:%d GpsSess:%d State:%d", 
                    pz_ConnectionItem->t_SessionHandle,
                    pz_ConnectionItem->w_ConnectionId, 
                    pz_ConnectionItem->e_ConnectionState );
}

/* -----------------------------------------------------------------------*//**
@brief
  This function is to be called to flush out connection Items, which are 
  pending a response from the BIT-Provider. It will flush out the items if they
  have been waiting for some time. 

@param[in|out]    pz_CltInfo   The client for whom the connections need to be
                               flushed. 
*//* ------------------------------------------------------------------------*/
static void qmiBitFlushUnrespondedDisconnects
(
  qmiBitCltInfoStructT* pz_CltInfo
)
{
  q_type *pz_ConnQ = &(pz_CltInfo->z_ConnectionQ);
  qmiBitConnectionInfoStructT *pz_CurrConnection = NULL;
  qmiBitConnectionInfoStructT *pz_DeleteConnItem = NULL;
  uint64  t_CurrTime = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  t_CurrTime = cgps_TimeTickGetMsec();
  
  pz_CurrConnection = q_check(pz_ConnQ);

  while ( pz_CurrConnection != NULL )
  {
    /* Initialize the Item to be deleted to NULL */
    pz_DeleteConnItem = NULL;
    
    if ( QMI_BIT_CONNECTION_STATE_DISCONNECTING == 
                    pz_CurrConnection->e_ConnectionState )
    {
      /* Assumption of no rollovers within a power cycle, uint64 
         should be long enough */
      if ( ( t_CurrTime - pz_CurrConnection->t_DisconnectSentTime )
                  > QMI_BIT_DISCONNECT_RESPONSE_TIMEOUT )
      {
        pz_DeleteConnItem = pz_CurrConnection;
      }
    }

    /* Go to the next Item */
    pz_CurrConnection = q_next(pz_ConnQ, &(pz_CurrConnection->z_Link) );

    /* Delete if there was one that has to be deleted */
    if ( NULL != pz_DeleteConnItem )
    {
      QMI_BIT_MSG_ERROR( "Connection Item (TimedOut) %d %d %d",
                    t_CurrTime,
                    pz_DeleteConnItem->t_DisconnectSentTime,
                    pz_DeleteConnItem->t_SessionHandle );
      
      /* NOTE: No need to notify GPS-Bit, as this is a case of flushing it out 
               and more over a successful Disconnect Result was 
               given to GPS-Bit when it was requested */
               
      qmiBitDeallocateConnectionInfoItem( pz_CltInfo, 
                                          pz_DeleteConnItem->t_SessionHandle);
    }
  }

}

/* -----------------------------------------------------------------------*//**
@brief
  Function to be called when a Pending transaction ID needs to be saved.

@param[in]    q_TransactionId   TransactionID that needs to be saved

@retval       TRUE: Transaction ID was saved 
@retval       FALSE: Transaction ID could not be saved 

@warning
  Assumes that a unique transaction ID is allocated and sent in. This function 
  does not ensure that a transaction ID is not duplicate. 
*//* ------------------------------------------------------------------------*/
static boolean qmiBitSavePendingTransactionID
(
  uint32 q_TransactionId
)
{
  boolean b_RetVal= FALSE;
  uint32  q_Cntr = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  for ( q_Cntr = 0;   
        q_Cntr < ARR_SIZE(z_QmiBit.pz_PendingTransactions); 
        q_Cntr++ )
  {
    if ( FALSE == z_QmiBit.pz_PendingTransactions[q_Cntr].b_IsValid )
    {
      /* Found an empty spot to save */
      z_QmiBit.pz_PendingTransactions[q_Cntr].b_IsValid = TRUE;
      z_QmiBit.pz_PendingTransactions[q_Cntr].q_TransactionId = q_TransactionId;
      b_RetVal = TRUE;
      break;
    }
  }
  QMI_BIT_MSG_LOW( "qmiBitSavePendingTransactionID TxId:%d, b_RetVal:%d", 
                    q_TransactionId, b_RetVal, 0 );

  return b_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to be called when a transaction ID needs to be removed from the 
  saved list.

@param[in]    q_TransactionId   Transaction ID that needs to be removed. 
*//* ------------------------------------------------------------------------*/
static void qmiBitRemovePendingTransactionID
(
  uint32 q_TransactionId
)
{
  uint32  q_Cntr = 0;
  boolean b_Status = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( q_Cntr = 0;   
        q_Cntr < ARR_SIZE(z_QmiBit.pz_PendingTransactions); 
        q_Cntr++ )
  {
    if ( ( TRUE == z_QmiBit.pz_PendingTransactions[q_Cntr].b_IsValid ) &&
          ( q_TransactionId == 
                    z_QmiBit.pz_PendingTransactions[q_Cntr].q_TransactionId  ) )
    {
      /* Found the transaction to be removed */
      z_QmiBit.pz_PendingTransactions[q_Cntr].b_IsValid = FALSE;
      b_Status = TRUE;
      break;
    }
  }
        
  QMI_BIT_MSG_LOW( "qmiBitRemovePendingTransactionID TxId:%d b_Status:%d", 
                    q_TransactionId, b_Status, 0 );
  
}

/* -----------------------------------------------------------------------*//**
@brief
  Function to be called to see if a transaction ID is present in the saved list. 

@param[in]    q_TransactionId   Transaction ID that needs to verified

@retval   TRUE  - Transaction ID is present
@retval   FALSE - Transaction ID is not present
*//* ------------------------------------------------------------------------*/
static boolean qmiBitIsPendingTransactionIDPresent
(
  uint32 q_TransactionId
)
{
  uint32  q_Cntr = 0;
  boolean b_RetVal = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  for ( q_Cntr = 0;   
        q_Cntr < ARR_SIZE(z_QmiBit.pz_PendingTransactions); 
        q_Cntr++ )
  {
    if ( ( TRUE == z_QmiBit.pz_PendingTransactions[q_Cntr].b_IsValid ) &&
          ( q_TransactionId == 
                    z_QmiBit.pz_PendingTransactions[q_Cntr].q_TransactionId  ) )
    {
      /* Found the transactionID */
      b_RetVal = TRUE;
      break;
    }
  }

  QMI_BIT_MSG_LOW( "qmiBitIsPendingTransactionIDPresent TxId:%d, b_RetVal:%d", 
                    q_TransactionId, b_RetVal, 0 );

  return b_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  A Generic BIT-Response Handler. The only function of this response handler 
  is to forward the response to BIT-Api if there was an error. If there 
  was no error then the final result will come later on in an indication. 

  It returns back the error that needs to be sent back in the Ack.

@param[in]   pz_CltInfo             The QMI-Client for which response is being 
                                    handled
@param[in]   e_BitEvent             The Event that should be used to notify 
                                    BIT-Api
@param[in]   pz_Resp                The Incoming Result from the QMI-BIT-Service
@param[in]   z_BitTransportHandle   The Transport handle associated with the event
@param[in]   t_BitSessionHandle     The Session handle assocuated with the event   
@param[in]   q_TransactionId        Transaction ID associated with the event

@retval    QMI_ERR_NONE_V01     Not notified to BIT-Api as a final result 
                                is awaited. 
@retval    QMI_ERR..            Refer to qmi_error_type_v01 for error values
*//* ------------------------------------------------------------------------*/
static qmi_error_type_v01 qmiBitGenericResponseHandler
(
  qmiBitCltInfoStructT                     *pz_CltInfo,
  gpsone_bit_event_e_type                   e_BitEvent,
  const qmi_response_type_v01              *pz_Resp,
  gpsone_bit_transport_handle_type          z_BitTransportHandle,
  uint64                                    t_BitSessionHandle,
  uint32                                    q_TransactionId  
)
{
  gpsone_bit_event_payload_type z_GpsEventPayload; 
  gpsone_bit_status_e_type e_Status;
  qmi_error_type_v01 e_RetVal = QMI_ERR_INTERNAL_V01;
  qmiBitConnectionInfoStructT *pz_ConnectionInfo;
  gpsone_bit_session_handle_type e_GpsBitSessionHandle = 
                                      GPSONE_BIT_INVALID_SESSION_HANDLE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Need to notify BIT-API only if there is an error */ 
  do 
  {
    if ( QMI_RESULT_SUCCESS_V01 == pz_Resp->result )
    {
      /* No action to be taken just send the Ack Back */
      e_RetVal = QMI_ERR_NONE_V01;
      break;
    }

    memset( &z_GpsEventPayload, 0, sizeof(z_GpsEventPayload ) );

    /* Valid Session Handle is not present for OPEN/CLOSE/CONNECT */ 
    if ( ( GPSONE_BIT_EVENT_OPEN_RESULT != e_BitEvent ) &&
         ( GPSONE_BIT_EVENT_CLOSE_RESULT != e_BitEvent ) &&
         ( GPSONE_BIT_EVENT_CONNECT_RESULT != e_BitEvent )  )
    {
      pz_ConnectionInfo = 
                          (qmiBitConnectionInfoStructT*)q_linear_search(
                                      &(pz_CltInfo->z_ConnectionQ),
                                      qmiBitCompareConnectionItemQMISessionHandle,
                                      (void*) &(t_BitSessionHandle));
      if ( NULL == pz_ConnectionInfo )
      {
        /* Could not find the session hence invalid Arg */
        e_RetVal = QMI_ERR_INVALID_ARG_V01;
        break;
      }
      e_GpsBitSessionHandle = 
              (gpsone_bit_session_handle_type)pz_ConnectionInfo->w_ConnectionId;
    }

    z_GpsEventPayload.event = e_BitEvent;
    z_GpsEventPayload.result = 
        (gpsone_bit_status_e_type)QMI_BIT_GET_GPS_BIT_VAL(
                                        pz_qmiBitStatusToQmiErrorMap,
                                        pz_Resp->error);
    e_Status = gpsone_bit_notify(  z_BitTransportHandle,
                                   e_GpsBitSessionHandle, 
                                   q_TransactionId, 
                                   &z_GpsEventPayload); 
    
    e_RetVal = 
        (qmi_error_type_v01)QMI_BIT_GET_QMI_BIT_VAL(pz_qmiBitStatusToQmiErrorMap,e_Status);

    
  } while(0);

  return e_RetVal;
}
  


/*============================================================================
 *                    B I T - A P I    C A L L B A C K S
 * =========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Callback registered with Legacy BIT-API.
  
  -- Start:Description from BIT-API For Reference --
  This function is used to inform BIT daemon that PDSM will start using its
  service. If for some reason, BIT daemon fails to initialize, it should return 
  GPSONE_BIT_STATUS_FAIL or other error code. Otherwise, it should return 
  GPSONE_BIT_STATUS_WAIT to inform PDSM that BIT daemon is in the process of being 
  brought up and open result will be sent asynchronously via gpsone_bit_notify
  with OPEN_RESULT event. 

  If BIT daemon can finish the open operation immediately, it can return 
  GPSONE_BIT_STATUS_SUCCESS immediately without further event notification.
  -- End:Description from BIT-API For Reference --


@param[in]   z_TransportHandle  Transport Handle assigned 
@param[in]   pz_OpenParam       Parameters for the Bit-Service to prepare 
                                itself.

@retval    GPSONE_BIT_STATUS_WAIT  Message has been sent to the Bit-Service
                                   and awaiting result. 
@retval    GPSONE_BIT_STATUS_...   Look into the enumeration gpsone_bit_status_e_type
                                   for the error values.

@warning  This function is written with the assumption that the next 
          qmiBitOpenCb()will be preceded by qmiBitCloseCb() shall been called. 
          Back to back calls to this Callback might cause some inexplicable 
          behavior. QMI_BIT_OPEN_RESP_V01 handling also will have to be 
          changed if the assumption is invalidated in the future. In case
          multiple calls are made, then we will have to save the handle to the 
          request so it can be matched later on. And more information is needed
          in the request handle to achieve this. Also probably have multiple
          transaction IDs as well. 

          For now since we support only one QMI-BIT-Service, this assumption 
          should be fine.
         
          
@see      qmiBitHandleOpenResp(), A Response is expected for the Request sent
          to the BIT Service.
*//* ------------------------------------------------------------------------*/
gpsone_bit_status_e_type qmiBitOpenCb
(
  gpsone_bit_transport_handle_type      z_TransportHandle,
  const gpsone_bit_open_params_type     *pz_OpenParam
)
{
  gpsone_bit_status_e_type    e_RetVal = GPSONE_BIT_STATUS_FAIL;
  uint32 q_CheckPt = 0;
  qmiBitCltInfoStructT *pz_CltInfo = NULL;
  qmiBitReqInfoStructT *pz_ReqItem = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /* Find the Client associated with this TransportHandle in the queue.
       Transport Handle is allocated when QMI_BIT_SERVICE_READY_IND_V01 is 
       received and we register with BIT-Api. */
    pz_CltInfo = 
         (qmiBitCltInfoStructT*) q_linear_search( &z_QmiBit.z_ClientQ,
                                        qmiBitCompareTransportHandle,
                                        (void*)&z_TransportHandle );
    /* Did not find the Client in the Queue */
    if ( NULL == pz_CltInfo )
    {
      e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
      q_CheckPt = 1; 
      break;
    }

    pz_ReqItem = qmiBitAllocateRequestItem(pz_CltInfo);
    if ( NULL == pz_ReqItem )
    {
      e_RetVal = GPSONE_BIT_STATUS_NO_RESOURCES;
      q_CheckPt = 2; 
      break;
    }

    pz_ReqItem->z_Req.z_OpenReq.transaction_id = QMI_BIT_TRANSACTION_ID_OPEN;
    pz_ReqItem->z_Msg.q_MsgId = QMI_BIT_OPEN_REQ_V01;
    pz_ReqItem->z_Msg.p_Msg = (void*)&(pz_ReqItem->z_Req.z_OpenReq);
    pz_ReqItem->z_Msg.q_MsgLen = sizeof(pz_ReqItem->z_Req.z_OpenReq);

    if ( FALSE == qmiBitSendReq(
                              pz_CltInfo,
                              &(pz_ReqItem->z_Msg ) ) )
    {
      e_RetVal = GPSONE_BIT_STATUS_IPC_FAILURE;
      q_CheckPt = 3;
      break;
    }
    
    /* We have to wait as the message is sent across and we need the response 
       message from the server to let us know of the outcome */
    e_RetVal = GPSONE_BIT_STATUS_WAIT;
  } while(0);

  if ( NULL != pz_ReqItem )
  {
    /* Free the Handle to the ReqItem, as it is not needed now. We sent the 
       message */
    qmiBitDeallocateRequestItem(&pz_ReqItem);
  }

  if ( 0 != q_CheckPt )
  {
    QMI_BIT_MSG_ERROR("OpenCb:%d, q_CheckPt:%d",e_RetVal, q_CheckPt, 0 );
  }
  return e_RetVal;
}


/* -----------------------------------------------------------------------*//**
@brief
  Callback registered with Legacy BIT-API.
  
  -- Start:Description from BIT-API For Reference --
  This function is used to inform BIT daemon that PDSM will stop using its
  service. If for some reason, BIT daemon fails to close, it should return 
  GPSONE_BIT_STATUS_FAIL or other error code. Otherwise, it should return 
  GPSONE_BIT_STATUS_WAIT to inform PDSM that BIT daemon is in the process of 
  being shut down and close result will be sent asynchronously via 
  gpsone_bit_notify with CLOSE_RESULT event.

  If BIT daemon can finish the close operation immediately, it can return 
  GPSONE_BIT_STATUS_SUCCESS immediately without further event notification.
  -- End:Description from BIT-API For Reference --


@param[in]   z_TransportHandle  Transport Handle assigned 
@param[in]   pz_CloseParam      Parameters for the Bit-Service to handle
                                close.

@retval    GPSONE_BIT_STATUS_WAIT  Message has been sent to the Bit-Service
                                   and awaiting result. 
@retval    GPSONE_BIT_STATUS_...   Look into the enumeration gpsone_bit_status_e_type
                                   for the error values.
*//* ------------------------------------------------------------------------*/
gpsone_bit_status_e_type qmiBitCloseCb
(
  gpsone_bit_transport_handle_type           z_TransportHandle,
  const gpsone_bit_close_params_type        *pz_CloseParam
)
{
  gpsone_bit_status_e_type    e_RetVal = GPSONE_BIT_STATUS_FAIL;
  uint32 q_CheckPt = 0;
  qmiBitCltInfoStructT *pz_CltInfo = NULL;
  qmiBitReqInfoStructT *pz_ReqItem = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /* Find the Connection Handle in the queue */
    pz_CltInfo = 
         (qmiBitCltInfoStructT*) q_linear_search( &z_QmiBit.z_ClientQ,
                                        qmiBitCompareTransportHandle,
                                        (void*)&z_TransportHandle );
    /* Did not find the Client in the Queue */
    if ( NULL == pz_CltInfo )
    {
      e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
      q_CheckPt = 1; 
      break;
    }

    pz_ReqItem = qmiBitAllocateRequestItem(pz_CltInfo);
    if ( NULL == pz_ReqItem )
    {
      e_RetVal = GPSONE_BIT_STATUS_NO_RESOURCES;
      q_CheckPt = 2; 
      break;
    }

    pz_ReqItem->z_Req.z_CloseReq.transaction_id = QMI_BIT_TRANSACTION_ID_CLOSE;
    pz_ReqItem->z_Msg.q_MsgId = QMI_BIT_CLOSE_REQ_V01;
    pz_ReqItem->z_Msg.p_Msg = (void*)&(pz_ReqItem->z_Req.z_CloseReq);
    pz_ReqItem->z_Msg.q_MsgLen = sizeof(pz_ReqItem->z_Req.z_CloseReq);

    if ( FALSE == qmiBitSendReq(
                              pz_CltInfo,
                              &(pz_ReqItem->z_Msg ) ) )
    {
      e_RetVal = GPSONE_BIT_STATUS_IPC_FAILURE;
      q_CheckPt = 3;
      break;
    }
    
    /* We have to wait as the message is sent across and we need the response 
       message from the server to let us know of the outcome */
    e_RetVal = GPSONE_BIT_STATUS_WAIT;
  } while(0);


  if ( NULL != pz_ReqItem )
  {
    /* Free the Handle to the ReqItem, as it is not needed now. We sent the 
       message */
    qmiBitDeallocateRequestItem(&pz_ReqItem);
  }


  if ( 0 != q_CheckPt )
  {
    QMI_BIT_MSG_ERROR("CloseCb:%d, q_CheckPt:%d",e_RetVal, q_CheckPt, 0 );
  }
  return e_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Callback registered with Legacy BIT-API.
  
  -- Start:Description from BIT-API For Reference --
  This function is used to establish end-to-end connection to a network server. 
  If for some reason, end-to-end connection can not be established, it should 
  return GPSONE_BIT_STATUS_FAIL or other error code. therwise, it should return 
  GPSONE_BIT_STATUS_WAIT to inform PDSM that end-to-end connection is in the 
  process of being established. Connect result should be sent asynchronously 
  via gpsone_bit_notify with CONNECT_RESULT event. 

  If BIT daemon can finish the connect operation immediately, it can return 
  GPSONE_BIT_STATUS_SUCCESS immediately without further event notification.
  -- End:Description from BIT-API For Reference --


@param[in]   z_TransportHandle  Transport Handle assigned 
@param[in]   q_TransactionId    Transaction ID for this transaction.
@param[in]   pz_CloseParam      Parameters for the Bit-Service to handle
                                close.

@retval    GPSONE_BIT_STATUS_WAIT  Message has been sent to the Bit-Service
                                   and awaiting result. 
@retval    GPSONE_BIT_STATUS_...   Look into the enumeration gpsone_bit_status_e_type
                                   for the error values.
*//* ------------------------------------------------------------------------*/
gpsone_bit_status_e_type qmiBitConnectCb
(
  gpsone_bit_transport_handle_type           z_TransportHandle,
  uint32                                     q_TransactionId,
  const gpsone_bit_connect_params_type      *pz_ConnectParam
)
{
  gpsone_bit_status_e_type    e_RetVal = GPSONE_BIT_STATUS_FAIL;
  uint32 q_CheckPt = 0;
  qmiBitCltInfoStructT *pz_CltInfo = NULL;
  qmiBitReqInfoStructT *pz_ReqItem = NULL;
  int32                 i_TmpVal; 
  bit_host_info_struct_type_v01 z_HostInfo;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /* Find the Connection Handle in the queue */
    pz_CltInfo = 
         (qmiBitCltInfoStructT*) q_linear_search( &z_QmiBit.z_ClientQ,
                                        qmiBitCompareTransportHandle,
                                        (void*)&z_TransportHandle );
    /* Did not find the Client in the Queue */
    if ( NULL == pz_CltInfo ) 
    {
      e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
      q_CheckPt = 1; 
      break;
    }

    if ( FALSE == qmiBitSavePendingTransactionID(q_TransactionId) )
    {
      /* Could not save the transaction ID */
      e_RetVal = GPSONE_BIT_STATUS_NO_RESOURCES;
      q_CheckPt = 2; 
      break;
    }

    pz_ReqItem = qmiBitAllocateRequestItem(pz_CltInfo);
    if ( NULL == pz_ReqItem )
    {
      e_RetVal = GPSONE_BIT_STATUS_NO_RESOURCES;
      q_CheckPt = 3; 
      break;
    }

    
    pz_ReqItem->z_Req.z_ConnectReq.transaction_id = q_TransactionId;
    pz_ReqItem->z_Req.z_ConnectReq.link =
              (bit_link_enum_type_v01)QMI_BIT_GET_QMI_BIT_VAL(
                                        pz_qmiBitLinkMap,
                                        pz_ConnectParam->link_type);

    i_TmpVal = QMI_BIT_GET_QMI_BIT_VAL(pz_qmiBitProtocolMap,
                                       pz_ConnectParam->protocol_type);
    QMI_BIT_SET_OPTIONAL_VALUE ( pz_ReqItem->z_Req.z_ConnectReq.protocol,
                                 ((bit_protocol_enum_type_v01)i_TmpVal));
    memset ( &z_HostInfo, 0, sizeof(z_HostInfo) );
    if ( (GPSONE_BIT_IP_ADDR == pz_ConnectParam->adr_type ) ||
         (GPSONE_BIT_HOST_NAME_AND_IP_ADDR == pz_ConnectParam->adr_type ) )
    {
      /* At this point either V4 or V6 is present */
      if ( GPSONE_BIT_IP_V4 == pz_ConnectParam->ip_addr.type )
      {
        z_HostInfo.validity_mask |= BIT_MASK_IPV4_ADDR_AND_PORT_V01;
        z_HostInfo.ipv4_addr = pz_ConnectParam->ip_addr.addr.v4_addr;
        z_HostInfo.ipv4_port = pz_ConnectParam->ip_port;
      }
      else /* GPSONE_BIT_IP_V6 */
      {
        z_HostInfo.validity_mask |= BIT_MASK_IPV6_ADDR_AND_PORT_V01;
        (void) gps_Memcpy( sizeof(z_HostInfo.ipv6_addr),
                      (void *)z_HostInfo.ipv6_addr,
                      sizeof(pz_ConnectParam->ip_addr.addr.v6_addr),
                      (void *)pz_ConnectParam->ip_addr.addr.v6_addr );
        z_HostInfo.ipv6_port = pz_ConnectParam->ip_port;
      }
    }

    if ( (GPSONE_BIT_HOST_NAME == pz_ConnectParam->adr_type ) ||
         (GPSONE_BIT_HOST_NAME_AND_IP_ADDR == pz_ConnectParam->adr_type ) )
    {
      z_HostInfo.validity_mask |= BIT_MASK_HOST_URL_AND_PORT_V01;    
      (void) gps_Memcpy( sizeof(z_HostInfo.url),
                           (void *)z_HostInfo.url,
                           sizeof(pz_ConnectParam->host_name),
                           (void *) pz_ConnectParam->host_name );
      z_HostInfo.url_port = pz_ConnectParam->ip_port;
    }
    
    QMI_BIT_SET_OPTIONAL_VALUE ( pz_ReqItem->z_Req.z_ConnectReq.host_info,
                                 z_HostInfo );
    pz_ReqItem->z_Msg.q_MsgId = QMI_BIT_CONNECT_REQ_V01;
    pz_ReqItem->z_Msg.p_Msg = (void*)&(pz_ReqItem->z_Req.z_ConnectReq);
    pz_ReqItem->z_Msg.q_MsgLen = sizeof(pz_ReqItem->z_Req.z_ConnectReq);

    if ( FALSE == qmiBitSendReq(
                              pz_CltInfo,
                              &(pz_ReqItem->z_Msg ) ) )
    {
      e_RetVal = GPSONE_BIT_STATUS_IPC_FAILURE;
      q_CheckPt = 4;
      break;
    }
    
    /* We have to wait as the message is sent across and we need the response 
       message from the server to let us know of the outcome */
    e_RetVal = GPSONE_BIT_STATUS_WAIT;
  } while(0);

  if ( NULL != pz_ReqItem )
  {
    /* Free the Handle to the ReqItem, as it is not needed now. We sent the 
       message */
    qmiBitDeallocateRequestItem(&pz_ReqItem);
  }

  if ( 0 != q_CheckPt )
  {
    QMI_BIT_MSG_ERROR("ConnectCb:%d, q_CheckPt:%d",e_RetVal, q_CheckPt, 0 );
  }
  return e_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Callback registered with Legacy BIT-API.
  
  -- Start:Description from BIT-API For Reference --
  This function is used to tear down end-to-end connection to a network server. 
  If for some reason, end-to-end connection can not be torn down, it should 
  return GPSONE_BIT_STATUS_FAIL. Otherwise, it should return GPSONE_BIT_STATUS_WAIT 
  to inform PDSM that end-to-end connection is in the process of being torn down.
  Disconnect result should be sent asynchronously via gpsone_bit_notify with 
  CONNECT_RESULT event. 

  If BIT daemon can finish the disconnect operation immediately, it can return 
  GPSONE_BIT_STATUS_SUCCESS immediately without further event notification.
  -- End:Description from BIT-API For Reference --


@param[in]   z_TransportHandle  Transport Handle assigned 
@param[in]   z_SessionHandle    Session Handle for the session which needs 
                                to be closed. 
@param[in]   q_TransactionId    Transaction ID for this transaction.
@param[in]   pz_DisconnectParam Parameters for the Bit-Service to handle
                                close of a session.

@retval    GPSONE_BIT_STATUS_WAIT  Message has been sent to the Bit-Service
                                   and awaiting result. 
@retval    GPSONE_BIT_STATUS_...   Look into the enumeration gpsone_bit_status_e_type
                                   for the error values.
*//* ------------------------------------------------------------------------*/
gpsone_bit_status_e_type qmiBitDisconnectCb
(
  gpsone_bit_transport_handle_type           z_TransportHandle,
  gpsone_bit_session_handle_type             z_SessionHandle,
  uint32                                     q_TransactionId,
  const gpsone_bit_disconnect_params_type   *pz_DisconnectParam
)
{
  gpsone_bit_status_e_type    e_RetVal = GPSONE_BIT_STATUS_FAIL;
  uint32 q_CheckPt = 0;
  qmiBitCltInfoStructT *pz_CltInfo = NULL;
  qmiBitReqInfoStructT *pz_ReqItem = NULL;
  qmiBitConnectionInfoStructT *pz_ConnItem = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /* Find the Transport Handle in the queue */
    pz_CltInfo = 
         (qmiBitCltInfoStructT*) q_linear_search( &z_QmiBit.z_ClientQ,
                                        qmiBitCompareTransportHandle,
                                        (void*)&z_TransportHandle );
    /* Did not find the Client in the Queue */
    if ( NULL == pz_CltInfo )
    {
      e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
      q_CheckPt = 1; 
      break;
    }

    /* Find the connection Item for the (16-bit) BIT-Session Handle  */
    pz_ConnItem = (qmiBitConnectionInfoStructT*)q_linear_search(
                      &(pz_CltInfo->z_ConnectionQ),
                      qmiBitCompareConnectionItemBITSessionHandle,
                      (void*) &(z_SessionHandle) );
    if ( NULL == pz_ConnItem )
    {
      e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
      q_CheckPt = 2;
      break;
    }


    pz_ReqItem = qmiBitAllocateRequestItem(pz_CltInfo);
    if ( NULL == pz_ReqItem )
    {
      e_RetVal = GPSONE_BIT_STATUS_NO_RESOURCES;
      q_CheckPt = 3; 
      break;
    }



    pz_ReqItem->z_Req.z_DisconnectReq.transaction_id = q_TransactionId;
    pz_ReqItem->z_Req.z_DisconnectReq.session_handle = 
                                                  pz_ConnItem->t_SessionHandle;

    pz_ReqItem->z_Msg.q_MsgId = QMI_BIT_DISCONNECT_REQ_V01;
    pz_ReqItem->z_Msg.p_Msg = (void*)&(pz_ReqItem->z_Req.z_DisconnectReq);
    pz_ReqItem->z_Msg.q_MsgLen = sizeof(pz_ReqItem->z_Req.z_DisconnectReq);

    if ( FALSE == qmiBitSendReq(
                              pz_CltInfo,
                              &(pz_ReqItem->z_Msg ) ) )
    {
      e_RetVal = GPSONE_BIT_STATUS_IPC_FAILURE;
      q_CheckPt = 4;
      break;
    }

    /* Move the state to Disconnecting to indicate that a request was 
       sent to the client. In case the client does not respond we can 
       free the memory when we see it apt for flushing */
    pz_ConnItem->e_ConnectionState = QMI_BIT_CONNECTION_STATE_DISCONNECTING;

    /* Save the time when Disconnect was sent so it can be used when 
       this item needs to be flushed */
    pz_ConnItem->t_DisconnectSentTime = cgps_TimeTickGetMsec();
    pz_ConnItem->q_DisconnectTransactionId = q_TransactionId;
    
    /* Don't Have to wait as this is disconnect, lets report back to the 
       engine that disconnect has happened and is successful. */
    e_RetVal = GPSONE_BIT_STATUS_SUCCESS;
  } while(0);

  if ( NULL != pz_ReqItem )
  {
    /* Free the Handle to the ReqItem, as it is not needed now. We sent the 
       message */
    qmiBitDeallocateRequestItem(&pz_ReqItem);
  }

  if ( 0 != q_CheckPt )
  {
    QMI_BIT_MSG_ERROR("DisconnectCb:%d, q_CheckPt:%d",e_RetVal, q_CheckPt, 0 );
  }
  return e_RetVal;
}
  
/* -----------------------------------------------------------------------*//**
@brief
  Callback registered with Legacy BIT-API.
  
  -- Start:Description from BIT-API For Reference --
  This function is used by PDSM to send data to the network. It passes a pointer 
  to a data buffer (*send_buf) and also specifies the size (in bytes) of the data buffer. 
  If for some reason, BIT daemon is not able to process the send request, 
  it should return GPSONE_BIT_STATUS_FAIL or other error code to inform PDSM that the 
  send request has failed. Otherwise, it should return GPSONE_BIT_STATUS_WAIT and return
  result asynchronously via gpsone_bit_notify() with SEND_RESULT event.

  If BIT daemon can finish the send operation immediately, it can return 
  GPSONE_BIT_STATUS_SUCCESS immediately without further event notification.
  -- End:Description from BIT-API For Reference --


@param[in]   z_TransportHandle  Transport Handle assigned 
@param[in]   z_SessionHandle    Session Handle for the session which needs 
                                to be closed. 
@param[in]   q_TransactionId    Transaction ID for this transaction.
@param[in]   pb_SendBuf         The byte stream that needs to be sent to the
                                BIT-Service
@param[in]   q_Length           The length of the buffer                                

@retval    GPSONE_BIT_STATUS_WAIT  Message has been sent to the Bit-Service
                                   and awaiting result. 
@retval    GPSONE_BIT_STATUS_...   Look into the enumeration gpsone_bit_status_e_type
                                   for the error values.

@warning
  This callback cannot accept data more than  BIT_CONST_PAYLOAD_LEN_MAX_V01. 
  (a) if we do accept more then we need to send it out in multiple messages.
      How do we allocate transactionIDs. How do we report status back to the 
      gpsone-bit ? 
*//* ------------------------------------------------------------------------*/
gpsone_bit_status_e_type qmiBitSendCb
(
  gpsone_bit_transport_handle_type           z_TransportHandle,
  gpsone_bit_session_handle_type             z_SessionHandle,
  uint32                                     q_TransactionId,
  uint8                                     *pb_SendBuf,
  uint32                                     q_Length
)
{
  gpsone_bit_status_e_type    e_RetVal = GPSONE_BIT_STATUS_FAIL;
  uint32 q_CheckPt = 0;
  qmiBitCltInfoStructT *pz_CltInfo = NULL;
  qmiBitReqInfoStructT *pz_ReqItem = NULL;
  qmiBitConnectionInfoStructT *pz_ConnItem = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /* Find the Connection Handle in the queue */
    pz_CltInfo = 
         (qmiBitCltInfoStructT*) q_linear_search( &z_QmiBit.z_ClientQ,
                                        qmiBitCompareTransportHandle,
                                        (void*)&z_TransportHandle );
    /* Did not find the Client in the Queue */
    if ( ( NULL == pz_CltInfo ) ||
         ( 0 == q_Length ) ||
         ( q_Length > BIT_CONST_PAYLOAD_LEN_MAX_V01 ) )
    {
      e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
      q_CheckPt = 1; 
      break;
    }

    /* Find the connection Item for the (16-bit) BIT-Session Handle  */
    pz_ConnItem = (qmiBitConnectionInfoStructT*)q_linear_search(
                      &(pz_CltInfo->z_ConnectionQ),
                      qmiBitCompareConnectionItemBITSessionHandle,
                      (void*) &(z_SessionHandle) );
    if ( NULL == pz_ConnItem )
    {
      e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
      q_CheckPt = 2;
      break;
    }


    pz_ReqItem = qmiBitAllocateRequestItem(pz_CltInfo);
    if ( NULL == pz_ReqItem )
    {
      e_RetVal = GPSONE_BIT_STATUS_NO_RESOURCES;
      q_CheckPt = 3; 
      break;
    }


    pz_ReqItem->z_Req.z_SendReq.transaction_id = q_TransactionId;
    pz_ReqItem->z_Req.z_SendReq.session_handle = 
                                    pz_ConnItem->t_SessionHandle;

    pz_ReqItem->z_Req.z_SendReq.payload_len = q_Length;

    /* Send only if we can send the entire packet across, */
    if ( q_Length > gps_Memcpy( BIT_CONST_PAYLOAD_LEN_MAX_V01,
                             (void *) pz_ReqItem->z_Req.z_SendReq.payload,
                             q_Length,
                             (void*) pb_SendBuf ) )
    {
      e_RetVal = GPSONE_BIT_STATUS_FATAL_ERROR;
      q_CheckPt = 4;
      break;
    }

    pz_ReqItem->z_Msg.q_MsgId = QMI_BIT_SEND_REQ_V01;
    pz_ReqItem->z_Msg.p_Msg = (void*)&(pz_ReqItem->z_Req.z_SendReq);
    pz_ReqItem->z_Msg.q_MsgLen = sizeof(pz_ReqItem->z_Req.z_SendReq);

    if ( FALSE == qmiBitSendReq(
                              pz_CltInfo,
                              &(pz_ReqItem->z_Msg ) ) )
    {
      e_RetVal = GPSONE_BIT_STATUS_IPC_FAILURE;
      q_CheckPt = 5;
      break;
    }

    /* We have to wait as the message is sent across and we need the response 
       message from the server to let us know of the outcome */
    e_RetVal = GPSONE_BIT_STATUS_WAIT;
  } while(0);

  if ( NULL != pz_ReqItem )
  {
    /* Free the Handle to the ReqItem, as it is not needed now. We sent the 
       message */
    qmiBitDeallocateRequestItem(&pz_ReqItem);
  }

  if ( 0 != q_CheckPt )
  {
    QMI_BIT_MSG_ERROR("SendCb:%d, q_CheckPt:%d",e_RetVal, q_CheckPt, 0 );
  }
  return e_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Callback registered with Legacy BIT-API.
  
  -- Start:Description from BIT-API For Reference --
  This function is used by PDSM to receive data from BIT daemon, when it gets
  DATA_READY event from BIT daemon. Upon getting recv request, BIT daemon should
  copy incoming data to the data buffer pointed by *recv_buf. If incoming data 
  size is larger than max_buff_size, it should only copy max_buf_size to the 
  data buffer pointed by *recv_buf. 

  BIT daemon should return the number of bytes copied to the data buffer and the 
  bytes left in its data buffer. BIT daemon should return GPSONE_BIT_STATUS_SUCCESS
  when some bytes are succesfully returned, otherwise an error code should be returned.
  If no data available now, BIT daemon should return GPSONE_BIT_STATUS_NO_MORE_DATA.

  Please note that this is a synchronous call, there is no async event for recv(),
  BIT daemon should handle the recv operation in a non-blocking way.
  -- End:Description from BIT-API For Reference --


@param[in]   z_TransportHandle  Transport Handle assigned 
@param[in]   z_SessionHandle    Session Handle for the session which needs 
                                to be closed. 
@param[in]   q_TransactionId    Transaction ID for this transaction.
@param[in]   pb_RecvBuf         The buffer which needs to be filled in order to
                                send data to the BIT-API.
@param[in]   q_MaxBufSize       The Maximum bytes that can be written to 
                                the buffer given by BIT-API
@param[in]   pq_BytesReturned   Number of bytes that were copied into the buffer
                                for BIT-API
@param[in]   pz_BytesLeftOver   Number of bytes left over for BIT-API to read.
                                

@retval    GPSONE_BIT_STATUS_WAIT  Message has been sent to the Bit-Service
                                   and awaiting result. 
@retval    GPSONE_BIT_STATUS_...   Look into the enumeration gpsone_bit_status_e_type
                                   for the error values.
*//* ------------------------------------------------------------------------*/
gpsone_bit_status_e_type qmiBitRecvCb
(
  gpsone_bit_transport_handle_type           z_TransportHandle,
  gpsone_bit_session_handle_type             z_SessionHandle,
  uint8                                     *pb_RecvBuf,
  uint32                                     q_MaxBufSize,
  uint32                                    *pq_BytesReturned,
  uint32                                    *pq_BytesLeftOver
)
{
  gpsone_bit_status_e_type    e_RetVal = GPSONE_BIT_STATUS_FAIL;
  uint32 q_CheckPt      = 0; 
  uint32 q_BytesToCopy  = 0, /* Uint16 as the watermark library uses uint16 */
         q_BytesPresent = 0,
         q_CurrBytesToCopy = 0;
  qmiBitCltInfoStructT *pz_CltInfo = NULL;
  qmiBitConnectionInfoStructT *pz_ConnItem = NULL;
  uint8  *pb_CurrBuffWriter = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {

    if ( ( NULL == pb_RecvBuf ) ||
         ( NULL == pq_BytesReturned ) ||
         ( NULL == pq_BytesLeftOver ) )
    {
      e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
      q_CheckPt = 1;
      break;
    }
    
    /* Find the Connection Handle in the queue */
    pz_CltInfo = 
         (qmiBitCltInfoStructT*) q_linear_search( &z_QmiBit.z_ClientQ,
                                        qmiBitCompareTransportHandle,
                                        (void*)&z_TransportHandle );
    if ( NULL == pz_CltInfo )
    {
      e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
      q_CheckPt = 2;
      break;
    }

    /* Find the connection Item for the (16-bit) BIT-Session Handle  */
    pz_ConnItem = (qmiBitConnectionInfoStructT*)q_linear_search(
                      &(pz_CltInfo->z_ConnectionQ),
                      qmiBitCompareConnectionItemBITSessionHandle,
                      (void*) &(z_SessionHandle) );
    if ( NULL == pz_ConnItem )
    {
      e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
      q_CheckPt = 3;
      break;
    }

    /* Find the number of bytes in the Buffer */
    q_BytesPresent = qmiBitGetCurrBuffCnt(pz_ConnItem);

    /* We can send as much as is present upto a max of the Buffer given by 
       BIT-API */
    q_BytesToCopy = MIN(q_BytesPresent, q_MaxBufSize);


     /* Set the Writer-location to the start */
    pb_CurrBuffWriter = pb_RecvBuf;

    while ( 0 < q_BytesToCopy )
    {
      /* Ensure that we never garble the memory. To catch issues earlier on */
      ASSERT ( pb_CurrBuffWriter < (pb_RecvBuf + q_MaxBufSize) );

      /* If there is no data in the intermediate buffer lets get some! */
      if ( NULL == pz_ConnItem->pz_DsmItem )
      {
        pz_ConnItem->pz_DsmItem = dsm_dequeue(&(pz_ConnItem->z_Wmk));
      }


      /* Now we copy upto a max of q_Bytes to Copy. */
      q_CurrBytesToCopy = MIN( dsm_length_packet(pz_ConnItem->pz_DsmItem),
                               q_BytesToCopy );


      /* The DSM API uses a 16-bit value, so it is capped to the max 
         that a 16bit unsigned integer can take */
      q_CurrBytesToCopy = MIN( q_CurrBytesToCopy, 0xFFFF );
      

      /* Note: DSM-Pullup will make the pz_DsmItem NULL if it pulled all
               bytes from it */
      if ( q_CurrBytesToCopy != dsm_pullup( &(pz_ConnItem->pz_DsmItem),
                                    pb_CurrBuffWriter,
                                    q_CurrBytesToCopy ) )
      {
        /* Could not copy the bytes that we wanted to copy. Should not reach
           here. Asserting to aid in debug */
        ASSERT(0);
      }

      q_BytesToCopy -= q_CurrBytesToCopy;
      pb_CurrBuffWriter += q_CurrBytesToCopy;
      q_CurrBytesToCopy = 0; /* Resetting just to be safe */
    }

    /* Now we have copied the amount required, lets fill up the rest of 
       data */
    (*pq_BytesReturned) = (pb_CurrBuffWriter-pb_RecvBuf); /* The total 
                               amount written is equivalent to the amount
                               that the BuffWriter advanced from initial */
    (*pq_BytesLeftOver) = qmiBitGetCurrBuffCnt(pz_ConnItem); /* Get the current
                               count again, its possible that there were
                               some enqueues, so always better to 
                               get current */
    e_RetVal = (( (*pq_BytesLeftOver) == 0 ) &&
                ( (*pq_BytesReturned) == 0 ) ) ? GPSONE_BIT_STATUS_NO_MORE_DATA : 
                                              GPSONE_BIT_STATUS_SUCCESS;
  } while(0);

  QMI_BIT_MSG_LOW( "Recv Cb: Bytes Returned:%d, BytesLeftOver:%d CheckPt:%d" ,
                   (*pq_BytesReturned),(*pq_BytesLeftOver),q_CheckPt);

  if ( 0 != q_CheckPt )
  {
    QMI_BIT_MSG_ERROR("OpenCb q_CheckPt:%d", q_CheckPt,0, 0 );
  }
  return e_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  Callback registered with Legacy BIT-API.
  
  -- Start:Description from BIT-API For Reference --
  This function is used by PDSM to request ioctl operation on BIT daemon.
  If for some reason, BIT daemon can't do this ioctl operation, it should return 
  GPSONE_BIT_STATUS_FAIL or other error code. Otherwise, it should return 
  GPSONE_BIT_STATUS_WAIT to inform PDSM that the ioctl operation is in progress.
  Result will be sent back asynchronously via gpsone_bit_notify() later with
  IOCTL_RESULT event.
  -- End:Description from BIT-API For Reference --


@param[in]   z_TransportHandle  Transport Handle assigned 
@param[in]   z_SessionHandle    Session Handle for the session which needs 
                                to be closed. 
@param[in]   q_TransactionId    Transaction ID for this transaction.
@param[in]   e_IoctlType        Identifies the IOCTL that needs to be sent. 
@param[in]   pz_IoctlParam      Parameters required for the IOCTL.
*//* ------------------------------------------------------------------------*/
gpsone_bit_status_e_type qmiBitIoctlCb
(
  gpsone_bit_transport_handle_type           z_TransportHandle,
  gpsone_bit_session_handle_type             z_SessionHandle,
  uint32                                     q_TransactionId,
  gpsone_bit_ioctl_e_type                    e_IoctlType,
  const gpsone_bit_ioctl_params_type        *pz_IoctlParam
)
{
  gpsone_bit_status_e_type    e_RetVal = GPSONE_BIT_STATUS_FAIL;
  uint32 q_CheckPt = 0;
  qmiBitCltInfoStructT *pz_CltInfo = NULL;
  qmiBitReqInfoStructT *pz_ReqItem = NULL;
  qmiBitConnectionInfoStructT *pz_ConnItem = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /* Find the Connection Handle in the queue */
    pz_CltInfo = 
         (qmiBitCltInfoStructT*) q_linear_search( &z_QmiBit.z_ClientQ,
                                        qmiBitCompareTransportHandle,
                                        (void*)&z_TransportHandle );
    /* Did not find the Client in the Queue */
    if ( NULL == pz_CltInfo ) 
    {
      e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
      q_CheckPt = 1; 
      break;
    }

    /* Find the connection Item for the (16-bit) BIT-Session Handle  */
    pz_ConnItem = (qmiBitConnectionInfoStructT*)q_linear_search(
                      &(pz_CltInfo->z_ConnectionQ),
                      qmiBitCompareConnectionItemBITSessionHandle,
                      (void*) &(z_SessionHandle) );
    if ( NULL == pz_ConnItem )
    {
      e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
      q_CheckPt = 2;
      break;
    }


    pz_ReqItem = qmiBitAllocateRequestItem(pz_CltInfo);
    if ( NULL == pz_ReqItem )
    {
      e_RetVal = GPSONE_BIT_STATUS_NO_RESOURCES;
      q_CheckPt = 3; 
      break;
    }

    switch ( e_IoctlType )
    {
      case GPSONE_BIT_IOCTL_FORCE_DORMANCY:
        pz_ReqItem->z_Msg.q_MsgId = QMI_BIT_SET_DORMANCY_REQ_V01;
        pz_ReqItem->z_Req.z_SetDormancyReq.transaction_id = q_TransactionId;
        pz_ReqItem->z_Req.z_GetLocalHostInfoReq.session_handle = 
                                                 pz_ConnItem->t_SessionHandle;
        pz_ReqItem->z_Req.z_SetDormancyReq.dormancy_state = FALSE; /* Dormant */

        pz_ReqItem->z_Msg.p_Msg = (void*)&(pz_ReqItem->z_Req.z_SetDormancyReq);
        pz_ReqItem->z_Msg.q_MsgLen = sizeof(pz_ReqItem->z_Req.z_SetDormancyReq);
        break;

      case GPSONE_BIT_IOCTL_UNFORCE_DORMANCY:
        pz_ReqItem->z_Msg.q_MsgId = QMI_BIT_SET_DORMANCY_REQ_V01;
        pz_ReqItem->z_Req.z_SetDormancyReq.transaction_id = q_TransactionId;
        pz_ReqItem->z_Req.z_GetLocalHostInfoReq.session_handle = 
                                                pz_ConnItem->t_SessionHandle;
        pz_ReqItem->z_Req.z_SetDormancyReq.dormancy_state = TRUE; /* Active */

        pz_ReqItem->z_Msg.p_Msg = (void*)&(pz_ReqItem->z_Req.z_SetDormancyReq);
        pz_ReqItem->z_Msg.q_MsgLen = sizeof(pz_ReqItem->z_Req.z_SetDormancyReq);
        break;

      case GPSONE_BIT_IOCTL_GET_LOCAL_IP_ADDR:
        pz_ReqItem->z_Msg.q_MsgId = QMI_BIT_GET_LOCAL_HOST_INFO_REQ_V01;
        pz_ReqItem->z_Req.z_GetLocalHostInfoReq.transaction_id = q_TransactionId;
        pz_ReqItem->z_Req.z_GetLocalHostInfoReq.session_handle = 
                                                pz_ConnItem->t_SessionHandle;

        pz_ReqItem->z_Msg.p_Msg = (void*)&(pz_ReqItem->z_Req.z_GetLocalHostInfoReq);
        pz_ReqItem->z_Msg.q_MsgLen = sizeof(pz_ReqItem->z_Req.z_GetLocalHostInfoReq);
        break;

      default:
        e_RetVal = GPSONE_BIT_STATUS_INVALID_PARAM;
        q_CheckPt = 4; 
        break;
        
    }

    if ( 0 != q_CheckPt )
    {
      /* Information should be filled in already for the return value */
      break;
    }
    

    if ( FALSE == qmiBitSendReq(
                              pz_CltInfo,
                              &(pz_ReqItem->z_Msg ) ) )
    {
      e_RetVal = GPSONE_BIT_STATUS_IPC_FAILURE;
      q_CheckPt = 5;
      break;
    }

    
    /* We have to wait as the message is sent across and we need the response 
       message from the server to let us know of the outcome */
    e_RetVal = GPSONE_BIT_STATUS_WAIT;
  } while(0);

  if ( NULL != pz_ReqItem )
  {
    /* Free the Handle to the ReqItem, as it is not needed now. We sent the 
       message */
    qmiBitDeallocateRequestItem(&pz_ReqItem);
  }


  if ( 0 != q_CheckPt )
  {
    QMI_BIT_MSG_ERROR("SendCb:%d, q_CheckPt:%d",e_RetVal, q_CheckPt, 0 );
  }
  return e_RetVal;
}



/*============================================================================
 *                       M E S S A G E    H A N D L E R S 
 * =========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_OPEN_RESP_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleOpenResp 
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_resp_msg_v01* pz_IncomingMsg = (bit_resp_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /* Ensure that the transaction ID that we got is what we expected */
    if ( QMI_BIT_TRANSACTION_ID_OPEN != pz_IncomingMsg->transaction_id )
    {
      e_QmiErr = QMI_ERR_INVALID_ARG_V01;
      break;
    }

    e_QmiErr = qmiBitGenericResponseHandler( 
                   pz_CltInfo,
                   GPSONE_BIT_EVENT_OPEN_RESULT,
                   &(pz_IncomingMsg->resp),
                   pz_CltInfo->z_BitTransportHandle,
                   GPSONE_BIT_INVALID_SESSION_HANDLE, /* No Need for Session Handle */
                   0 ) ;/* No Transaction ID allocated from BIT-API */
  } while(0);
  
  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_OPEN_STATUS_IND_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleOpenStatusInd
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_open_status_ind_msg_v01* pz_IncomingMsg = 
                            (bit_open_status_ind_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  gpsone_bit_event_payload_type z_GpsEventPayload; 
  gpsone_bit_status_e_type e_Status;
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /* Ensure that the transaction ID that we got is what we expected */
    if ( QMI_BIT_TRANSACTION_ID_OPEN != pz_IncomingMsg->transaction_id )
    {
      e_QmiErr = QMI_ERR_INVALID_ARG_V01;
      break;
    }

    memset( &z_GpsEventPayload, 0, sizeof(z_GpsEventPayload ) );
    z_GpsEventPayload.event = GPSONE_BIT_EVENT_OPEN_RESULT;
    z_GpsEventPayload.result = 
            ( QMI_RESULT_SUCCESS_V01 == pz_IncomingMsg->status.result ) ?
            GPSONE_BIT_STATUS_SUCCESS :
            ((gpsone_bit_status_e_type)QMI_BIT_GET_GPS_BIT_VAL( 
                                      pz_qmiBitStatusToQmiErrorMap, 
                                      pz_IncomingMsg->status.error ) );

    /*** No Payload for this event to BIT-API ***/

    e_Status = gpsone_bit_notify( pz_CltInfo->z_BitTransportHandle,
                       GPSONE_BIT_INVALID_SESSION_HANDLE, /* No Need for Session Handle */
                       0, /* No need for transaction Id in this call */
                       &z_GpsEventPayload); 
    
    e_QmiErr = 
      (qmi_error_type_v01)QMI_BIT_GET_QMI_BIT_VAL(pz_qmiBitStatusToQmiErrorMap,e_Status);
  } while(0);

  qmiBitSetResponse(&(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_CLOSE_RESP_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleCloseResp 
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_resp_msg_v01* pz_IncomingMsg = (bit_resp_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do 
  {
    
    /* Ensure that the transaction ID that we got is what we expected */
    if ( QMI_BIT_TRANSACTION_ID_CLOSE != pz_IncomingMsg->transaction_id )
    {
      e_QmiErr = QMI_ERR_INVALID_ARG_V01;
      break;
    }

    e_QmiErr = qmiBitGenericResponseHandler( 
                   pz_CltInfo,
                   GPSONE_BIT_EVENT_CLOSE_RESULT,
                   &(pz_IncomingMsg->resp),
                   pz_CltInfo->z_BitTransportHandle,
                   GPSONE_BIT_INVALID_SESSION_HANDLE, /* No Need for Session Handle */
                   0 ) ;/* No Transaction ID allocated from BIT-API */
  } while(0);
  qmiBitSetResponse(&(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_CLOSE_STATUS_IND_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleCloseStatusInd
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_close_status_ind_msg_v01* pz_IncomingMsg = 
                            (bit_close_status_ind_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  gpsone_bit_event_payload_type z_GpsEventPayload; 
  gpsone_bit_status_e_type e_Status;
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /* Ensure that the transaction ID that we got is what we expected */
    if ( QMI_BIT_TRANSACTION_ID_CLOSE != pz_IncomingMsg->transaction_id )
    {
      e_QmiErr = QMI_ERR_INVALID_ARG_V01;
      break;
    }

    memset( &z_GpsEventPayload, 0, sizeof(z_GpsEventPayload ) );
    z_GpsEventPayload.event = GPSONE_BIT_EVENT_CLOSE_RESULT;
    z_GpsEventPayload.result = 
            ( QMI_RESULT_SUCCESS_V01 == pz_IncomingMsg->status.result ) ?
            GPSONE_BIT_STATUS_SUCCESS :
            ((gpsone_bit_status_e_type)QMI_BIT_GET_GPS_BIT_VAL( 
                                      pz_qmiBitStatusToQmiErrorMap, 
                                      pz_IncomingMsg->status.error ) );
    
    /*** No Payload for this event to BIT-API ***/

    e_Status = gpsone_bit_notify( pz_CltInfo->z_BitTransportHandle,
                       GPSONE_BIT_INVALID_SESSION_HANDLE, /* No Need for Session Handle */
                       0, /* No need for transaction Id in this call */
                       &z_GpsEventPayload); 
    
    e_QmiErr = 
        (qmi_error_type_v01)QMI_BIT_GET_QMI_BIT_VAL(pz_qmiBitStatusToQmiErrorMap,e_Status);
  } while(0);

  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_CONNECT_RESP_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleConnectResp 
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_resp_msg_v01* pz_IncomingMsg = (bit_resp_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do 
  {
    if ( FALSE == qmiBitIsPendingTransactionIDPresent(
                              pz_IncomingMsg->transaction_id ) )
    {
      e_QmiErr = QMI_ERR_INVALID_ARG_V01;
      break;
    }

    /* If there was some error, The generic response handler
       will send error to gpsone-bit-notify, so we can remove it. 

       In case there is an error, it means that the client will send the 
       final result as part of QMI_BIT_CONNECT_STATUS_IND 

       So if there is an error remove it from the Pending Transaction ID
       List */
    if ( QMI_RESULT_SUCCESS_V01 != pz_IncomingMsg->resp.result )
    {
      qmiBitRemovePendingTransactionID(pz_IncomingMsg->transaction_id );
    }
    
    e_QmiErr = qmiBitGenericResponseHandler( 
                   pz_CltInfo,
                   GPSONE_BIT_EVENT_CONNECT_RESULT,
                   &(pz_IncomingMsg->resp),
                   pz_CltInfo->z_BitTransportHandle,
                   GPSONE_BIT_INVALID_SESSION_HANDLE, /* No Need for Session Handle */
                   pz_IncomingMsg->transaction_id ) ;/* Transaction ID allocated by 
                                                        BIT-API */

  } while(0);                                                        


  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_CONNECT_STATUS_IND_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleConnectStatusInd
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_connect_status_ind_msg_v01* pz_IncomingMsg = 
                            (bit_connect_status_ind_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  gpsone_bit_event_payload_type z_GpsEventPayload; 
  gpsone_bit_status_e_type e_Status;
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
  gpsone_bit_session_handle_type z_BitApiSessionHandle; 
  qmiBitConnectionInfoStructT *pz_ConnItem = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {

  
    if ( FALSE == qmiBitIsPendingTransactionIDPresent(
                              pz_IncomingMsg->transaction_id ) )
    {
      e_QmiErr = QMI_ERR_INVALID_ARG_V01;
      break;
    }

    /* This is the final result so we have to remove it in all cases */
    qmiBitRemovePendingTransactionID(pz_IncomingMsg->transaction_id);
    
    memset( &z_GpsEventPayload, 0, sizeof(z_GpsEventPayload ) );
    z_GpsEventPayload.event = GPSONE_BIT_EVENT_CONNECT_RESULT;
    z_GpsEventPayload.result = 
            ( QMI_RESULT_SUCCESS_V01 == pz_IncomingMsg->status.result ) ?
            GPSONE_BIT_STATUS_SUCCESS :
            ((gpsone_bit_status_e_type)QMI_BIT_GET_GPS_BIT_VAL( 
                                      pz_qmiBitStatusToQmiErrorMap, 
                                      pz_IncomingMsg->status.error ) );
    /*** No Payload for this event to BIT-API ***/

    /* Ensure that we have a valid session handle if success is reported  */
    if ( ( QMI_RESULT_SUCCESS_V01 == pz_IncomingMsg->status.result) &&
         ( TRUE != pz_IncomingMsg->session_handle_valid )  )
    {
      e_QmiErr = QMI_ERR_INVALID_ARG_V01;
      break;
    }

    /* If result is failure then no need to alocate Connection Info Item */
    if ( QMI_RESULT_FAILURE_V01 == pz_IncomingMsg->status.result )
    {
      z_BitApiSessionHandle = GPSONE_BIT_INVALID_SESSION_HANDLE;
    }
    else
    {
      z_BitApiSessionHandle = qmiBitAllocateConnectionInfoItem(  
                                              pz_CltInfo,
                                              pz_IncomingMsg->session_handle);

      if ( GPSONE_BIT_INVALID_SESSION_HANDLE == z_BitApiSessionHandle )
      {
        /* Could not allocate Session Handle, Internal Error */
        e_QmiErr = QMI_ERR_INTERNAL_V01;
        break;
      }
    }

    e_Status = gpsone_bit_notify( pz_CltInfo->z_BitTransportHandle,
                       z_BitApiSessionHandle, 
                       pz_IncomingMsg->transaction_id, 
                       &z_GpsEventPayload); 
    
    e_QmiErr = 
        (qmi_error_type_v01)QMI_BIT_GET_QMI_BIT_VAL(pz_qmiBitStatusToQmiErrorMap,e_Status);

    if ( QMI_RESULT_FAILURE_V01 == pz_IncomingMsg->status.result )
    {
      /* Since the BIT-Provider sent a failure, we did not allocate,
         after notifying the Bit-Api we need to break */
         
      /* Qmi Err is already set, no need to set again */      
      break;
    }

    /* Note: We reach here only if the BIT-Provider sends a SUCCESS in 
             the status indication */
    if ( GPSONE_BIT_STATUS_SUCCESS != e_Status )
    { 
      /* Deallocate the Connection Info Item As we indicate an error */
      (void) qmiBitDeallocateConnectionInfoItem( pz_CltInfo,
                                          pz_IncomingMsg->session_handle);
      /* Qmi Err is already set, no need to set again */      
      break;
    }

    /* Send a message to client that we are ready to accept messages */
    pz_ConnItem = (qmiBitConnectionInfoStructT*)q_linear_search(
                      &(pz_CltInfo->z_ConnectionQ),
                      qmiBitCompareConnectionItemQMISessionHandle,
                      (void*) &(pz_IncomingMsg->session_handle) );
    if ( NULL == pz_ConnItem )
    {
      /* Should never reach here, as we just allocated an item for this 
         Connection */
      ASSERT(0);
    }

    /* Connection State is initialized */
    pz_ConnItem->e_ConnectionState = QMI_BIT_CONNECTION_STATE_INITIALIZED;
    
    QMI_BIT_NOTIFY_CLIENT_READY_TO_RECEIVE(&pz_ConnItem->z_Wmk,pz_ConnItem);
    
  }while(0);

  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_DISCONNECT_RESP_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleDisconnectResp 
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_session_resp_msg_v01* pz_IncomingMsg = (bit_session_resp_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Note: Not notifying Gps-Bit as it was already done before */

  if ( QMI_RESULT_SUCCESS_V01 != pz_IncomingMsg->resp.result )
  {
    /* In case an error was encountered while disconnect, release 
       the connectionInfo as it will not be used again 
       Ignore, return value, as this is an effort to clean up and notification
       has already been given to the Bit-Api */ 
    (void) qmiBitDeallocateConnectionInfoItem(  
                                          pz_CltInfo,
                                          pz_IncomingMsg->session_handle);
  }

  /* All cases of handing are successful */
  e_QmiErr = QMI_ERR_NONE_V01;


  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_DISCONNECT_STATUS_IND_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleDisconnectStatusInd
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_disconnect_status_ind_msg_v01* pz_IncomingMsg = 
                            (bit_disconnect_status_ind_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
  gpsone_bit_session_handle_type z_BitApiSessionHandle; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {

    z_BitApiSessionHandle = qmiBitDeallocateConnectionInfoItem(  
                                            pz_CltInfo,
                                            pz_IncomingMsg->session_handle);
    if ( GPSONE_BIT_INVALID_SESSION_HANDLE == z_BitApiSessionHandle )
    {
      /* Could not find the Session Handle, probably invalid argument */
      e_QmiErr = QMI_ERR_INVALID_ARG_V01;
    }
    else
    {
      e_QmiErr = QMI_ERR_NONE_V01;

    }
  } while(0);

  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_SEND_RESP_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleSendResp 
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_session_resp_msg_v01* pz_IncomingMsg = (bit_session_resp_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  e_QmiErr = qmiBitGenericResponseHandler( 
                 pz_CltInfo,
                 GPSONE_BIT_EVENT_SEND_RESULT,
                 &(pz_IncomingMsg->resp),
                 pz_CltInfo->z_BitTransportHandle,
                 pz_IncomingMsg->session_handle, /* No Need for Session Handle */
                 pz_IncomingMsg->transaction_id ) ;/* Transaction ID allocated by 
                                                      BIT-API */

  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_SEND_STATUS_IND_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleSendStatusInd
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_send_status_ind_msg_v01* pz_IncomingMsg = 
                            (bit_send_status_ind_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  gpsone_bit_event_payload_type z_GpsEventPayload; 
  gpsone_bit_status_e_type e_Status;
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
  qmiBitConnectionInfoStructT *pz_ConnectionInfo = NULL; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    memset( &z_GpsEventPayload, 0, sizeof(z_GpsEventPayload ) );
    z_GpsEventPayload.event = GPSONE_BIT_EVENT_SEND_RESULT;
    z_GpsEventPayload.result = 
            ( QMI_RESULT_SUCCESS_V01 == pz_IncomingMsg->status.result ) ?
            GPSONE_BIT_STATUS_SUCCESS :
            ((gpsone_bit_status_e_type)QMI_BIT_GET_GPS_BIT_VAL( 
                                      pz_qmiBitStatusToQmiErrorMap, 
                                      pz_IncomingMsg->status.error ) );

    if ( TRUE == pz_IncomingMsg->bytes_sent_valid )
    {
      z_GpsEventPayload.arg.bytes_sent = pz_IncomingMsg->bytes_sent;
    }

    pz_ConnectionInfo = 
                        (qmiBitConnectionInfoStructT*)q_linear_search(
                                    &(pz_CltInfo->z_ConnectionQ),
                                    qmiBitCompareConnectionItemQMISessionHandle,
                                    (void*) &(pz_IncomingMsg->session_handle));
    if ( NULL == pz_ConnectionInfo )
    {
      /* Could not find the Connection Info for this session */
      e_QmiErr = QMI_ERR_GENERAL_V01;
      break;
    }
    e_Status = gpsone_bit_notify( pz_CltInfo->z_BitTransportHandle,
                       (gpsone_bit_session_handle_type)pz_ConnectionInfo->w_ConnectionId, 
                       pz_IncomingMsg->transaction_id, 
                       &z_GpsEventPayload); 
    e_QmiErr = 
        (qmi_error_type_v01)QMI_BIT_GET_QMI_BIT_VAL(pz_qmiBitStatusToQmiErrorMap,e_Status);
  } while(0);

  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_READY_TO_RECEIVE_RESP_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleReadyToReceiveResp 
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
  gpsone_bit_event_payload_type z_GpsEventPayload;   
  bit_session_resp_msg_v01 * pz_IncomingMsg = 
                                  (bit_session_resp_msg_v01*)pz_MsgInfo->p_Msg;
  qmiBitConnectionInfoStructT* pz_ConnectionInfo = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /* Ensure that the transaction ID that we got is what we expected */
    if ( pz_IncomingMsg == NULL || QMI_BIT_TRANSACTION_ID_RTR != pz_IncomingMsg->transaction_id )
    {
      e_QmiErr = QMI_ERR_INVALID_ARG_V01;
      break;
    }

    
    pz_ConnectionInfo = 
                        (qmiBitConnectionInfoStructT*)q_linear_search(
                                    &(pz_CltInfo->z_ConnectionQ),
                                    qmiBitCompareConnectionItemQMISessionHandle,
                                    (void*) &(pz_IncomingMsg->session_handle));

    if ( NULL == pz_ConnectionInfo )
    {
      /* Could not find the Connection Info for this session */
      e_QmiErr = QMI_ERR_INVALID_ARG_V01;
      break;
    }

    /* If the result was not a success, then notify abort as there was a problem,
       And its better to disconnect */       
    if ( QMI_RESULT_SUCCESS_V01 != pz_IncomingMsg->resp.result )
    {
      memset(&z_GpsEventPayload, 0, sizeof(z_GpsEventPayload));
      z_GpsEventPayload.event = GPSONE_BIT_EVENT_ABORT_CONNNECTION;
      z_GpsEventPayload.result = GPSONE_BIT_STATUS_FAIL;

      /* Notify Abort to GPSBit, it will call disconnect and hence
         cleaning up the connection. */
      gpsone_bit_notify( pz_CltInfo->z_BitTransportHandle,
                         (gpsone_bit_session_handle_type)pz_ConnectionInfo->w_ConnectionId,
                         pz_IncomingMsg->transaction_id, 
                         &z_GpsEventPayload );
    }
    
    /* Nothing to do, just send back the Ack */
    e_QmiErr = QMI_ERR_NONE_V01;

  } while(0);


  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_DATA_RECEIVED_IND_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleDataReceivedInd
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_data_received_ind_msg_v01 *pz_IncomingMsg =
        (bit_data_received_ind_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
  qmiBitConnectionInfoStructT *pz_ConnItem = NULL;
  qmiBitReqInfoStructT *pz_ReqItem = NULL;
  boolean               b_SendStatusReq = FALSE;
  dsm_item_type *pz_DsmItem = NULL;
  uint32 q_WmkCurrCnt  = 0 ;
  uint32 q_MaxBytesCanRecv = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  do
  { 
    
    pz_ConnItem = 
                  (qmiBitConnectionInfoStructT*)q_linear_search(
                              &(pz_CltInfo->z_ConnectionQ),
                              qmiBitCompareConnectionItemQMISessionHandle,
                              (void*) &(pz_IncomingMsg->session_handle));

    if ( NULL == pz_ConnItem )
    {
      /* Could not find the Connection Info for this session */
      e_QmiErr = QMI_ERR_INVALID_ARG_V01;
      break;
    }

    /* If we received a 0-payload message, not expected */
    if ( 0 == pz_IncomingMsg->payload_len )
    {
      e_QmiErr = QMI_ERR_MESSAGE_NOT_SENT_V01;  
      break;
    }

    pz_ReqItem = qmiBitAllocateRequestItem(pz_CltInfo);
    if ( NULL == pz_ReqItem )
    {
      e_QmiErr = QMI_ERR_NO_MEMORY_V01;
      break;
    }

    /* Fill in the values in the Request that are available now */
    pz_ReqItem->z_Msg.q_MsgId = QMI_BIT_DATA_RECEIVED_STATUS_REQ_V01;
    pz_ReqItem->z_Msg.q_MsgLen = 
                        sizeof(pz_ReqItem->z_Req.z_DataReceivedStatusReq );
    pz_ReqItem->z_Msg.p_Msg = 
                        (void*)&(pz_ReqItem->z_Req.z_DataReceivedStatusReq);

    pz_ReqItem->z_Req.z_DataReceivedStatusReq.transaction_id = 
                                          QMI_BIT_TRANSACTION_ID_DATA_RECEIVED;
    pz_ReqItem->z_Req.z_DataReceivedStatusReq.seq_num = 
                                               pz_IncomingMsg->seq_num;
    pz_ReqItem->z_Req.z_DataReceivedStatusReq.session_handle = 
                                               pz_IncomingMsg->session_handle;

      
    /* If this is not the sequence number that was expected */
    if ( pz_IncomingMsg->seq_num != pz_ConnItem->q_ExpectedSeqNum )
    {
      qmiBitSetResponse( &(pz_ReqItem->z_Req.z_DataReceivedStatusReq.resp), 
                            QMI_ERR_SEGMENT_ORDER_V01 );
      b_SendStatusReq = TRUE;
      e_QmiErr = QMI_ERR_NONE_V01;  /* No error in handling this Indication from 
                                       BIT Service */
      break;
    }

    if ( pz_IncomingMsg->payload_len != dsm_pushdown_packed( &pz_DsmItem,
                                               (void*)pz_IncomingMsg->payload,
                                               pz_IncomingMsg->payload_len,
                                               DSM_DS_SMALL_ITEM_POOL ) )
    {
      qmiBitSetResponse( &(pz_ReqItem->z_Req.z_DataReceivedStatusReq.resp), 
                            QMI_ERR_NO_MEMORY_V01 );
      b_SendStatusReq = TRUE;
      e_QmiErr = QMI_ERR_NONE_V01;  /* No error in handling this Indication from 
                                       BIT Service */
      break;
    }

    /* Now push the data into the watermark */
    dsm_enqueue( &(pz_ConnItem->z_Wmk),
                 &pz_DsmItem );
    
    /* Now increment the sequence number as we accepted the current packet */
    pz_ConnItem->q_ExpectedSeqNum++;
    
    qmiBitSetResponse( &(pz_ReqItem->z_Req.z_DataReceivedStatusReq.resp), 
                          QMI_ERR_NONE_V01 );

    
    q_WmkCurrCnt = qmiBitGetCurrBuffCnt(pz_ConnItem);
    q_MaxBytesCanRecv = MIN ( BIT_CONST_PAYLOAD_LEN_MAX_V01,
                              (QMI_BIT_HI_WMK_THRESHOLD - q_WmkCurrCnt) );
    
    QMI_BIT_SET_OPTIONAL_VALUE( 
               pz_ReqItem->z_Req.z_DataReceivedStatusReq.max_recv_payload_size,
               q_MaxBytesCanRecv );

    b_SendStatusReq = TRUE;
    
    e_QmiErr = QMI_ERR_NONE_V01;
  }while(0);

  if ( TRUE == b_SendStatusReq )
  {
    /* Note: Might cause the Status_Req to go out before the Ack, if this 
       causes trouble, will have to send IPC to service task ( PDCOMMS ) 
       and then handle it */
    if ( FALSE == qmiBitSendReq(
                                  pz_CltInfo,
                                  &(pz_ReqItem->z_Msg ) ) )
    {
      e_QmiErr = QMI_ERR_INTERNAL_V01;
    }
  }

  if ( NULL != pz_ReqItem )
  {
    /* Free the item as it is not needed */
    qmiBitDeallocateRequestItem(&pz_ReqItem);
  }

  /* The actual response goes out in the indication */
  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr );
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_DATA_RECEIVED_STATUS_RESP_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleDataReceivedStatusResp 
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
  bit_session_resp_msg_v01 * pz_IncomingMsg = 
                                  (bit_session_resp_msg_v01*)pz_MsgInfo->p_Msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Ensure that the transaction ID that we got is what we expected */
  if ( QMI_BIT_TRANSACTION_ID_DATA_RECEIVED != pz_IncomingMsg->transaction_id )
  {
    e_QmiErr = QMI_ERR_INVALID_ARG_V01;
  }
  else
  {
    /* Nothing to do, just send back the Ack */
    e_QmiErr = QMI_ERR_NONE_V01;
  }

  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}


/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_SET_DORMANCY_RESP_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleSetDormancyResp 
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_session_resp_msg_v01* pz_IncomingMsg = (bit_session_resp_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  e_QmiErr = qmiBitGenericResponseHandler( 
                 pz_CltInfo,
                 GPSONE_BIT_EVENT_IOCTL_RESULT,
                 &(pz_IncomingMsg->resp),
                 pz_CltInfo->z_BitTransportHandle,
                 pz_IncomingMsg->session_handle, /* No Need for Session Handle */
                 pz_IncomingMsg->transaction_id ) ;

  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_SET_DORMANCY_STATUS_IND_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleSetDormancyStatusInd
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_set_dormancy_status_ind_msg_v01* pz_IncomingMsg = 
                            (bit_set_dormancy_status_ind_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  gpsone_bit_event_payload_type z_GpsEventPayload; 
  gpsone_bit_status_e_type e_Status;
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
  qmiBitConnectionInfoStructT *pz_ConnectionInfo = NULL; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {

    memset( &z_GpsEventPayload, 0, sizeof(z_GpsEventPayload ) );
    z_GpsEventPayload.event = GPSONE_BIT_EVENT_IOCTL_RESULT;
    z_GpsEventPayload.result = 
            ( QMI_RESULT_SUCCESS_V01 == pz_IncomingMsg->status.result ) ?
            GPSONE_BIT_STATUS_SUCCESS :
            ((gpsone_bit_status_e_type)QMI_BIT_GET_GPS_BIT_VAL( 
                                      pz_qmiBitStatusToQmiErrorMap, 
                                      pz_IncomingMsg->status.error ) );
    
    /*** No Payload for this event to BIT-API ***/

    pz_ConnectionInfo = 
                        (qmiBitConnectionInfoStructT*)q_linear_search(
                                    &(pz_CltInfo->z_ConnectionQ),
                                    qmiBitCompareConnectionItemQMISessionHandle,
                                    (void*) &(pz_IncomingMsg->session_handle));

    if ( NULL == pz_ConnectionInfo )
    {
      /* Could not find the Connection Info for this session */
      e_QmiErr = QMI_ERR_GENERAL_V01;
      break;
    }

    e_Status = gpsone_bit_notify( pz_CltInfo->z_BitTransportHandle,
                       (gpsone_bit_session_handle_type)pz_ConnectionInfo->w_ConnectionId, 
                       pz_IncomingMsg->transaction_id, 
                       &z_GpsEventPayload); 
    e_QmiErr = 
        (qmi_error_type_v01)QMI_BIT_GET_QMI_BIT_VAL(pz_qmiBitStatusToQmiErrorMap,e_Status);
  } while(0);

  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_GET_LOCAL_HOST_INFO_RESP_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleGetLocalHostInfoResp 
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_session_resp_msg_v01* pz_IncomingMsg = (bit_session_resp_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  e_QmiErr = qmiBitGenericResponseHandler( 
                 pz_CltInfo,
                 GPSONE_BIT_EVENT_IOCTL_RESULT,
                 &(pz_IncomingMsg->resp),
                 pz_CltInfo->z_BitTransportHandle,
                 pz_IncomingMsg->session_handle, 
                 pz_IncomingMsg->transaction_id ) ;

  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}

/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_GET_LOCAL_HOST_INFO_STATUS_IND_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleGetLocalHostInfoStatusInd
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_get_local_host_info_status_ind_msg_v01* pz_IncomingMsg = 
                            (bit_get_local_host_info_status_ind_msg_v01*)pz_MsgInfo->p_Msg;
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  gpsone_bit_event_payload_type z_GpsEventPayload; 
  gpsone_bit_status_e_type e_Status;
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
  qmiBitConnectionInfoStructT *pz_ConnectionInfo = NULL; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {

    memset( &z_GpsEventPayload, 0, sizeof(z_GpsEventPayload ) );
    z_GpsEventPayload.event = GPSONE_BIT_EVENT_IOCTL_RESULT;
    z_GpsEventPayload.result = 
            ( QMI_RESULT_SUCCESS_V01 == pz_IncomingMsg->status.result ) ?
            GPSONE_BIT_STATUS_SUCCESS :
            ((gpsone_bit_status_e_type)QMI_BIT_GET_GPS_BIT_VAL( 
                                      pz_qmiBitStatusToQmiErrorMap, 
                                      pz_IncomingMsg->status.error ) );

    if ( pz_IncomingMsg->local_host_info_valid )
    {
      if ( pz_IncomingMsg->local_host_info.validity_mask & BIT_MASK_IPV4_ADDR_AND_PORT_V01 )
      {
        z_GpsEventPayload.arg.ipaddr.type = GPSONE_BIT_IP_V4;
        z_GpsEventPayload.arg.ipaddr.addr.v4_addr = 
                                    pz_IncomingMsg->local_host_info.ipv4_addr;
      }
      else if ( pz_IncomingMsg->local_host_info.validity_mask & BIT_MASK_IPV6_ADDR_AND_PORT_V01 )
      {
        z_GpsEventPayload.arg.ipaddr.type = GPSONE_BIT_IP_V6;
        (void) gps_Memcpy( sizeof(z_GpsEventPayload.arg.ipaddr.addr.v6_addr),
                      (void*)z_GpsEventPayload.arg.ipaddr.addr.v6_addr,
                      sizeof(pz_IncomingMsg->local_host_info.ipv6_addr),
                      (void*)pz_IncomingMsg->local_host_info.ipv6_addr );
      }
    }
    
    pz_ConnectionInfo = 
                        (qmiBitConnectionInfoStructT*)q_linear_search(
                                    &(pz_CltInfo->z_ConnectionQ),
                                    qmiBitCompareConnectionItemQMISessionHandle,
                                    (void*) &(pz_IncomingMsg->session_handle));

    if ( NULL == pz_ConnectionInfo )
    {
      /* Could not find the Connection Info for this session */
      e_QmiErr = QMI_ERR_GENERAL_V01;
      break;
    }

    e_Status = gpsone_bit_notify( pz_CltInfo->z_BitTransportHandle,
                       (gpsone_bit_session_handle_type)pz_ConnectionInfo->w_ConnectionId, 
                       pz_IncomingMsg->transaction_id, 
                       &z_GpsEventPayload); 
    e_QmiErr = 
      (qmi_error_type_v01)QMI_BIT_GET_QMI_BIT_VAL(pz_qmiBitStatusToQmiErrorMap,e_Status);
  } while(0);

  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);
}


/* -----------------------------------------------------------------------*//**
@brief
  Message handler to handle message QMI_BIT_SERVICE_READY_IND_V01

@param[in|out]  pz_CltInfo            Client for whom this message is intended.
@param[in]      p_MsgInfo             Information regarding to the message
@param[out]     pz_AckInfo            The Acknowledgement that needs to 
                                      be sent to the BIT-Service
*//* ------------------------------------------------------------------------*/
static void qmiBitHandleServiceReadyInd 
(  
  qmiBitCltInfoStructT                  *pz_CltInfo, 
  const qmiBitMsgInfoStructT            *pz_MsgInfo, 
  qmiBitAckInfoStructT                  *pz_AckInfo
)
{
  bit_ack_msg_v01 * pz_Ack = &(pz_AckInfo->z_GenAck);
  gpsone_bit_status_e_type  e_BitErrStatus;
  qmi_error_type_v01 e_QmiErr = QMI_ERR_INTERNAL_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /* This message should be received only once after client Init, if we 
       receive it more than one time, we just send a Success, no need to 
       flag an error */
    if ( TRUE == pz_CltInfo->b_RegisteredWithBitApi )
    {
      e_QmiErr = QMI_ERR_NONE_V01;
      break;
    }

    /* The String has in it embedded the minor version and address to the 
       Client Information */
    snprintf( pz_CltInfo->z_BitRegParams.bit_name, 
             sizeof(pz_CltInfo->z_BitRegParams.bit_name),
             "BIT[%d]%p",
             BIT_V01_IDL_MINOR_VERS,
             pz_CltInfo );
    pz_CltInfo->z_BitRegParams.bit_type = GPSONE_BIT_TYPE_IP;

     
    e_BitErrStatus = gpsone_bit_register ( 
                             &pz_CltInfo->z_BitRegParams,
                             &pz_CltInfo->z_BitTransportHandle,
                             qmiBitOpenCb,
                             qmiBitCloseCb,
                             qmiBitConnectCb,
                             qmiBitDisconnectCb,
                             qmiBitSendCb,
                             qmiBitRecvCb,
                             qmiBitIoctlCb );
    e_QmiErr = 
      (qmi_error_type_v01)QMI_BIT_GET_QMI_BIT_VAL(pz_qmiBitStatusToQmiErrorMap,e_BitErrStatus);

    /* If successfully registed with BIT-API set the flag, to 
       avoid re-registrations */
    if ( QMI_ERR_NONE_V01 == e_QmiErr )
    {
      pz_CltInfo->b_RegisteredWithBitApi = TRUE;
    }
    else
    {
      QMI_BIT_MSG_LOW ( "Service Ready Indication: BIT-API Err:%d, QmiErr:%d",
                         e_BitErrStatus,e_QmiErr,0);
    }
  } while(0);

  qmiBitSetResponse( &(pz_Ack->resp), e_QmiErr);
  pz_AckInfo->q_MsgId = pz_MsgInfo->q_MsgId;  /* Same Message ID as received
                           in the Response */
  pz_AckInfo->q_MsgLen = sizeof(*pz_Ack);

}



/*============================================================================
 *                       Q C S I    C A L L B A C K S 
 * =========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Callback function provided to QCSI, which notify the shim layer of a 
  new client connecting. 
  
@param[in]   pz_ClientHandle   Handle of client that is connecting allocated by
                               QCSI
@param[in]   pz_ServiceCookie  Handle of service, as provided by qmi-bit layer to
                               QCSI during registration of service. 
@param[out]  pz_ConHandle      Connection handle allocated by qmi-bit given back
                               given to QCSI for reference. 

@retval    QMI_CSI_CB_NO_ERR        Client Connect was handled with success 
@retval    QMI_CSI_CB_CONN_REFUSED  If client connect cannot be accepted
@retval    QMI_CSI_CB_INTERNAL_ERR  For all other errors

@warning
  A new client handle is accepted only if it is not present in the client 
  queue.
*//* ------------------------------------------------------------------------*/
static qmi_csi_cb_error qmiBitServiceConnectCb
(
  void                     *p_ClientHandle,
  void                     *pz_ServiceCookie,
  void                    **pz_ConHandle
)

{
  qmiBitCltInfoStructT *pz_CltInfo;
  uint32                q_CheckPt = 0;
  qmi_csi_cb_error      e_RetVal = QMI_CSI_CB_INTERNAL_ERR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check if the Client Handle is Non-Null, and is not a duplicate that 
     has already been used */
  QMI_BIT_MSG_LOW("Connectonnect Cb Received ConHandle:0x%x", pz_ConHandle, 0, 0 );

  do 
  {
    /* Invalid Client Handle */
    if ( NULL == p_ClientHandle ) 
    {
      q_CheckPt = 1;
      e_RetVal = QMI_CSI_CB_INTERNAL_ERR;
      break;
    }

    /* Invalid Service Cookie */
    if ( pz_ServiceCookie != &z_QmiBit)
    {
      q_CheckPt = 2;
      e_RetVal = QMI_CSI_CB_INTERNAL_ERR;
      break;
    }      

    /* Client handle already exists */
    if ( NULL != q_linear_search( &z_QmiBit.z_ClientQ,
                                     qmiBitCompareQmiClientHandle,
                                     p_ClientHandle ) )
    {
      q_CheckPt = 3;
      e_RetVal = QMI_CSI_CB_INTERNAL_ERR;
      break;
    }                                     

    /* Do not allow more than supported BIT-Services */
    if ( q_cnt( &(z_QmiBit.z_ClientQ) ) >= QMI_BIT_MAX_BIT_SERVICE )
    {
      q_CheckPt = 4;
      e_RetVal = QMI_CSI_CB_CONN_REFUSED;
      break;
    }

    pz_CltInfo = 
       (qmiBitCltInfoStructT*) os_MemAlloc( 
                           sizeof(qmiBitCltInfoStructT), 
                           OS_MEM_SCOPE_TASK );

    /* Heap Allocation Failed */
    if ( pz_CltInfo == NULL ) 
    {
      q_CheckPt = 5;
      e_RetVal = QMI_CSI_CB_NO_MEM;
      break;
    }
    
    /* Memset to "0" for first use, it might have garbage and cause 
       trouble */
    memset ( pz_CltInfo,
             0,
             sizeof(*pz_CltInfo) );
    
    /* Initialize the link, return value is a pointer to the link, 
       not needed */
    (void) q_link ( pz_CltInfo, &pz_CltInfo->z_Link );

    /* Store the client handle allocated by QCSI */
    pz_CltInfo->p_QmiClientHandle = p_ClientHandle;

    q_init(&(pz_CltInfo->z_ConnectionQ));
    
    /* Add it to the queue of clients */
    q_put( &z_QmiBit.z_ClientQ, 
           &pz_CltInfo->z_Link );
    
    /* Give the Client Info as a handle to the QCSI Framework */
    (*pz_ConHandle) = pz_CltInfo;

    /* The first valid Session ID */
    pz_CltInfo->w_NextConnectionId = 0;
    
    e_RetVal = QMI_CSI_CB_NO_ERR;
      
  }while(0);

  if ( QMI_CSI_CB_NO_ERR != e_RetVal )
  {
    QMI_BIT_MSG_ERROR("ConnectCb() Returning Error: q_CheckPt: %d, e_RetVal: %d",
                  q_CheckPt, e_RetVal, 0 );
  }
  
  return e_RetVal;
}



/* -----------------------------------------------------------------------*//**
@brief
  Callback function provided to QCSI, which notifies the shim layer of an  
  existing client disconnecting. At this point loc-close is called to 
  release the client handle with loc-api
  
@param[in]   pz_ConHandle     QMI-Handle of client that is Disconnecting. 
@param[in]   pz_ServiceCookie  Handle of service, as provided by SHIM layer to
                               QCSI during registration of service. 

*//* ------------------------------------------------------------------------*/
static void  qmiBitServiceDisconnectCb
(
  void                      *pz_ConHandle,
  void                      *pz_ServiceCookie
)
{
  qmiBitCltInfoStructT *pz_CltInfo = NULL;
  qmiBitConnectionInfoStructT *pz_ConnInfo = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QMI_BIT_MSG_LOW("Disconnect Cb Received ConHandle:0x%x", pz_ConHandle, 0, 0 );

  if ( NULL != pz_ConHandle )
  {
    /* Find the Connection Handle in the queue */
    pz_CltInfo = 
         (qmiBitCltInfoStructT*) q_linear_search( &z_QmiBit.z_ClientQ,
                                        qmiBitCompareCltInfoHandle,
                                        pz_ConHandle );
    if ( NULL != pz_CltInfo )
    {

      /* Remove from Queue */
      if ( TRUE != q_delete_ext( &z_QmiBit.z_ClientQ,
                            &pz_CltInfo->z_Link ) )
      {
        QMI_BIT_MSG_ERROR("Could not delete the client info from queue ",0, 0, 0);
      }

      /* Deallocate all the Connection Info's associated with 
         this BIT-Service */
      /* Move all ConnectionInfoItems to the Free Q after handling them 
         and destroy the ConnectionQ */
        pz_ConnInfo = 
          ( qmiBitConnectionInfoStructT* )q_check(&(pz_CltInfo->z_ConnectionQ));
        while ( NULL != pz_ConnInfo ) 
        {
          /* Handle Abort of the Connection Item */
          qmiBitAbortConnection(pz_ConnInfo);
          
          if ( GPSONE_BIT_INVALID_SESSION_HANDLE ==  qmiBitDeallocateConnectionInfoItem( pz_CltInfo,
                                                             pz_ConnInfo->t_SessionHandle) )
          {
            /* Assert earlier than causing a Dog-Time out, Since the item 
               exists in the ConnectionQ, there should be no reason why we 
               cannot remove it from the ConnectionQ. If we do not ASSERT(0) here
               it would go in an infinite loop causing a Dog-Crash. */
            ASSERT(0);
          }
          /* Get the next Request Item.
             Note its q_Check() not q_get()*/
          pz_ConnInfo = 
            ( qmiBitConnectionInfoStructT* )q_check(&(pz_CltInfo->z_ConnectionQ));
        
        }
        q_destroy(&(pz_CltInfo->z_ConnectionQ));

        if( GPSONE_BIT_STATUS_SUCCESS != 
                  gpsone_bit_deregister(pz_CltInfo->z_BitTransportHandle) )
        {
          QMI_BIT_MSG_ERROR("Could not deregister Successfully",0,0,0);
        }

      /* Free the memory in case of error */
      os_MemFree( (void**) &pz_CltInfo );
    }
  }
}

/* -----------------------------------------------------------------------*//**
@brief
   This callback is invoked when the infrastructure receives an incoming 
   message. The infrastructure decodes the data and gives it to the services 

@param[in]  pz_ConHandle           Service handle used by the infrastructure
                                   to communicate to each individual service.
@param[in]  z_ReqHandle            Opaque handle provided by the infrastructure
                                   to specify a particular transaction and
                                   message.

@param[in]  i_MsgId                Message Id pertaining to a particular
                                   message. 
@param[in]  pz_Msg                 C struct with the decoded data.
@param[in]  i_MsgLen               Length of the c struct.                                   
@param[in]  pz_ServiceCookie         Service specific data.Service cookie is 
                                   registered with the infrastructure during 
                                   service registration(qmi_csi_register).                     


@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB.....       Look into the enumeration qmi_csi_error for
                                 the error values.
*//* ------------------------------------------------------------------------*/
static qmi_csi_cb_error qmiBitHandleMsgFromBITServiceCb
(
  void                    *pz_ConHandle,
  qmi_req_handle           z_ReqHandle,
  uint32                   q_MsgId,
  void                    *pz_Msg,
  uint32                   q_MsgLen,
  void                    *pz_ServiceCookie
  )
{
  qmi_csi_cb_error                e_RetVal = QMI_CSI_CB_INTERNAL_ERR;
  qmiBitCltInfoStructT*           pz_CltInfo = NULL;
  boolean                         b_FoundIndex = FALSE;
  uint32                          q_CheckPt = 0;
  uint32                          q_Cntr = 0;
  qmiBitMsgInfoStructT            z_MsgInfo;
  qmiBitAckInfoStructT            z_AckInfo; /*< NOTE: If this structure 
                                     becomes too large, allocate on heap 
                                     instead of stack */
  const uint32                    q_MsgHdlrTableSize = 
                                      GNSS_ARRAY_SIZEOF(pz_qmiBitMsgHandlerTable);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  do
  {
    QMI_BIT_MSG_LOW("Recvd MsgID:0x%x", q_MsgId, 0, 0 );

    /* NULL pz_ConHandle is handled */
    pz_CltInfo = q_linear_search(  &z_QmiBit.z_ClientQ,     
                                   qmiBitCompareCltInfoHandle,
                                   pz_ConHandle );
      
    if ( NULL == pz_CltInfo )
    {
      q_CheckPt = 1; 
      break;
    }

    memset ( &(z_MsgInfo), 0, sizeof(z_MsgInfo) );
    memset ( &(z_AckInfo), 0, sizeof(z_AckInfo) );

    /* Fill up the structure for ease */
    z_MsgInfo.q_MsgId = q_MsgId;
    z_MsgInfo.p_Msg = pz_Msg;
    z_MsgInfo.q_MsgLen = q_MsgLen;

    /* Find the entry in the Table */
    for ( q_Cntr = 0;
          q_Cntr < q_MsgHdlrTableSize;
          q_Cntr++ )
    {
      
      if ( z_MsgInfo.q_MsgId  == pz_qmiBitMsgHandlerTable[q_Cntr].q_MsgId )
      {
        b_FoundIndex = TRUE;
        /* Break from the for loop searching the message handler table */
        break; 
      }
    }

    /* If not found bail out. */
    if ( ( q_MsgHdlrTableSize <= q_Cntr ) ||
         ( FALSE ==  b_FoundIndex ) )
    {
      q_CheckPt = 2;
      break;
    }


    /* The Message handler shall fill up the z_AckInfo in all cases */
    pz_qmiBitMsgHandlerTable[q_Cntr].pz_MsgHdlrFunc (
                                                    pz_CltInfo,
                                                    &z_MsgInfo,
                                                    &z_AckInfo );

    
    e_RetVal = qmiBitSendAck(
                    pz_CltInfo,
                    z_ReqHandle,
                    &z_AckInfo );

    /* Check Point in case the Ack does not get sent we can deduce it 
       from this Check Pt */
    q_CheckPt = 3;

  } while(0);

  if ( QMI_CSI_CB_NO_ERR != e_RetVal )
  {
    QMI_BIT_MSG_ERROR( "Request:0x%x, CheckPt:%d, Error:%d", 
                   q_MsgId, q_CheckPt, e_RetVal );
  }

  return e_RetVal;
}



/*****************************************************************************
 *****************************************************************************
 *
 *                   Externalized Function Definitions
 *
 *****************************************************************************
 * ***************************************************************************/
 
/* -----------------------------------------------------------------------*//**
@brief
 This function needs to be called to initialize the QMI-Loc module. It
 initializes the internal data structures of the module. 

@retval  TRUE  - Successful initialization of the module
@retval  FALSE - Module initialization was not successful

@warning
  This function does not gaurd itself against being called multiple times. 
  Once called it can be called again only if "cleanup" is called. 
*//* ------------------------------------------------------------------------*/
boolean qmiBitInit 
( 
  void 
)
{
  boolean                       b_RetVal = FALSE;
  qmi_csi_os_params             z_OsParams;
  qmi_csi_error                 rc = QMI_CSI_INTERNAL_ERR;
  qmi_idl_service_object_type   z_QmiBitServiceObject = NULL;
  uint32_t                      q_SvcId;
  uint32                        q_CheckPt = 0;
  qmi_sap_error                 e_SapErr = QMI_SAP_INTERNAL_ERR;
  uint32                        q_Cntr = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the Service Context Information */
  memset ( &z_QmiBit, 
            0,
            sizeof(z_QmiBit ) );

  /* Initialize the queue which will hold all handles of clients and maintain 
     the mapping of QMI-Client-handles and the loc-api-client handles */
  (void) q_init(&z_QmiBit.z_ClientQ);

  /* Get the TCB for the task in which initialization of shim was called */
  z_OsParams.tcb = os_Self();
  z_OsParams.sig = QMI_BIT_SIGNAL;

  z_QmiBitServiceObject =  bit_get_service_object_v01();

  do
  {
    if ( QMI_IDL_LIB_NO_ERR != qmi_idl_get_service_id( z_QmiBitServiceObject,
                                       &q_SvcId ) )
    {
      q_CheckPt = 1;
      break;
    }
  
    /* Register the service with QCSI Framework */
    rc = mqcsi_register (   q_SvcId,
                            BIT_V01_IDL_MAJOR_VERS,
                            BIT_V01_IDL_MINOR_VERS,
                            z_QmiBitServiceObject, 
                            qmiBitServiceConnectCb, 
                            qmiBitServiceDisconnectCb, 
                            qmiBitHandleMsgFromBITServiceCb, 
                            (void*)&z_QmiBit, 
                            &z_OsParams, 
                            &(z_QmiBit.p_ServiceHandle));
        
    if(QMI_CSI_NO_ERR != rc )
    {
      q_CheckPt = 2;
      break;
    }
    /** Register the service with QSAP*/
    e_SapErr = qmi_sap_register(z_QmiBitServiceObject,
                              NULL,
                              &(z_QmiBit.z_QSAPUserHandle));
    
    if( QMI_SAP_NO_ERR != e_SapErr )
    {
      q_CheckPt = 3;
      /* Unregister with QCSI */
      mqcsi_unregister(z_QmiBit.p_ServiceHandle);
      break;
    }

    /* Initialize the Queues to manage the Requests */
    q_init(&z_QmiBit.z_RequestFreeQ);
    q_init(&z_QmiBit.z_RequestQ);

    /* Initialize the Queues to manage the Connections */
    q_init(&z_QmiBit.z_ConnectionFreeQ);

    for ( q_Cntr = 0; q_Cntr < QMI_BIT_MAX_PENDING_TRANSACTION_IDS; q_Cntr++ )
    {
      z_QmiBit.pz_PendingTransactions[q_Cntr].b_IsValid = FALSE;
    }
    
    b_RetVal = TRUE;
  }while(0);
  
  if ( FALSE == b_RetVal )
  {
    QMI_BIT_MSG_ERROR("Init Error, CheckPt:%d, rc:%d, e_SapErr:%d",
                   q_CheckPt,rc,e_SapErr);
  }

  return b_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
  This function is called to reset and disconnect the shim layer from the 
  QMI-Loc Service. After this function call, the loc-middleware task will need 
  to call locQmiShimInit() to be able to communicate over the QMI-framework. 

@warning
  This function SHALL BE called only and only in the task-context in which the 
  service was initialized. This function may not perform as expected if 
  called in any random task-context. 
*//* ------------------------------------------------------------------------*/
void qmiBitCleanup 
( 
  void 
)
{
  qmiBitCltInfoStructT *pz_CltInfo = NULL;
  qmi_csi_error                 rc = QMI_CSI_INTERNAL_ERR;
  qmiBitReqInfoStructT  *pz_ReqItem;
  qmiBitConnectionInfoStructT *pz_ConnItem;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*  Continue Stopping only if we have a valid service handle */
  if ( z_QmiBit.p_ServiceHandle != NULL )
  {
    /** Unregister with QSAP */
    if( z_QmiBit.z_QSAPUserHandle )
    {
      if(QMI_SAP_NO_ERR != qmi_sap_deregister(z_QmiBit.z_QSAPUserHandle))
      {
        QMI_BIT_MSG_ERROR("Could not deregister the service with QSAP",
                       0, 0, 0); 
      }
    }

    /* Unregister with framework, so no more messages arrive */
    rc = mqcsi_unregister (  z_QmiBit.p_ServiceHandle );
    if ( rc != QMI_CSI_NO_ERR )
    {
      QMI_BIT_MSG_ERROR( "Could not deregister the service successfully",
                      0, 0, 0); 
    }


     /* Reset to initial values */
    memset ( &z_QmiBit.p_ServiceHandle,
             0,
             sizeof(z_QmiBit.p_ServiceHandle ) );

    /* For each client call the disconnect callback. The same actions need to 
       be taken. Hence q_check() instead of q_get() */
    pz_CltInfo = 
      ( qmiBitCltInfoStructT* )q_check(&z_QmiBit.z_ClientQ);
    while ( NULL != pz_CltInfo ) 
    {

      /* Trigger the function that handles disconnect client callback, 
         as at this point the same functionality needs to be performed 
         NOTE: Memory is freed inside this call */
      qmiBitServiceDisconnectCb( (void*) pz_CltInfo,
                                    &z_QmiBit );

      /* Get the next connection handle. 
         NOTE: it is q_check() not q_get() */
      pz_CltInfo = 
        ( qmiBitCltInfoStructT* )q_check(&z_QmiBit.z_ClientQ);
    }

    if ( 0 != q_cnt(&z_QmiBit.z_ClientQ) )
    {
      QMI_BIT_MSG_ERROR( "Cleanup was not proper, still some clients dangling ",
                      0, 0, 0); 
    }

    /* Destroy the queue. It is expected to call Init() again before using this 
       module */
    q_destroy(&z_QmiBit.z_ClientQ);

    /* Move all outstanding requests to the Free Q after handling them 
       and destroy the ReqQ
       NOTE: There should be none, as these are freed up in the same code-path */
    pz_ReqItem = 
      ( qmiBitReqInfoStructT* )q_check(&z_QmiBit.z_RequestQ);
    while ( NULL != pz_ReqItem ) 
    {
      /* Handle Abort of the Request Item */
      qmiBitAbortRequestItem(pz_ReqItem);
      
      if ( FALSE ==  qmiBitDeallocateRequestItem(&pz_ReqItem) )
      {
        /* Assert earlier than causing a Dog-Time out, Since the item is 
           removed from the RequestQ, there should be no reason why we 
           cannot remove it from the RequestQ. If we do not ASSERT(0) here
           it would go in an infinite loop causing a Dog-Crash. */
        ASSERT(0);
      }
      /* Get the next Request Item.
         Note its q_Check() not q_get()*/
      pz_ReqItem = 
        ( qmiBitReqInfoStructT* )q_check(&z_QmiBit.z_RequestQ);

    }
    q_destroy(&z_QmiBit.z_RequestQ);


    /* Free all heap-memory from the FreeQ and destroy the FreeQ */
    pz_ReqItem = 
      ( qmiBitReqInfoStructT* )q_get(&z_QmiBit.z_RequestFreeQ);
    while ( NULL != pz_ReqItem ) 
    {
      os_MemFree( (void **) &pz_ReqItem);

      /* Get the next Request Item*/
      pz_ReqItem = 
        ( qmiBitReqInfoStructT* )q_get(&z_QmiBit.z_RequestFreeQ);
    }
    q_destroy(&z_QmiBit.z_RequestFreeQ);

    /* Free all the heap memory from the Connection FreeQ. All the Connection
       Items are moved to the FreeQ during the DisconectCb.*/
    pz_ConnItem = 
      ( qmiBitConnectionInfoStructT* )q_get(&z_QmiBit.z_ConnectionFreeQ);
    while ( NULL != pz_ConnItem ) 
    {
      os_MemFree( (void **) &pz_ConnItem);

      /* Get the next Request Item*/
      pz_ConnItem = 
        ( qmiBitConnectionInfoStructT* )q_get(&z_QmiBit.z_ConnectionFreeQ);
    }
    q_destroy(&z_QmiBit.z_ConnectionFreeQ);
  }
}

/* -----------------------------------------------------------------------*//**
@brief
  This function is called by the Service owner indicating that there is a 
  QMI-Event that is queued up in QCSI that needs to be handled.
*//* ------------------------------------------------------------------------*/
void qmiBitHandleEvent
( 
  void 
)
{
  qmi_csi_error       e_Err;
  qmi_csi_os_params             z_OsParams;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the TCB for the task in which initialization of shim was called 
     Assumption is that this function will be called in the service provider's
     context, and hence os_Self() should return the service provider's 
     context. If called in someone else's context, qmi_csi_handle_event() 
     should return an error */

  z_OsParams.tcb = os_Self();
  z_OsParams.sig = QMI_BIT_SIGNAL;

  QMI_BIT_MSG_HIGH("Handle QMI Event", 0, 0,0);
  e_Err = mqcsi_handle_event ( z_QmiBit.p_ServiceHandle,
                               &z_OsParams
                               );

  if ( QMI_CSI_NO_ERR != e_Err )
  {
    QMI_BIT_MSG_ERROR( "Could not handle the event properly Err:%d ",
                    e_Err, 0, 0); 
  }
}



