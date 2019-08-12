#ifndef PDSM_ATL_H
#define PDSM_ATL_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

      PDSM ABSTRACT TRANSPORT LAYER INTERFACE INTERNAL HEADER FILE

DESCRIPTION
  This header file contains the data/API definition for the interface between 
  pdsm abstract transport layer and transport layer proxy.

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/13   ssu      Adding a new WWAN type used for AGNSS Emergency connections.  
06/07/11   ss       Supporting multiple APN over ATL
02/26/09    dx     fix rpc meta-comment for rpcgen
06/02/08    kj     de-featurize this API header file
10/11/07    bp     changes for incorporating ATL based DNS lookup functionality
10/17/06    cl     Moved FEATURE_PDAPI
10/16/06    rw     Create iface name struct, remove dependency with DS
09/08/06    rw     Add support for multiple PDP contexts
07/11/05    rw     Add L2 proxy dereg function and L2 proxy request ack events
06/01/06    rw     Add special registration function for PPP proxy Open/Close
02/24/06    cl     Initial Release

===========================================================================*/

#include "comdef.h"    /* Definition for basic types and macros */

/* Upperbound of ATL send/recv buffer (limited by RPC) */
#define LIMIT_MAX_BUF_SIZE    0x3FFF  /* 16K */  

typedef uint32 pdsm_atl_type;
#define PDSM_ATL_TYPE_NONE    0x0000
#define PDSM_ATL_TYPE_IP      0x0001
#define PDSM_ATL_TYPE_DBM     0x0002

typedef enum {
  PDSM_ATL_PROTOCOL_TYPE_NONE,
  PDSM_ATL_PROTOCOL_TYPE_IS801,
  PDSM_ATL_PROTOCOL_TYPE_V1,
  PDSM_ATL_PROTOCOL_TYPE_V2,
  PDSM_ATL_PROTOCOL_TYPE_JCDMA, 
  PDSM_ATL_PROTOCOL_TYPE_MAX,
} pdsm_atl_protocol_type_e_type;

typedef enum {
  PDSM_ATL_SESSION_HANDLE_OPERATION__NONE = -1,
  PDSM_ATL_SESSION_HANDLE_OPERATION_ALLOCATE,
  PDSM_ATL_SESSION_HANDLE_OPERATION_DEALLOCATE  
} pdsm_atl_session_handle_operation_e_type;

/*---------------------------------------------------------------------------
                  PDSM ATL Events from Communication Manager
---------------------------------------------------------------------------*/
typedef int pdsm_atl_session_handle_type;

typedef uint32 pdsm_atl_event_type;
#define PDSM_ATL_EVENT_CLEAR                             0x0000
#define PDSM_ATL_EVENT_OPEN_SUCCESS                      0x0001
#define PDSM_ATL_EVENT_OPEN_FAIL                         0x0002
#define PDSM_ATL_EVENT_CLOSE_SUCCESS                     0x0004
#define PDSM_ATL_EVENT_CLOSE_FAIL                        0x0008
#define PDSM_ATL_EVENT_CONNECT_SUCCESS                   0x0010 
#define PDSM_ATL_EVENT_CONNECT_FAIL                      0x0020
#define PDSM_ATL_EVENT_DISCONNECT_SUCCESS                0x0040 
#define PDSM_ATL_EVENT_DISCONNECT_FAIL                   0x0080
#define PDSM_ATL_EVENT_SEND                              0x0100
#define PDSM_ATL_EVENT_RECV                              0x0200
#define PDSM_ATL_EVENT_FORCE_DORMANCY_SUCCESS            0x0400 
#define PDSM_ATL_EVENT_FORCE_DORMANCY_FAIL               0x0800
#define PDSM_ATL_EVENT_UNFORCE_DORMANCY_SUCCESS          0x1000
#define PDSM_ATL_EVENT_UNFORCE_DORMANCY_FAIL             0x2000
#define PDSM_ATL_EVENT_NETWORK_FAILURE                   0x4000
#define PDSM_ATL_EVENT_SOCKET_FAILURE                    0x8000
#define PDSM_ATL_EVENT_L2_PROXY_OPEN_ACK                 0x10000
#define PDSM_ATL_EVENT_L2_PROXY_CLOSE_ACK                0x20000
#define PDSM_ATL_EVENT_DNS_LOOKUP_SUCCESS                0x40000
#define PDSM_ATL_EVENT_DNS_LOOKUP_FAIL                   0x80000
#define PDSM_ATL_EVENT_DNS_LOOKUP_REQUEST_ACK            0x100000

typedef enum pdsm_atl_operation_return_value
{
  PDSM_ATL_OPERATION_SUCCESS = 0,
  PDSM_ATL_OPERATION_WAIT,
  PDSM_ATL_OPERATION_FAIL
} pdsm_atl_operation_return_value_e_type;

typedef enum pdsm_atl_dormancy_operation_e{
  PDSM_ATL_DORMANCY_OPERATION_NONE = -1,  
  PDSM_ATL_DORMANCY_OPERATION_FORCE,  
  PDSM_ATL_DORMANCY_OPERATION_UNFORCE  
} pdsm_atl_dormancy_operation_e_type;

/*---------------------------------------------------------------------------
                          Multiple PDP context
                     (Copy from "dsumtspdpreg.h")
---------------------------------------------------------------------------*/
#define  PDSM_ATL_MAX_APN_STRING_LEN    100   /* Max length of APN string    */

typedef enum
{
  PDSM_ATL_PDP_IP=0x0,                     /* PDP type IP (IPV4)          */
  PDSM_ATL_PDP_PPP,                        /* PDP type PPP                */
  PDSM_ATL_PDP_IPV6,                       /* PDP type IPV6               */
  PDSM_ATL_PDP_IPV4V6,                     /* PDP type IPV4V6             */
  PDSM_ATL_PDP_MAX=0x10000000              /* Force enum to be 32 bit     */
} pdsm_atl_pdp_type_e_type;


/*---------------------------------------------------------------------------
TYPEDEF PDSM_ATL_ADDR_FAMILY_ENUM_TYPE

DESCRIPTION
  enum which discriminates the union that defines the IP addresses type 
  in the structure pdsm_atl_ip_addr_type
---------------------------------------------------------------------------*/
typedef enum
{
  PDSM_ATL_IP_V4  = 1,                    /* Protocol Family - Internet v4 */
  PDSM_ATL_IP_V6  = 2                     /* Protocol Family - Internet v6 */
} pdsm_atl_addr_family_e_type;


/*---------------------------------------------------------------------------
TYPEDEF PDSM_ATL_IP_ADDR_TYPE

DESCRIPTION
  structure which is a discriminated union that defines the IP addresses that
  we support.
---------------------------------------------------------------------------*/
typedef struct
{
  pdsm_atl_addr_family_e_type type; 

  union
  {
    uint32 v4_addr;   
    /*~ IF (_DISC_ == PDSM_ATL_IP_V4) pdsm_atl_ip_addr_type.addr.v4_addr*/
    uint8 v6_addr[16];
    /*~ IF (_DISC_ == PDSM_ATL_IP_V6) pdsm_atl_ip_addr_type.addr.v6_addr */
    /*~ DEFAULT pdsm_atl_ip_addr_type.addr.void */
  } addr; 
  /*~ FIELD pdsm_atl_ip_addr_type.addr DISC pdsm_atl_ip_addr_type.type*/
} pdsm_atl_ip_addr_type;

/*---------------------------------------------------------------------------
  Definition of iface names
---------------------------------------------------------------------------*/
typedef enum pdsm_atl_iface_e {
  PDSM_ATL_IFACE_NONE = -1, 
  PDSM_ATL_IFACE_CDMA_SN = 0,
  PDSM_ATL_IFACE_CDMA_AN,
  PDSM_ATL_IFACE_UMTS,
  PDSM_ATL_IFACE_SIO,
  PDSM_ATL_IFACE_LO,
  PDSM_ATL_IFACE_WWAN,
  PDSM_ATL_IFACE_ANY_DEFAULT,
  PDSM_ATL_IFACE_ANY,
  PDSM_ATL_IFACE_RM,
  PDSM_ATL_IFACE_MAX
} pdsm_atl_iface_name_e_type;

typedef enum
{
  PDSM_ATL_CONNECTION_LBS=0x0,        /*LBS connection*/
  PDSM_ATL_CONNECTION_WWAN_INTERNET,  /*ANY WWAN Internet connection*/
  PDSM_ATL_CONNECTION_LBS_EMERGENCY,  /*LBS Emergency Connection*/
  PDSM_ATL_CONNECTION_MAX = 0x1000000
} pdsm_atl_connection_e_type;


/*---------------------------------------------------------------------------
TYPEDEF PDSM_ATL_OPEN_PARAM_TYPE

DESCRIPTION
  structure which used by the pdsm_atl_open_f_type functions
---------------------------------------------------------------------------*/
typedef struct
{
  pdsm_atl_iface_name_e_type   iface_name;    /* Interface name */
  pdsm_atl_addr_family_e_type  iface_family;  /* Interface family */
  pdsm_atl_ip_addr_type        iface_ip_addr; /* Interface IP address, IPv4 or IPv6 */
  pdsm_atl_pdp_type_e_type     pdp_type;      /* PDP call type*/
  pdsm_atl_connection_e_type   connection_type;  /* Connection type*/
  char                         apn_name[PDSM_ATL_MAX_APN_STRING_LEN+1];  /* APN Name */
  /*~ FIELD pdsm_atl_open_params_type.apn_name STRING 101 */
} pdsm_atl_open_params_type;

/*---------------------------------------------------------------------------
TYPEDEF PDSM_ATL_HOST_TYPE

DESCRIPTION
  enum which discriminates the fields in the structure 
  pdsm_atl_connect_param_type.
---------------------------------------------------------------------------*/
typedef enum
{
  IP_ADDR,
  HOST_NAME
} pdsm_atl_host_e_type;

#define PDSM_ATL_MAX_URL_STRING_LEN 256
/*---------------------------------------------------------------------------
TYPEDEF PDSM_ATL_CONNECT_PARAM_TYPE

DESCRIPTION
  structure which used by the pdsm_atl_connect_f_type functions
---------------------------------------------------------------------------*/
typedef struct
{
  pdsm_atl_host_e_type      adr_type;
  uint16                    ip_port;
  pdsm_atl_ip_addr_type     ip_addr;
  char                      host_name[PDSM_ATL_MAX_URL_STRING_LEN];
  /*~ FIELD pdsm_atl_connect_params_type.host_name STRING 256 */
  pdsm_atl_open_params_type iface_param;
} pdsm_atl_connect_params_type;


/*---------------------------------------------------------------------------
TYPEDEF PDSM_ATL_DNS_PARAMS_TYPE

DESCRIPTION
  This structure is used by the pdsm_atl_dns_lookup_f_type function to
  provide DNS lookup parameters to the DNS Proxy.

  If url is a non-NULL string, then code running on the App Processor
  is expected to resolve this string into an IP Address and Port needed
  by the modem.  In most cases, this means doing a DNS lookup of this
  URL on the App Processor.  In some cases, it may simply mean converting
  this string into a 32-bit IP Address and port since this string is
  already in that type of format.  For example, if the URL string is
  "10.20.5.35:8000", then code on the App Process can just convert
  this into an IP Address and store 8000 into the IP Port.  If
  the URL string is in FQDN format like "www.myserver.com:8000", then
  a DNS lookup will be required.  The bottom line is that the
  SUPL code inside the modem can only work with an IP Address and Port,
  not an FQDN style string.

---------------------------------------------------------------------------*/
typedef struct
{
  char    url[PDSM_ATL_MAX_URL_STRING_LEN+1];
  /*~ FIELD pdsm_atl_dns_params_type.url STRING 256 */
} pdsm_atl_dns_params_type;


/*---------------------------------------------------------------------------
TYPEDEF PDSM_ATL_DNS_RESPONSE_TYPE

DESCRIPTION
  This structure is used by the pdsm_atl_dns_response function to
  provide DNS lookup results to the PDSM Core.

---------------------------------------------------------------------------*/
typedef struct
{
  pdsm_atl_ip_addr_type        ip_addr;       /* IP address, IPv4 or IPv6 */
  uint16                       ip_port;       /* IP Port Number */
} pdsm_atl_dns_response_type;

/********************************************************************************************/
/*                                                                                          */
/* Below is the function prototypes which transport layer proxy task should provide to PDSM */
/*                                                                                          */
/********************************************************************************************/

/*===========================================================================

FUNCTION: pdsm_atl_session_handle_operation_f_type

DESCRIPTION:
  This function is called by PDSM to get a session handle from transport layer 
  proxy for IS801, V1/V2, and JCDMA sessions. Transport layer proxy uses session 
  handle to differentiate different sessions for incoming requests 
  (open/close/send/revc..etc). 
  When PDSM calls this function, it also specifies what transport layer type it needs. 
  If a transport layer proxy is implemented to handle multiple transport layer type, 
  it uses this information to invoke proper transport layer functions.
===========================================================================*/
typedef boolean (pdsm_atl_session_handle_operation_f_type)
(
  pdsm_atl_session_handle_operation_e_type   session_handle_operation,
  pdsm_atl_type                              transport_layer_type, 
  pdsm_atl_protocol_type_e_type              protocol_type,
  pdsm_atl_session_handle_type               *session_handle
  /*~ PARAM OUT session_handle POINTER */  
);
/*~ CALLBACK pdsm_atl_session_handle_operation_f_type
    ONERROR return FALSE
*/

/*===========================================================================

FUNCTION: pdsm_atl_open_f_type

DESCRIPTION:
  This function is used to establish physical link to the network. If no physical 
  link needs to be established, this function should return 
  PDSM_ATL_OPERATION_SUCCESS. If for some reason, physical can not be 
  established, it should return PDSM_ATL_OPERATION_FAIL. Otherwise, 
  it should return PDSM_ATL_OPERATION_WAIT to inform PDSM that physical 
  link is in the process of being brought up.
===========================================================================*/
typedef	pdsm_atl_operation_return_value_e_type (pdsm_atl_open_f_type)
(
  pdsm_atl_session_handle_type  session_handle,
  pdsm_atl_open_params_type     open_param
);
/*~ CALLBACK pdsm_atl_open_f_type
    ONERROR return PDSM_ATL_OPERATION_FAIL */


/*===========================================================================

FUNCTION: pdsm_atl_close_f_type

DESCRIPTION:
  This function is used to tear down physical link to the network. If no physical 
  link needs to be torn down, this function should return 
  PDSM_ATL_OPERATION_SUCCESS. If for some reason, physical can not be 
  torn down, it should return PDSM_ATL_OPERATION_FAIL. Otherwise, 
  it should return PDSM_ATL_OPERATION_WAIT to inform PDSM that physical 
  link is in the processor of being torn down.
===========================================================================*/
typedef	pdsm_atl_operation_return_value_e_type (pdsm_atl_close_f_type)
(
  pdsm_atl_session_handle_type  session_handle,
  boolean                       e911_close
);
/*~ CALLBACK pdsm_atl_close_f_type
    ONERROR return PDSM_ATL_OPERATION_FAIL
*/


/*===========================================================================

FUNCTION: pdsm_atl_connect_f_type

DESCRIPTION:
  This function is used to establish end-to-end connection to a network server. 
  If no end-to-end connection needs to be established, this function should 
  return PDSM_ATL_OPERATION_SUCCESS. If for some reason, end-to-end 
  connection can not be established, it should return PDSM_ATL_OPERATION_FAIL. 
  Otherwise, it should return PDSM_ATL_OPERATION_WAIT to inform PDSM 
  that end-to-end connection is in the process of being established.
===========================================================================*/
typedef	pdsm_atl_operation_return_value_e_type (pdsm_atl_connect_f_type)
(
  pdsm_atl_session_handle_type  session_handle,
  pdsm_atl_connect_params_type  connect_param
);
/*~ CALLBACK pdsm_atl_connect_f_type
    ONERROR return PDSM_ATL_OPERATION_FAIL
*/


/*===========================================================================
FUNCTION: pdsm_atl_dns_lookup_f_type

DESCRIPTION:
  This function is used to request the DNS Proxy to do a DNS lookup.
  PDSM_ATL_OPERATION_FAIL should be returned if the DNS lookup cannot be done, 
  otherwise PDSM_ATL_OPERATION_WAIT should be returned which indicates that
  a DNS lookup is in progress and results will be returned later.

===========================================================================*/
typedef pdsm_atl_operation_return_value_e_type (pdsm_atl_dns_lookup_f_type) 
(
  pdsm_atl_session_handle_type    session_handle,
  pdsm_atl_dns_params_type        dns_lookup_params
 );
/*~ CALLBACK pdsm_atl_dns_lookup_f_type
    ONERROR return PDSM_ATL_OPERATION_FAIL
*/


/*===========================================================================

FUNCTION: pdsm_atl_disconnect_f_type

DESCRIPTION:
  This function is used to tear down end-to-end connection to a network server. 
  If no end-to-end connection needs to be torn down, this function should return 
  PDSM_ATL_OPERATION_SUCCESS. If for some reason, end-to-end 
  connection can not be torn down, it should return PDSM_ATL_OPERATION_FAIL. 
  Otherwise, it should return PDSM_ATL_OPERATION_WAIT to inform PDSM that 
  end-to-end connection is in the process of being torn down.
===========================================================================*/
typedef	pdsm_atl_operation_return_value_e_type (pdsm_atl_disconnect_f_type)
(
  pdsm_atl_session_handle_type  session_handle,
  boolean                       silent_disconnect
);
/*~ CALLBACK pdsm_atl_disconnect_f_type
    ONERROR return PDSM_ATL_OPERATION_FAIL
*/

/*===========================================================================

FUNCTION: pdsm_atl_send_f_type

DESCRIPTION:
  This function is used by PDSMto send data to the network. It passes a pointer 
  to a data buffer (*start) and also specifies the size (in bytes) of the data buffer. 
  If for some reason, transport layer proxy is not able to process the send request, 
  it should return -1 to inform PDSM that the send request has failed. Otherwise, 
  it should return the size (in bytes) of data it processes.
===========================================================================*/
typedef	int16 (pdsm_atl_send_f_type)
(
  pdsm_atl_session_handle_type  session_handle,
  byte                          *start, 
 /*~ PARAM start VARRAY LIMIT_MAX_BUF_SIZE LENGTH length */   
  uint16                        length
);
/*~ CALLBACK pdsm_atl_send_f_type
    ONERROR return -1
*/

/*===========================================================================

FUNCTION: pdsm_atl_recv_f_type

DESCRIPTION:
  This function is used by PDSM to receive data from transport layer proxy, 
  when it gets read event from transport layer proxy. Upon getting recv request, 
  transport layer proxy should copy incoming data to the data buffer pointed 
  by *start. If incoming data size is larger than max_buff_size, it should only 
  copy max_buf_size to the data buffer pointed by *start. 
  Transport layer proxy should return the number of bytes copied to the data buffer. 
  A return value of -1 is a special case which means transport layer proxy gets EOF 
  from the network and it is used to inform PDSM to tear down the connection.
===========================================================================*/
typedef int16 (pdsm_atl_recv_f_type)
(
  pdsm_atl_session_handle_type  session_handle,
  byte                          *start, 
  /*~ PARAM OUT start VARRAY LIMIT_MAX_BUF_SIZE LENGTH max_buf_size */          
  uint16                        max_buf_size
);
/*~ CALLBACK pdsm_atl_recv_f_type
    ONERROR return -1
*/

/*===========================================================================

FUNCTION: pdsm_atl_dormancy_operation_f_type

DESCRIPTION:
  This function is used by PDSM to force dormancy and unforce dormancy. 
  If a transport layer proxy does not support force dormancy operation, 
  it should return FALSE. 
===========================================================================*/
typedef boolean (pdsm_atl_dormancy_operation_f_type)
(
  pdsm_atl_session_handle_type         session_handle,
  pdsm_atl_dormancy_operation_e_type   dormancy_operation
);
/*~ CALLBACK pdsm_atl_dormancy_operation_f_type
    ONERROR return FALSE
*/

/*===========================================================================

FUNCTION pdsm_atl_reg

DESCRIPTION
  This function is called by transport layer proxy to inform PDSM that it is ready 
  to serve.
  
DEPENDENCIES

RETURN VALUE
  TURE: If PDSM abstract layer allows registration
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_atl_reg
( 
  pdsm_atl_type                                  transport_layer_type, 
  pdsm_atl_session_handle_operation_f_type       *session_handle_operation_fp,
   /*~ PARAM session_handle_operation_fp POINTER */
  pdsm_atl_open_f_type                           *open_fp,
   /*~ PARAM open_fp POINTER */
  pdsm_atl_close_f_type                          *close_fp,
   /*~ PARAM close_fp POINTER */
  pdsm_atl_connect_f_type                        *connect_fp,
   /*~ PARAM connect_fp POINTER */
  pdsm_atl_disconnect_f_type                     *disconnect_fp,
   /*~ PARAM disconnect_fp POINTER */
  pdsm_atl_send_f_type                           *send_fp,
   /*~ PARAM send_fp POINTER */
  pdsm_atl_recv_f_type                           *recv_fp,
   /*~ PARAM recv_fp POINTER */
  pdsm_atl_dormancy_operation_f_type             *dormancy_fp
   /*~ PARAM dormancy_fp POINTER */
);
/*~ FUNCTION pdsm_atl_reg */


/*===========================================================================

FUNCTION pdsm_atl_l2_proxy_reg

DESCRIPTION
  Proxy PPP clients (i.e. co-processor task) must call this function to 
  register with PDSM-Core to forward all PPP connection request to a PPP
  proxy client.

DEPENDENCIES

RETURN VALUE
  TRUE - Successfully Registered with PDSM-Core
  FALSE - Error

SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_atl_l2_proxy_reg
( 
  pdsm_atl_type                                  transport_layer_type, 

  pdsm_atl_open_f_type                           *open_fp,
  /*~ PARAM open_fp POINTER */
  pdsm_atl_close_f_type                          *close_fp
  /*~ PARAM close_fp POINTER */
);
/*~ FUNCTION pdsm_atl_l2_proxy_reg 
    RELEASE_FUNC pdsm_atl_l2_proxy_dereg(transport_layer_type) */

/*===========================================================================

FUNCTION pdsm_atl_l2_proxy_dereg

DESCRIPTION
  Proxy PPP clients (i.e. co-processor task) must call this function to 
  deregister with PDSM-Core indicating it no longer will proxy the connection.

DEPENDENCIES

RETURN VALUE
  TRUE - Successfully deregistered with PDSM-Core
  FALSE - Error

SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_atl_l2_proxy_dereg
( 
  pdsm_atl_type                                  transport_layer_type 
);
/*~ FUNCTION pdsm_atl_l2_proxy_dereg */


/*===========================================================================

FUNCTION pdsm_atl_post_event

DESCRIPTION
  This function is called by transport layer proxy to inform PDSM network events
  
DEPENDENCIES

RETURN VALUE
  TURE: If PDSM abstract layer recognizes the event and is able to send event to upper
        layer.
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_atl_post_event 
(
  pdsm_atl_type                 transport_layer_type, 
  pdsm_atl_session_handle_type  session_handle, 
  pdsm_atl_event_type           event,
  pdsm_atl_open_params_type     open_param_info
);
/*~ FUNCTION pdsm_atl_post_event */


/*===========================================================================
bnp
FUNCTION pdsm_atl_dns_proxy_reg

DESCRIPTION
  Proxy DNS service providers (eg: Serial PC-PDAPI s/w) call this function
  to register callbacks with the PDSM core, which may be invoked to 
  resolve names to IP addresses.

DEPENDENCIES

RETURN VALUE
  TRUE - Successfully Registered with PDSM-Core
  FALSE - Error

SIDE EFFECTS

===========================================================================*/
boolean pdsm_atl_dns_proxy_reg
( 
 pdsm_atl_type                                  transport_layer_type, 
 pdsm_atl_dns_lookup_f_type              *p_dns_lookup
  /*~ PARAM p_dns_lookup POINTER */
);
/*~ FUNCTION pdsm_atl_dns_proxy_reg */

/*===========================================================================

FUNCTION pdsm_atl_dns_proxy_dereg

DESCRIPTION
  The DNS Proxy can call this function to deregister with PDSM-Core

DEPENDENCIES

RETURN VALUE
  TRUE - Successfully Deregistered with PDSM-Core
  FALSE - Error

SIDE EFFECTS

===========================================================================*/
boolean pdsm_atl_dns_proxy_dereg
( 
  pdsm_atl_type                                  transport_layer_type 
);
/*~ FUNCTION pdsm_atl_dns_proxy_dereg */

/*===========================================================================

FUNCTION pdsm_atl_dns_proxy_response

DESCRIPTION
  The DNS Proxy Clients must call this function to provide the DNS lookup
  results to PDSM-Core

DEPENDENCIES

RETURN VALUE
  TRUE - If PDSM ATL is able to accept the response and send it to upper layers
  FALSE - Otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_atl_dns_proxy_response
( 
  pdsm_atl_type                 transport_layer_type, 
  pdsm_atl_session_handle_type  session_handle, 
  pdsm_atl_event_type           event,
  pdsm_atl_dns_response_type    dns_results
  );
/*~ FUNCTION pdsm_atl_dns_proxy_response */

#ifdef __cplusplus
}
#endif

#endif /* PDSM_ATL_H */

