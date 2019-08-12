#ifndef _DSSOCKET_H
#define _DSSOCKET_H

/*===========================================================================

   D A T A   S E R V I C E S   S O C K E T   A P I   H E A D E R   F I L E

DESCRIPTION

 The Data Services Socket Header File. Contains shared variables and enums,
 as well as declarations for functions.

Copyright (c) 1998-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dssocket.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "dserrno.h"
#include "dssocket_defs.h"
#include "dss_netpolicy.h"
#include "ps_iface_defs.h"
#include "dss_iface_ioctl.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
                          DSS VERSION  MajorMinor
The least siginficant digit is the minor version number while the  most
significant digits form the major version number.
---------------------------------------------------------------------------*/
#define DSS_VERSION 2421

/*---------------------------------------------------------------------------
           Network and socket call back declarations for opennetlib2.
---------------------------------------------------------------------------*/
typedef void (*dss_net_cb_fcn)
(
  sint15            dss_nethandle,                               /* Application id */
  dss_iface_id_type iface_id,                    /* Interfcae id structure */
  sint15            dss_errno, /* type of network error, ENETISCONN, ENETNONET.*/
  void            * net_cb_user_data               /* Call back User data  */
);

typedef void (*dss_sock_cb_fcn)
(
  sint15 dss_nethandle,                                          /* Application id */
  sint15 sockfd,                                      /* socket descriptor */
  uint32 event_mask,                                     /* Event occurred */
  void * sock_cb_user_data       /* User data specfied during registration */
);

/*---------------------------------------------------------------------------
                       Socket Options Data Types
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  The data type used for changing the socket call back function using the
  socket option DSS_SO_CB_FN.
---------------------------------------------------------------------------*/
typedef struct
{
  dss_sock_cb_fcn  sock_cb_fcn;
  void*            sock_cb_user_data;
} dss_sock_cb_fcn_type;

/*---------------------------------------------------------------------------
  Definition for various options that affect the behaviors of a socket.
  Only the options whose names are defined here are supported. The
  following definition is also used as index to the table of all supported
  options.

  +----------------------+-----------+--------------+-----------+-----------+
  |  Name                |   type    |  default val |  min val  |  max val  |
  +----------------------+-----------+--------------+-----------+-----------+
  |  DSS_IP_TTL          |    int    |     255      |     0     |    255    |
  |  DSS_SO_SYS_SOCK     |   bool    |    False     |    N/A    |    N/A    |
  |  DSS_SO_SILENT_CLOSE |   bool    |    False     |    N/A    |    N/A    |
  |  DSS_SO_RCVBUF **    |    int    |     2144     |   2144    |   17520   |
  |  DSS_SO_LINGER       |  dss_so_  |    {0,0}     |   {0,0}   |  {1, N/A} |
  |                      |linger_type|              |           |           |
  |  DSS_SO_SNDBUF **    |    int    |     2144     |    536    |   18432   |
  |  DSS_TCP_MAXSEG      |    int    |      536     |    536    |   18432   |
  |  DSS_SO_SDB_ACK_CB   |dss_so_sdb_| (NULL, NULL) |    N/A    |    N/A    |
  |                      |ack_cb_type|              |           |           |
  |  DSS_TCP_NODELAY     |   bool    |    FALSE     |    N/A    |    N/A    |
  |  DSS_SO_KEEPALIVE    |   bool    |    FALSE     |    N/A    |    N/A    |
  |  DSS_SO_NETPOLICY    |dss_net_   |  DEF_POLICY  |    N/A    |    N/A    |
  |                      |policy_type|              |           |           |
  |  DSS_SO_TX_IFACE     |dss_iface_ |DSS_INVALID_  |    N/A    |    N/A    |
  |                      |id_type    | IFACE_ID     |           |           |
  |  DSS_TCP_DELAYED_ACK |   bool    |    FALSE     |    N/A    |    N/A    |
  |  DSS_TCP_SACK        |   bool    |    FALSE     |    N/A    |    N/A    |
  |  DSS_TCP_TIME_STAMP  |   bool    |    FALSE     |    N/A    |    N/A    |
  |  DSS_BCMCS_JOIN      |dss_so_ip_ |    0         |    N/A    |    N/A    |
  |                      |addr_type  |              |           |           |
  |  DSS_BCMCS_LEAVE     |dss_so_ip_ |    0         |    N/A    |    N/A    |
  |                      |addr_type  |              |           |           |
  |  DSS_SO_CB_FCN       |dss_sock_cb| (NULL, NULL) |    N/A    |    N/A    |
  |                      |_fcn_type  |              |           |           |
  |  DSS_TCP_EIFEL       |   bool    |    FALSE     |    N/A    |    N/A    |
  |  DSS_SO_QOS_SHARE_HANDLE int     |     0        |    N/A    |    N/A    |
  |  DSS_SO_REUSEADDR    |   bool    |    FALSE     |    N/A    |    N/A    |
  |  DSS_SO_DISABLE_     |   bool    |    FALSE     |    N/A    |    N/A    |
  |          FLOW_FWDING |           |              |           |           |
  |  DSS_IP_ADD_MEMBERSHIP dss_ip_   |    N/A       |    N/A    |    N/A    |
  |                      |    mreqn  |              |           |           |
  |  DSS_IP_DROP_        | dss_ip_   |    N/A       |    N/A    |    N/A    |
  |         MEMBERSHIP   |    mreqn  |              |           |           |
  |  DSS_IP_MULTICAST_TTL| uint8     |    1         |    0      |    255    |
  |  DSS_TCP_FIONREAD    | uint32    |    N/A       |    0      | 0xFFFFFFFF|
  |  DSS_IP_LAST_RECEIVED| uint8     |    0         |    0      |    255    |
  |                 _TOS |           |              |           |           |
  |  DSS_IP_MULTICAST_LOOP uint8     |    FALSE     |    N/A    |    N/A    |
  |  DSS_SO_FIONREAD     | uint32    |    N/A       |    0      | 0xFFFFFFFF|
  +----------------------+-----------+--------------+-----------+-----------+

  ** NOTE: the use of this option REQUIRES that more memory be added to the
           DSM item pool.  For each socket that expands its window the
           appropriate memory needs to be added.
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_SOCKOPT_MIN     = -1,           /* lower bound                          */
  DSS_IP_TTL          =  0,           /* time-to-live                         */
  DSS_SO_SYS_SOCK     =  1,           /* bool: is this a system socket?       */
  DSS_SO_SILENT_CLOSE =  2,           /* bool: close() call causes conn reset */
  DSS_SO_RCVBUF       =  3,           /* set the receive window size          */
  DSS_SO_LINGER       =  4,           /* linger on close                      */
  DSS_SO_SNDBUF       =  5,           /* set/get the sndbuf queue size        */
  DSS_TCP_MAXSEG      =  6,           /* set/get the TCP maximum segement size*/
  DSS_SO_SDB_ACK_CB   =  7,           /* call a cb upon recv'ing SDB data ack */
  DSS_TCP_NODELAY     =  8,           /* Disable Nagle's algorithm            */
  DSS_SO_KEEPALIVE    =  9,           /* Send keepalive probes?               */
  DSS_SO_NETPOLICY    =  10,          /* get socket netpolicy                 */
  DSS_SO_TX_IFACE     =  11,          /* get tx iface id                      */
  DSS_TCP_DELAYED_ACK =  12,          /* Enable delayed ack                   */
  DSS_TCP_SACK        =  13,          /* Enable SACK                          */
  DSS_TCP_TIME_STAMP  =  14,          /* Enable TCP time stamp option         */
  DSS_BCMCS_JOIN      =  15,          /* Join m'cast group                    */
  DSS_BCMCS_LEAVE     =  16,          /* Leave m'cast group                   */
  DSS_IP_RECVIF       =  17,          /* Get incoming packet's interface      */
  DSS_IP_TOS          =  18,          /* Type of Service                      */
  DSS_IPV6_TCLASS     =  19,          /* Traffic class for V6 sockets         */
  DSS_SO_CB_FCN       =  20,          /* set the socket callback function     */
  DSS_SO_ERROR_ENABLE =  21,          /* Enable storage of ICMP err in so_err */
  DSS_SO_ERROR        =  22,          /* Get value of ICMP so_error           */
  DSS_SO_LINGER_RESET =  23,          /* Linger and reset on timeout          */
  DSS_IP_RECVERR      =  24,          /* Enable getting ICMP error pkts       */
  DSS_IPV6_RECVERR    =  25,          /* Enable getting ICMPv6 error pkts     */
  DSS_TCP_EIFEL       =  26,          /* Enable EIFEL Algorithm               */
  DSS_SO_QOS_SHARE_HANDLE = 27,       /* QOS group handle                     */
  DSS_SO_REUSEADDR    =  28,          /* Enable Socket Reuse                  */
  DSS_SO_DISABLE_FLOW_FWDING =  29,   /* Disable forwarding data on best effort
                                         flow if QoS flow can't be used       */
  DSS_ICMP_ECHO_ID      = 30,         /* ICMP ECHO_REQ message, identifier    */
  DSS_ICMP_ECHO_SEQ_NUM = 31,         /* ICMP ECHO_REQ message, sequence num  */
  DSS_IP_ADD_MEMBERSHIP = 32,         /* Requests that the system join a
                                         multicast group                      */
  DSS_IP_DROP_MEMBERSHIP = 33,        /* Allows the system to leave a multicast
                                         group                                */
  DSS_IPV6_ADD_MEMBERSHIP = 34,
  DSS_IPV6_DROP_MEMBERSHIP = 35,
  DSS_IP_MULTICAST_TTL = 36,          /* Specifies the time-to-live value for
                                         multicast datagrams sent through this
                                         socket                               */
  DSS_TCP_FIONREAD     = 37,          /* To get the TCP rcvq length           */
  DSS_IP_LAST_RECEIVED_TOS = 38,      /* To get TOS value in the last received
                                         IP packet. */
  DSS_IP_MULTICAST_LOOP = 39,         /* Enable/Disable multicast datagrams to
                                         be looped back */
  DSS_SO_FIONREAD = 40,               /* To get the TCP/UDP/ICMP socket rcvq 
                                         length. */
  DSS_SO_SKIP_ROUTE_SCOPE_CHECK = 41, /* To enable/disable socket route scope check */
  DSS_TCP_MAX_BACKOFF_TIME      = 42, /*  Max TCP backoff time */
  DSS_UDP_ENCAPS                = 45, /* Enable UDP encapsulation */
  DSS_SOCKOPT_MAX                     /* determine upper bound and array size */
} dss_sockopt_names_type;

/*---------------------------------------------------------------------------
  Socket option level specifies the code in the system to interpret the
  option: the general socket code or some protocol-specific code.  If
  an option is unknown at a given level, an error will be generated.
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_IPPROTO_IP   = 1,               /* IP protocol level                 */
  DSS_SOL_SOCKET   = 2,               /* socket level                      */
  DSS_SOCK         = DSS_SOL_SOCKET,  /* another alias for socket level    */
  DSS_IPPROTO_TCP  = 3,               /* TCP protocol level                */
  DSS_IPPROTO_IPV6 = 4,               /* IPV6 protocol level               */
  DSS_IPPROTO_ICMP  = 5,               /* ICMP protocol level              */
  DSS_IPPROTO_ICMP6 = 6,              /* ICMPv6 protocol level            */
  DSS_IPPROTO_UDP   = 7               /* UDP prptocol level                */
} dss_sockopt_levels_type;

/*---------------------------------------------------------------------------
  Structure to hold interface index and multicast group address. This
  structure is required when socket options DSS_IP_ADD_MEMBERSHIP or
  DSS_IP_DROP_MEMBERSHIP are used.
---------------------------------------------------------------------------*/
typedef struct {
    struct ps_in_addr mcast_grp;     /* IPV4 multicast group address */
    ps_ip_addr_type   local_address; /* IP address of local interface */
    dss_iface_id_type iface_id;      /* interface index */
} dss_ip_mreqn;

typedef struct {
  struct ps_in6_addr mcast_grp_v6;
  ps_ip_addr_type    local_address;
  dss_iface_id_type  iface_id;
}dss_ipv6_mreqn;

/*---------------------------------------------------------------------------
  The data type used for SO_LINGER socket option. Note that l_linger is in
  ms, not in seconds as on UNIX systems
---------------------------------------------------------------------------*/
typedef struct
{
  int l_onoff;                                     /* linger active or not */
  int l_linger;                   /* how many milli-secondss to linger for */
} dss_so_linger_type;

/*---------------------------------------------------------------------------
  Structures defined for the IP_RECVIF and IP_RECVSTADDR options.
---------------------------------------------------------------------------*/
typedef struct
{
  struct ps_in_addr ip_addr;                   /* destination IPv4 address */
  uint32            if_index;                  /* received interface index */
} dss_in_pktinfo_type;

typedef struct
{
  struct ps_in6_addr ip6_addr;                 /* destination IPv6 address */
  uint32             if_index;                 /* received interface index */
} dss_in6_pktinfo_type;

/*---------------------------------------------------------------------------
  The pointer to function data type used for registering callback for the
  SO_SDB_ACK_CB socket option.
---------------------------------------------------------------------------*/
typedef
void (*dss_so_sdb_ack_cb_fcn)
(
  sint15 sockfd,                                     /* socket descriptor  */
  dss_sdb_ack_status_info_type* sdb_ack_info,    /* info about the sdb ack */
  void*  user_data    /* supplied at the time of setting the socket option */
);

/*---------------------------------------------------------------------------
  The data type used for SO_SDB_ACK_CB socket option. The caller supplies
  user_data when setting the socket option and it is passed to the
  sdb_ack_cb_fcn() whenever it is invoked.
---------------------------------------------------------------------------*/
typedef struct
{
  dss_so_sdb_ack_cb_fcn sdb_ack_cb;
  void*                 user_data;
} dss_so_sdb_ack_cb_type;

/*---------------------------------------------------------------------------
  The values for the 'how' argument of the shutdown() call. Specifies what
  action to perform:
---------------------------------------------------------------------------*/
#define  DSS_SHUT_RD   0                      /* disallow further receives */
#define  DSS_SHUT_WR   1                         /* disallow further sends */
#define  DSS_SHUT_RDWR 2     /* disallow further receives as well as sends */

/*---------------------------------------------------------------------------
  Structures defined for the API dss_getnetdownreason().
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_NET_DOWN_REASON_NOT_SPECIFIED        = PS_NET_DOWN_REASON_NOT_SPECIFIED,
  DSS_NET_DOWN_REASON_CLOSE_IN_PROGRESS =
                                         PS_NET_DOWN_REASON_CLOSE_IN_PROGRESS,
  DSS_NET_DOWN_REASON_NW_INITIATED_TERMINATION =
                                  PS_NET_DOWN_REASON_NW_INITIATED_TERMINATION,
  DSS_NET_DOWN_REASON_APP_PREEMPTED = PS_NET_DOWN_REASON_APP_PREEMPTED,

  DSS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_THROTTLED = 
              PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_THROTTLED,
  DSS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_DISALLOWED = 
              PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_DISALLOWED,
  DSS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_THROTTLED = 
              PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_THROTTLED,
  DSS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_DISALLOWED = 
              PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_DISALLOWED,


  DSS_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING =
    PS_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING,
  DSS_NET_DOWN_REASON_LLC_SNDCP_FAILURE =
    PS_NET_DOWN_REASON_LLC_SNDCP_FAILURE,

  DSS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES =
                                    PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES,
  DSS_NET_DOWN_REASON_UNKNOWN_APN            = PS_NET_DOWN_REASON_UNKNOWN_APN,
  DSS_NET_DOWN_REASON_UNKNOWN_PDP            = PS_NET_DOWN_REASON_UNKNOWN_PDP,
  DSS_NET_DOWN_REASON_AUTH_FAILED            = PS_NET_DOWN_REASON_AUTH_FAILED,
  DSS_NET_DOWN_REASON_GGSN_REJECT            = PS_NET_DOWN_REASON_GGSN_REJECT,
  DSS_NET_DOWN_REASON_ACTIVATION_REJECT = PS_NET_DOWN_REASON_ACTIVATION_REJECT,
  DSS_NET_DOWN_REASON_OPTION_NOT_SUPPORTED =
                                      PS_NET_DOWN_REASON_OPTION_NOT_SUPPORTED,
  DSS_NET_DOWN_REASON_OPTION_UNSUBSCRIBED =
                                       PS_NET_DOWN_REASON_OPTION_UNSUBSCRIBED,
  DSS_NET_DOWN_REASON_OPTION_TEMP_OOO    = PS_NET_DOWN_REASON_OPTION_TEMP_OOO,
  DSS_NET_DOWN_REASON_NSAPI_ALREADY_USED =
                                        PS_NET_DOWN_REASON_NSAPI_ALREADY_USED,
  DSS_NET_DOWN_REASON_REGULAR_DEACTIVATION =
                                      PS_NET_DOWN_REASON_REGULAR_DEACTIVATION,
  DSS_NET_DOWN_REASON_QOS_NOT_ACCEPTED  = PS_NET_DOWN_REASON_QOS_NOT_ACCEPTED,
  DSS_NET_DOWN_REASON_NETWORK_FAILURE    = PS_NET_DOWN_REASON_NETWORK_FAILURE,
  DSS_NET_DOWN_REASON_UMTS_REATTACH_REQ =
                                         PS_NET_DOWN_REASON_UMTS_REATTACH_REQ,
  DSS_NET_DOWN_REASON_FEATURE_NOT_SUPPORTED =
                                    PS_NET_DOWN_REASON_FEATURE_NOT_SUPPORTED,
  DSS_NET_DOWN_REASON_TFT_SEMANTIC_ERROR =
                                        PS_NET_DOWN_REASON_TFT_SEMANTIC_ERROR,
  DSS_NET_DOWN_REASON_TFT_SYNTAX_ERROR  = PS_NET_DOWN_REASON_TFT_SYNTAX_ERROR,
  DSS_NET_DOWN_REASON_UNKNOWN_PDP_CONTEXT =
                                       PS_NET_DOWN_REASON_UNKNOWN_PDP_CONTEXT,
  DSS_NET_DOWN_REASON_FILTER_SEMANTIC_ERROR =
                                     PS_NET_DOWN_REASON_FILTER_SEMANTIC_ERROR,
  DSS_NET_DOWN_REASON_FILTER_SYNTAX_ERROR =
                                       PS_NET_DOWN_REASON_FILTER_SYNTAX_ERROR,
  DSS_NET_DOWN_REASON_PDP_WITHOUT_ACTIVE_TFT =
                                    PS_NET_DOWN_REASON_PDP_WITHOUT_ACTIVE_TFT,
  DSS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED =
                                        PS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED,
  DSS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED =
                                        PS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED,
  DSS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY = 
                                   PS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY,
  DSS_NET_DOWN_REASON_ESM_INFO_NOT_RECEIVED =
                                     PS_NET_DOWN_REASON_ESM_INFO_NOT_RECEIVED,
  DSS_NET_DOWN_REASON_PDN_CONN_DOES_NOT_EXIST =
                                   PS_NET_DOWN_REASON_PDN_CONN_DOES_NOT_EXIST,
  DSS_NET_DOWN_REASON_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED =
                        PS_NET_DOWN_REASON_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED,
  DSS_NET_DOWN_REASON_COLLISION_WITH_NW_INIT_REQ =
                                PS_NET_DOWN_REASON_COLLISION_WITH_NW_INIT_REQ,
  DSS_NET_DOWN_REASON_UNSUPPORTED_QCI_VALUE =
                                     PS_NET_DOWN_REASON_UNSUPPORTED_QCI_VALUE,
  DSS_NET_DOWN_REASON_BEARER_HANDLING_NOT_SUPPORTED =
                             PS_NET_DOWN_REASON_BEARER_HANDLING_NOT_SUPPORTED,
  DSS_NET_DOWN_REASON_MAX_ACTIVE_PDP_CONTEXT_REACHED = 
                            PS_NET_DOWN_REASON_MAX_ACTIVE_PDP_CONTEXT_REACHED,
  DSS_NET_DOWN_REASON_UNSUPPORTED_APN_IN_CURRENT_PLMN =
                           PS_NET_DOWN_REASON_UNSUPPORTED_APN_IN_CURRENT_PLMN,
  DSS_NET_DOWN_REASON_INVALID_TRANSACTION_ID =
                                    PS_NET_DOWN_REASON_INVALID_TRANSACTION_ID,
  DSS_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC =
                                PS_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC,
  DSS_NET_DOWN_REASON_INVALID_MANDATORY_INFO =
                                    PS_NET_DOWN_REASON_INVALID_MANDATORY_INFO,
  DSS_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED =
                                  PS_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED,
  DSS_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE =
                              PS_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE,
  DSS_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT =
                                      PS_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT,
  DSS_NET_DOWN_REASON_CONDITIONAL_IE_ERROR =
                                      PS_NET_DOWN_REASON_CONDITIONAL_IE_ERROR,
  DSS_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE =
                       PS_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE,
  DSS_NET_DOWN_REASON_PROTOCOL_ERROR      = PS_NET_DOWN_REASON_PROTOCOL_ERROR,
  DSS_NET_DOWN_REASON_APN_TYPE_CONFLICT   =  PS_NET_DOWN_REASON_APN_TYPE_CONFLICT,
  DSS_NET_DOWN_REASON_INVALID_PCSCF_ADDR   =  PS_NET_DOWN_REASON_INVALID_PCSCF_ADDR,
  DSS_NET_DOWN_REASON_INVALID_DNS_ADDR   =  PS_NET_DOWN_REASON_INVALID_DNS_ADDR,
  DSS_NET_DOWN_REASON_INVALID_PCSCF_DNS_ADDR   =  PS_NET_DOWN_REASON_INVALID_PCSCF_DNS_ADDR,
  DSS_NET_DOWN_REASON_EMM_ACCESS_BARRED   =  PS_NET_DOWN_REASON_EMM_ACCESS_BARRED,
  DSS_NET_DOWN_REASON_EMM_ACCESS_BARRED_INFINITE_RETRY   =  
                                PS_NET_DOWN_REASON_EMM_ACCESS_BARRED_INFINITE_RETRY,
  DSS_NET_DOWN_REASON_EMM_ACCESS_BARRED_EAB   =  PS_NET_DOWN_REASON_EMM_ACCESS_BARRED_EAB,
  DSS_NET_DOWN_REASON_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN = 
                      PS_NET_DOWN_REASON_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN,
  DSS_NET_DOWN_REASON_TEST_LOOPBACK_MODE_A_OR_B_ENABLED = 
    PS_NET_DOWN_REASON_TEST_LOOPBACK_MODE_A_OR_B_ENABLED,
  DSS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE  = PS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE,
  DSS_NET_DOWN_REASON_NAS_SIGNALLING_ERROR  = 
                      PS_NET_DOWN_REASON_NAS_SIGNALLING_ERROR,

  DSS_NET_DOWN_REASON_INTERNAL_MIN = PS_NET_DOWN_REASON_INTERNAL_MIN,
  DSS_NET_DOWN_REASON_INTERNAL_ERROR  = PS_NET_DOWN_REASON_INTERNAL_ERROR,
  DSS_NET_DOWN_REASON_INTERNAL_CALL_ENDED =
                                      PS_NET_DOWN_REASON_INTERNAL_CALL_ENDED,
  DSS_NET_DOWN_REASON_INTERNAL_UNKNOWN_CAUSE_CODE =
                              PS_NET_DOWN_REASON_INTERNAL_UNKNOWN_CAUSE_CODE,

  /* Internal mode handler error codes */
  DSS_NET_DOWN_REASON_INTERNAL_MMGSDI_CARD_EVT = PS_NET_DOWN_REASON_INTERNAL_MMGSDI_CARD_EVT,
  DSS_NET_DOWN_REASON_INTERNAL_LPM_OR_PWR_DOWN = PS_NET_DOWN_REASON_INTERNAL_LPM_OR_PWR_DOWN,
  DSS_NET_DOWN_REASON_INTERNAL_APN_DISABLED    = PS_NET_DOWN_REASON_INTERNAL_APN_DISABLED,
  DSS_NET_DOWN_REASON_APN_DISABLED             = DSS_NET_DOWN_REASON_INTERNAL_APN_DISABLED,
  /*Alias for DSS_NET_DOWN_REASON_INTERNAL_APN_DISABLED*/
  DSS_NET_DOWN_REASON_INTERNAL_MPIT_EXPIRED    = PS_NET_DOWN_REASON_INTERNAL_MPIT_EXPIRED,
  DSS_NET_DOWN_REASON_INTERNAL_IPV6_ADDR_TRANSFER_FAILED 
                        = PS_NET_DOWN_REASON_INTERNAL_IPV6_ADDR_TRANSFER_FAILED,
  DSS_NET_DOWN_REASON_INTERNAL_TRAT_SWAP_FAILED 
                        = PS_NET_DOWN_REASON_INTERNAL_TRAT_SWAP_FAILED,
  DSS_NET_DOWN_REASON_INTERNAL_EHRPD_TO_HRPD_FALLBACK 
                        = PS_NET_DOWN_REASON_INTERNAL_EHRPD_TO_HRPD_FALLBACK,
  DSS_NET_DOWN_REASON_MANDATORY_APN_DISABLED
                        = PS_NET_DOWN_REASON_MANDATORY_APN_DISABLED,
  DSS_NET_DOWN_REASON_MIP_CONFIG_FAILURE
                        = PS_NET_DOWN_REASON_MIP_CONFIG_FAILURE,
  DSS_NET_DOWN_REASON_INTERNAL_INACT_TIMER_EXPIRE
                        = PS_NET_DOWN_REASON_INTERNAL_PDN_INACTIVITY_TIMER_EXPIRED,
  DSS_NET_DOWN_REASON_PROFILES_NOT_COMPATIBLE
                        = PS_NET_DOWN_REASON_PROFILES_NOT_COMPATIBLE,
  DSS_NET_DOWN_REASON_NULL_RESOLVED_APN_NO_MATCH
                        = PS_NET_DOWN_REASON_NULL_RESOLVED_APN_NO_MATCH,
  DSS_NET_DOWN_REASON_INVALID_APN_NAME   =  PS_NET_DOWN_REASON_INVALID_APN_NAME,
  DSS_NET_DOWN_REASON_INTERNAL_MAX = PS_NET_DOWN_REASON_INTERNAL_MAX,

  DSS_NET_DOWN_REASON_CDMA_LOCK = PS_NET_DOWN_REASON_CDMA_LOCK,
  DSS_NET_DOWN_REASON_INTERCEPT = PS_NET_DOWN_REASON_INTERCEPT,
  DSS_NET_DOWN_REASON_REORDER = PS_NET_DOWN_REASON_REORDER,
  DSS_NET_DOWN_REASON_REL_SO_REJ= PS_NET_DOWN_REASON_REL_SO_REJ,
  DSS_NET_DOWN_REASON_INCOM_CALL = PS_NET_DOWN_REASON_INCOM_CALL,
  DSS_NET_DOWN_REASON_ALERT_STOP = PS_NET_DOWN_REASON_ALERT_STOP,
  DSS_NET_DOWN_REASON_ACTIVATION = PS_NET_DOWN_REASON_ACTIVATION,
  DSS_NET_DOWN_REASON_MAX_ACCESS_PROBE = PS_NET_DOWN_REASON_MAX_ACCESS_PROBE,
  DSS_NET_DOWN_REASON_CCS_NOT_SUPPORTED_BY_BS =
                                  PS_NET_DOWN_REASON_CCS_NOT_SUPPORTED_BY_BS,
  DSS_NET_DOWN_REASON_NO_RESPONSE_FROM_BS =
                                      PS_NET_DOWN_REASON_NO_RESPONSE_FROM_BS,
  DSS_NET_DOWN_REASON_REJECTED_BY_BS = PS_NET_DOWN_REASON_REJECTED_BY_BS,
  DSS_NET_DOWN_REASON_INCOMPATIBLE = PS_NET_DOWN_REASON_INCOMPATIBLE,
  DSS_NET_DOWN_REASON_ALREADY_IN_TC = PS_NET_DOWN_REASON_ALREADY_IN_TC,
  DSS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_GPS =
                                PS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_GPS,
  DSS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_SMS =
                                PS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_SMS,
  DSS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_VOICE_CALL =
                                PS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_VOICE_CALL,
  DSS_NET_DOWN_REASON_NO_CDMA_SRV      = PS_NET_DOWN_REASON_NO_CDMA_SRV,
  DSS_NET_DOWN_REASON_MC_ABORT         = PS_NET_DOWN_REASON_MC_ABORT,
  DSS_NET_DOWN_REASON_PSIST_NG         = PS_NET_DOWN_REASON_PSIST_NG,
  DSS_NET_DOWN_REASON_UIM_NOT_PRESENT  = PS_NET_DOWN_REASON_UIM_NOT_PRESENT,
  DSS_NET_DOWN_REASON_RETRY_ORDER      = PS_NET_DOWN_REASON_RETRY_ORDER,
  DSS_NET_DOWN_REASON_ACCESS_BLOCK     = PS_NET_DOWN_REASON_ACCESS_BLOCK,
  DSS_NET_DOWN_REASON_ACCESS_BLOCK_ALL = PS_NET_DOWN_REASON_ACCESS_BLOCK_ALL,
  DSS_NET_DOWN_REASON_IS707B_MAX_ACC   = PS_NET_DOWN_REASON_IS707B_MAX_ACC,
  DSS_NET_DOWN_REASON_THERMAL_EMERGENCY = PS_NET_DOWN_REASON_THERMAL_EMERGENCY,
  DSS_NET_DOWN_REASON_CALL_ORIG_THROTTLED   = PS_NET_DOWN_REASON_CALL_ORIG_THROTTLED,

  DSS_NET_DOWN_REASON_CONF_FAILED = PS_NET_DOWN_REASON_CONF_FAILED,
  DSS_NET_DOWN_REASON_INCOM_REJ = PS_NET_DOWN_REASON_INCOM_REJ,
  DSS_NET_DOWN_REASON_NO_GW_SRV = PS_NET_DOWN_REASON_NO_GW_SRV,
  DSS_NET_DOWN_REASON_NO_GPRS_CONTEXT = PS_NET_DOWN_REASON_NO_GPRS_CONTEXT,
  DSS_NET_DOWN_REASON_ILLEGAL_MS = PS_NET_DOWN_REASON_ILLEGAL_MS,
  DSS_NET_DOWN_REASON_ILLEGAL_ME = PS_NET_DOWN_REASON_ILLEGAL_ME,
  DSS_NET_DOWN_REASON_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED =
       PS_NET_DOWN_REASON_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED,
  DSS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED =
                             PS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED,
  DSS_NET_DOWN_REASON_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK =
          PS_NET_DOWN_REASON_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK,
  DSS_NET_DOWN_REASON_IMPLICITLY_DETACHED =
                                    PS_NET_DOWN_REASON_IMPLICITLY_DETACHED,
  DSS_NET_DOWN_REASON_PLMN_NOT_ALLOWED =
                                      PS_NET_DOWN_REASON_PLMN_NOT_ALLOWED,
  DSS_NET_DOWN_REASON_LA_NOT_ALLOWED = PS_NET_DOWN_REASON_LA_NOT_ALLOWED,
  DSS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN =
                PS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN,
  DSS_NET_DOWN_REASON_PDP_DUPLICATE = PS_NET_DOWN_REASON_PDP_DUPLICATE,
  DSS_NET_DOWN_REASON_UE_RAT_CHANGE = PS_NET_DOWN_REASON_UE_RAT_CHANGE,
  DSS_NET_DOWN_REASON_CONGESTION    = PS_NET_DOWN_REASON_CONGESTION,
  DSS_NET_DOWN_REASON_NO_PDP_CONTEXT_ACTIVATED =
                               PS_NET_DOWN_REASON_NO_PDP_CONTEXT_ACTIVATED,
  DSS_NET_DOWN_REASON_ACCESS_CLASS_DSAC_REJECTION  =
                               PS_NET_DOWN_REASON_ACCESS_CLASS_DSAC_REJECTION,
  DSS_NET_DOWN_REASON_PDP_ACTIVATE_MAX_RETRY_FAILED = 
                               PS_NET_DOWN_REASON_PDP_ACTIVATE_MAX_RETRY_FAILED,
  DSS_NET_DOWN_REASON_ESM_PROC_TIME_OUT = 
                               PS_NET_DOWN_REASON_ESM_PROC_TIME_OUT,							   
  DSS_NET_DOWN_REASON_INVALID_CONNECTIN_ID = 
                               PS_NET_DOWN_REASON_INVALID_CONNECTION_ID,
  DSS_NET_DOWN_REASON_INVALID_NSAPI = 
                               PS_NET_DOWN_REASON_INVALID_NSAPI,
  DSS_NET_DOWN_REASON_INVALID_PRI_NSAPI = 
                               PS_NET_DOWN_REASON_INVALID_PRI_NSAPI,
  DSS_NET_DOWN_REASON_INVALID_FIELD = 
                               PS_NET_DOWN_REASON_INVALID_FIELD,
  DSS_NET_DOWN_REASON_RAB_SETUP_FAILURE = 
                               PS_NET_DOWN_REASON_RAB_SETUP_FAILURE,
  DSS_NET_DOWN_REASON_PDP_ESTABLISH_MAX_TIMEOUT = 
                               PS_NET_DOWN_REASON_PDP_ESTABLISH_MAX_TIMEOUT,
  DSS_NET_DOWN_REASON_PDP_MODIFY_MAX_TIMEOUT = 
                               PS_NET_DOWN_REASON_PDP_MODIFY_MAX_TIMEOUT,
  DSS_NET_DOWN_REASON_PDP_INACTIVE_MAX_TIMEOUT = 
                               PS_NET_DOWN_REASON_PDP_INACTIVE_MAX_TIMEOUT,
  DSS_NET_DOWN_REASON_PDP_LOWERLAYER_ERROR = 
                               PS_NET_DOWN_REASON_PDP_LOWERLAYER_ERROR,
  DSS_NET_DOWN_REASON_PPD_UNKNOWN_REASON = 
                               PS_NET_DOWN_REASON_PPD_UNKNOWN_REASON,
  DSS_NET_DOWN_REASON_PDP_MODIFY_COLLISION = 
                               PS_NET_DOWN_REASON_PDP_MODIFY_COLLISION,
  DSS_NET_DOWN_REASON_PDP_MBMS_REQUEST_COLLISION = 
                               PS_NET_DOWN_REASON_PDP_MBMS_REQUEST_COLLISION,
  DSS_NET_DOWN_REASON_MBMS_DUPLICATE = 
                               PS_NET_DOWN_REASON_MBMS_DUPLICATE,
  DSS_NET_DOWN_REASON_SM_PS_DETACHED = 
                               PS_NET_DOWN_REASON_SM_PS_DETACHED,
  DSS_NET_DOWN_REASON_SM_NO_RADIO_AVAILABLE = 
                               PS_NET_DOWN_REASON_SM_NO_RADIO_AVAILABLE,
  DSS_NET_DOWN_REASON_SM_ABORT_SERVICE_NOT_AVAILABLE = 
                               PS_NET_DOWN_REASON_SM_ABORT_SERVICE_NOT_AVAILABLE,
  DSS_NET_DOWN_REASON_SM_NAS_SRV_REQ_FAILURE = 
                               PS_NET_DOWN_REASON_SM_NAS_SRV_REQ_FAILURE,
  DSS_NET_DOWN_REASON_RAB_FAILURE = 
                                PS_NET_DOWN_REASON_RAB_FAILURE,
  DSS_NET_DOWN_REASON_EPS_SERVICES_NOT_ALLOWED = 
                                PS_NET_DOWN_REASON_EPS_SERVICES_NOT_ALLOWED,
  DSS_NET_DOWN_REASON_TRACKING_AREA_NOT_ALLOWED = 
                                PS_NET_DOWN_REASON_TRACKING_AREA_NOT_ALLOWED,
  DSS_NET_DOWN_REASON_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA = 
                  PS_NET_DOWN_REASON_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA,
  DSS_NET_DOWN_REASON_NO_SUITABLE_CELLS_IN_TRACKING_AREA = 
                         PS_NET_DOWN_REASON_NO_SUITABLE_CELLS_IN_TRACKING_AREA,
  DSS_NET_DOWN_REASON_NOT_AUTHORIZED_FOR_THIS_CSG = 
                                PS_NET_DOWN_REASON_NOT_AUTHORIZED_FOR_THIS_CSG,
  DSS_NET_DOWN_REASON_ESM_UNKNOWN_EPS_BEARER_CONTEXT = 
                              PS_NET_DOWN_REASON_ESM_UNKNOWN_EPS_BEARER_CONTEXT,
  DSS_NET_DOWN_REASON_DRB_RELEASED_AT_RRC = 
                                PS_NET_DOWN_REASON_DRB_RELEASED_AT_RRC,
  DSS_NET_DOWN_REASON_NAS_SIG_CONN_RELEASED = 
                                PS_NET_DOWN_REASON_NAS_SIG_CONN_RELEASED,
  DSS_NET_DOWN_REASON_EMM_DETACHED = PS_NET_DOWN_REASON_EMM_DETACHED,
  DSS_NET_DOWN_REASON_EMM_ATTACH_FAILED = PS_NET_DOWN_REASON_EMM_ATTACH_FAILED,
  DSS_NET_DOWN_REASON_EMM_ATTACH_STARTED = PS_NET_DOWN_REASON_EMM_ATTACH_STARTED,
  DSS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED = 
                                   PS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED,
  DSS_NET_DOWN_REASON_ESM_ACTIVE_DEDICATED_BEARER_REACTIVATED_BY_NW = 
                PS_NET_DOWN_REASON_ESM_ACTIVE_DEDICATED_BEARER_REACTIVATED_BY_NW,
  DSS_NET_DOWN_REASON_ESM_LOWER_LAYER_FAILURE = 
                                      PS_NET_DOWN_REASON_ESM_LOWER_LAYER_FAILURE,
  DSS_NET_DOWN_REASON_ESM_SYNC_UP_WITH_NW = 
                                          PS_NET_DOWN_REASON_ESM_SYNC_UP_WITH_NW,
  DSS_NET_DOWN_REASON_ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER = 
            PS_NET_DOWN_REASON_ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER,
  DSS_NET_DOWN_REASON_ESM_BAD_OTA_MESSAGE = 
                                          PS_NET_DOWN_REASON_ESM_BAD_OTA_MESSAGE,
  DSS_NET_DOWN_REASON_ESM_DS_REJECTED_THE_CALL = 
                                     PS_NET_DOWN_REASON_ESM_DS_REJECTED_THE_CALL,
  DSS_NET_DOWN_REASON_ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT = 
                           PS_NET_DOWN_REASON_ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT,
  DSS_NET_DOWN_REASON_DS_EXPLICIT_DEACT = PS_NET_DOWN_REASON_DS_EXPLICIT_DEACT,
  DSS_NET_DOWN_REASON_ESM_LOCAL_CAUSE_NONE = 
                                         PS_NET_DOWN_REASON_ESM_LOCAL_CAUSE_NONE,
  DSS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE = 
                       PS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
  DSS_NET_DOWN_REASON_ACL_FAILURE = PS_NET_DOWN_REASON_ACL_FAILURE,
  DSS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED_DS_DISALLOW = 
                       PS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED_DS_DISALLOW,
  DSS_NET_DOWN_REASON_EMM_T3417_EXPIRED = PS_NET_DOWN_REASON_EMM_T3417_EXPIRED,
  DSS_NET_DOWN_REASON_EMM_T3417_EXT_EXPIRED = 
                                        PS_NET_DOWN_REASON_EMM_T3417_EXT_EXPIRED,
  DSS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_TXN = 
                                 PS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_TXN,
  DSS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_HO = 
                                  PS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_HO,
  DSS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_CONN_REL = 
                            PS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_CONN_REL,
  DSS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_RLF = 
                                 PS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_RLF,
  DSS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN = 
                       PS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN,
  DSS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE = 
                                        PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE,
  DSS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_ABORTED = 
                                PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_ABORTED,
  DSS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_ACCESS_BARRED = 
                          PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_ACCESS_BARRED,
  DSS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CELL_RESEL = 
                             PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CELL_RESEL,
  DSS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE = 
                         PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE,
  DSS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED = 
                          PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED,
  DSS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_LINK_FAILURE = 
                           PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_LINK_FAILURE,
  DSS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_NOT_CAMPED = 
                             PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_NOT_CAMPED,
  DSS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_SI_FAILURE = 
                             PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_SI_FAILURE,
  DSS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CONN_REJECT = 
                            PS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CONN_REJECT,
  DSS_NET_DOWN_REASON_LRRC_CONN_REL_NORMAL = 
                                         PS_NET_DOWN_REASON_LRRC_CONN_REL_NORMAL,
  DSS_NET_DOWN_REASON_LRRC_CONN_REL_RLF = PS_NET_DOWN_REASON_LRRC_CONN_REL_RLF,
  DSS_NET_DOWN_REASON_LRRC_CONN_REL_CRE_FAILURE = 
                                    PS_NET_DOWN_REASON_LRRC_CONN_REL_CRE_FAILURE,
  DSS_NET_DOWN_REASON_LRRC_CONN_REL_OOS_DURING_CRE = 
                                 PS_NET_DOWN_REASON_LRRC_CONN_REL_OOS_DURING_CRE,
  DSS_NET_DOWN_REASON_LRRC_CONN_REL_ABORTED = 
                                        PS_NET_DOWN_REASON_LRRC_CONN_REL_ABORTED,
  DSS_NET_DOWN_REASON_LRRC_CONN_REL_SIB_READ_ERROR = 
                                 PS_NET_DOWN_REASON_LRRC_CONN_REL_SIB_READ_ERROR,
  DSS_NET_DOWN_REASON_DETACH_WITH_REATTACH_LTE_NW_DETACH = 
                           PS_NET_DOWN_REASON_DETACH_WITH_REATTACH_LTE_NW_DETACH,
  DSS_NET_DOWN_REASON_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH = 
                       PS_NET_DOWN_REASON_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH,
  DSS_NET_DOWN_REASON_MESSAGE_EXCEED_MAX_L2_LIMIT =
                       PS_NET_DOWN_REASON_MESSAGE_EXCEED_MAX_L2_LIMIT,

  DSS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_REQ_ERROR = 
                               PS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_REQ_ERROR,
                                                                      
  DSS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_TAI_CHANGE = 
                              PS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_TAI_CHANGE,                   
                                                     
  DSS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE =
                          PS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE,
                                                                      
  DSS_NET_DOWN_REASON_RRC_CONN_REL_ABORTED_IRAT_SUCCESS =
                            PS_NET_DOWN_REASON_RRC_CONN_REL_ABORTED_IRAT_SUCCESS,
 
  DSS_NET_DOWN_REASON_RRC_CONN_REL_RLF_SEC_NOT_ACTIVE =                                                                      
                              PS_NET_DOWN_REASON_RRC_CONN_REL_RLF_SEC_NOT_ACTIVE,                   
 
  DSS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_TO_LTE_ABORTED =                                                                      
                             PS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_TO_LTE_ABORTED,
 
  DSS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_SUCCESS =                                                                      
                  PS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_SUCCESS,       
 
  DSS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_ABORTED =                                                                      
                  PS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_ABORTED,
                                                                      
  DSS_NET_DOWN_REASON_IMSI_UNKNOWN_IN_HSS =
                                          PS_NET_DOWN_REASON_IMSI_UNKNOWN_IN_HSS,                               
                                                                      
  DSS_NET_DOWN_REASON_IMEI_NOT_ACCEPTED =   PS_NET_DOWN_REASON_IMEI_NOT_ACCEPTED,                                 
 
  DSS_NET_DOWN_REASON_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED =                                                               
                PS_NET_DOWN_REASON_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED,     
                                                                  
  DSS_NET_DOWN_REASON_EPS_SERVICES_NOT_ALLOWED_IN_PLMN =
                             PS_NET_DOWN_REASON_EPS_SERVICES_NOT_ALLOWED_IN_PLMN,
                       
  DSS_NET_DOWN_REASON_MSC_TEMPORARILY_NOT_REACHABLE =
                                PS_NET_DOWN_REASON_MSC_TEMPORARILY_NOT_REACHABLE,
                                                  
  DSS_NET_DOWN_REASON_CS_DOMAIN_NOT_AVAILABLE =
                                      PS_NET_DOWN_REASON_CS_DOMAIN_NOT_AVAILABLE,                           
                                            
  DSS_NET_DOWN_REASON_ESM_FAILURE =               PS_NET_DOWN_REASON_ESM_FAILURE,                                       
                                                       
  DSS_NET_DOWN_REASON_MAC_FAILURE =               PS_NET_DOWN_REASON_MAC_FAILURE,
                     
  DSS_NET_DOWN_REASON_SYNCH_FAILURE =           PS_NET_DOWN_REASON_SYNCH_FAILURE,
                                             
  DSS_NET_DOWN_REASON_UE_SECURITY_CAPABILITIES_MISMATCH =                                              
                            PS_NET_DOWN_REASON_UE_SECURITY_CAPABILITIES_MISMATCH,
                           
  DSS_NET_DOWN_REASON_SECURITY_MODE_REJ_UNSPECIFIED =
                                PS_NET_DOWN_REASON_SECURITY_MODE_REJ_UNSPECIFIED,                     
                          
  DSS_NET_DOWN_REASON_NON_EPS_AUTH_UNACCEPTABLE = 
                                    PS_NET_DOWN_REASON_NON_EPS_AUTH_UNACCEPTABLE,
                          
  DSS_NET_DOWN_REASON_CS_FALLBACK_CALL_EST_NOT_ALLOWED = 
                             PS_NET_DOWN_REASON_CS_FALLBACK_CALL_EST_NOT_ALLOWED,                     
                               
  DSS_NET_DOWN_REASON_NO_EPS_BEARER_CONTEXT_ACTIVATED =
                              PS_NET_DOWN_REASON_NO_EPS_BEARER_CONTEXT_ACTIVATED,         
  
  DSS_NET_DOWN_REASON_EMM_INVALID_STATE =   PS_NET_DOWN_REASON_EMM_INVALID_STATE,

  DSS_NET_DOWN_REASON_NAS_LAYER_FAILURE = 
                                            PS_NET_DOWN_REASON_NAS_LAYER_FAILURE,

  DSS_NET_DOWN_REASON_DETACH_EPS_SERVICES_NOT_ALLOWED = 
                              PS_NET_DOWN_REASON_DETACH_EPS_SERVICES_NOT_ALLOWED,

  DSS_NET_DOWN_REASON_SM_INTERNAL_PDP_DEACTIVATION = 
                              PS_NET_DOWN_REASON_SM_INTERNAL_PDP_DEACTIVATION,

  DSS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING = 
                                  PS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING,

  DSS_NET_DOWN_REASON_CD_GEN_OR_BUSY = PS_NET_DOWN_REASON_CD_GEN_OR_BUSY,
  DSS_NET_DOWN_REASON_CD_BILL_OR_AUTH = PS_NET_DOWN_REASON_CD_BILL_OR_AUTH,
  DSS_NET_DOWN_REASON_CHG_HDR = PS_NET_DOWN_REASON_CHG_HDR,
  DSS_NET_DOWN_REASON_EXIT_HDR = PS_NET_DOWN_REASON_EXIT_HDR,
  DSS_NET_DOWN_REASON_HDR_NO_SESSION = PS_NET_DOWN_REASON_HDR_NO_SESSION,
  DSS_NET_DOWN_REASON_HDR_ORIG_DURING_GPS_FIX =
                                  PS_NET_DOWN_REASON_HDR_ORIG_DURING_GPS_FIX,
  DSS_NET_DOWN_REASON_HDR_CS_TIMEOUT = PS_NET_DOWN_REASON_HDR_CS_TIMEOUT,
  DSS_NET_DOWN_REASON_HDR_RELEASED_BY_CM =
                                       PS_NET_DOWN_REASON_HDR_RELEASED_BY_CM,
  DSS_NET_DOWN_REASON_COLLOC_ACQ_FAIL      = PS_NET_DOWN_REASON_COLLOC_ACQ_FAIL,
  DSS_NET_DOWN_REASON_OTASP_COMMIT_IN_PROG = PS_NET_DOWN_REASON_OTASP_COMMIT_IN_PROG,
  DSS_NET_DOWN_REASON_NO_HYBR_HDR_SRV      = PS_NET_DOWN_REASON_NO_HYBR_HDR_SRV,
  DSS_NET_DOWN_REASON_HDR_NO_LOCK_GRANTED  = PS_NET_DOWN_REASON_HDR_NO_LOCK_GRANTED,
  DSS_NET_DOWN_REASON_HOLD_OTHER_IN_PROG   = PS_NET_DOWN_REASON_HOLD_OTHER_IN_PROG,
  DSS_NET_DOWN_REASON_HDR_FADE             = PS_NET_DOWN_REASON_HDR_FADE,
  DSS_NET_DOWN_REASON_HDR_ACC_FAIL         = PS_NET_DOWN_REASON_HDR_ACC_FAIL,
  DSS_NET_DOWN_REASON_CLIENT_END = PS_NET_DOWN_REASON_CLIENT_END,
  DSS_NET_DOWN_REASON_NO_SRV = PS_NET_DOWN_REASON_NO_SRV,
  DSS_NET_DOWN_REASON_FADE = PS_NET_DOWN_REASON_FADE,
  DSS_NET_DOWN_REASON_REL_NORMAL = PS_NET_DOWN_REASON_REL_NORMAL,
  DSS_NET_DOWN_REASON_ACC_IN_PROG = PS_NET_DOWN_REASON_ACC_IN_PROG,
  DSS_NET_DOWN_REASON_ACC_FAIL = PS_NET_DOWN_REASON_ACC_FAIL,
  DSS_NET_DOWN_REASON_REDIR_OR_HANDOFF = PS_NET_DOWN_REASON_REDIR_OR_HANDOFF,
  DSS_NET_DOWN_REASON_OFFLINE               = PS_NET_DOWN_REASON_OFFLINE,
  DSS_NET_DOWN_REASON_EMERGENCY_MODE        = PS_NET_DOWN_REASON_EMERGENCY_MODE,
  DSS_NET_DOWN_REASON_PHONE_IN_USE          = PS_NET_DOWN_REASON_PHONE_IN_USE,
  DSS_NET_DOWN_REASON_INVALID_MODE          = PS_NET_DOWN_REASON_INVALID_MODE,
  DSS_NET_DOWN_REASON_INVALID_SIM_STATE     = PS_NET_DOWN_REASON_INVALID_SIM_STATE,
  DSS_NET_DOWN_REASON_NO_COLLOC_HDR         = PS_NET_DOWN_REASON_NO_COLLOC_HDR,
  DSS_NET_DOWN_REASON_CALL_CONTROL_REJECTED = PS_NET_DOWN_REASON_CALL_CONTROL_REJECTED,
  DSS_NET_DOWN_REASON_MODEM_RESTART         = PS_NET_DOWN_REASON_MODEM_RESTART,
  DSS_NET_DOWN_REASON_UNPREFERRED_RAT       = PS_NET_DOWN_REASON_UNPREFERRED_RAT,
  DSS_NET_DOWN_REASON_PHYS_LINK_CLOSE_IN_PROGRESS = 
    PS_NET_DOWN_REASON_PHYS_LINK_CLOSE_IN_PROGRESS,
  DSS_NET_DOWN_REASON_APN_PENDING_HANDOVER   = 
    PS_NET_DOWN_REASON_APN_PENDING_HANDOVER,
  DSS_NET_DOWN_REASON_PROFILE_BEARER_INCOMPATIBLE =
    PS_NET_DOWN_REASON_PROFILE_BEARER_INCOMPATIBLE,
  DSS_NET_DOWN_REASON_MAX_V4_CONNECTIONS = PS_NET_DOWN_REASON_MAX_V4_CONNECTIONS,
  DSS_NET_DOWN_REASON_MAX_V6_CONNECTIONS = PS_NET_DOWN_REASON_MAX_V6_CONNECTIONS,
  DSS_NET_DOWN_REASON_APN_MISMATCH = PS_NET_DOWN_REASON_APN_MISMATCH,
  DSS_NET_DOWN_REASON_IP_VERSION_MISMATCH = PS_NET_DOWN_REASON_IP_VERSION_MISMATCH,
  DSS_NET_DOWN_REASON_DUN_CALL_DISALLOWED = PS_NET_DOWN_REASON_DUN_CALL_DISALLOWED,
  DSS_NET_DOWN_REASON_INVALID_PROFILE = PS_NET_DOWN_REASON_INVALID_PROFILE,
  DSS_NET_DOWN_REASON_INTERNAL_EPC_NONEPC_TRANSITION = 
                                 PS_NET_DOWN_REASON_INTERNAL_EPC_NONEPC_TRANSITION,
  DSS_NET_DOWN_REASON_INVALID_PROFILE_ID = PS_NET_DOWN_REASON_INVALID_PROFILE_ID,
  DSS_NET_DOWN_REASON_SUBS_ID_MISMATCH = PS_NET_DOWN_REASON_SUBS_ID_MISMATCH,
  DSS_NET_DOWN_REASON_APN_PRM_CHG  = PS_NET_DOWN_REASON_APN_PRM_CHG,
  DSS_NET_DOWN_REASON_NULL_APN_DISALLOWED = PS_NET_DOWN_REASON_NULL_APN_DISALLOWED,
  DSS_NET_DOWN_REASON_THERMAL_MITIGATION = PS_NET_DOWN_REASON_THERMAL_MITIGATION,
  DSS_NET_DOWN_REASON_DATA_SETTINGS_DISABLED = PS_NET_DOWN_REASON_DATA_SETTINGS_DISABLED,
  DSS_NET_DOWN_REASON_DATA_ROAMING_SETTINGS_DISABLED = PS_NET_DOWN_REASON_DATA_ROAMING_SETTINGS_DISABLED,
  DSS_NET_DOWN_REASON_VALIDATION_FAILURE = PS_NET_DOWN_REASON_VALIDATION_FAILURE,
  /* EAP Error codes. */
  DSS_NET_DOWN_REASON_EAP_MIN =
    PS_NET_DOWN_REASON_EAP_MIN,
  DSS_NET_DOWN_REASON_EAP_CLIENT_ERR_UNABLE_TO_PROCESS =
    PS_NET_DOWN_REASON_EAP_CLIENT_ERR_UNABLE_TO_PROCESS,
  DSS_NET_DOWN_REASON_EAP_CLIENT_ERR_UNSUPPORTED_VERS =
    PS_NET_DOWN_REASON_EAP_CLIENT_ERR_UNSUPPORTED_VERS,
  DSS_NET_DOWN_REASON_EAP_CLIENT_ERR_INSUFFICIENT_CHALLANGES =
    PS_NET_DOWN_REASON_EAP_CLIENT_ERR_INSUFFICIENT_CHALLANGES,
  DSS_NET_DOWN_REASON_EAP_CLIENT_ERR_RAND_NOT_FRESH =
    PS_NET_DOWN_REASON_EAP_CLIENT_ERR_RAND_NOT_FRESH,
  DSS_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_AFTER_AUTH =
    PS_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_AFTER_AUTH,
  DSS_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_BEFORE_AUTH =
    PS_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_BEFORE_AUTH,
  DSS_NET_DOWN_REASON_EAP_NOTIFICATION_TEMP_DENIED_ACCESS =
    PS_NET_DOWN_REASON_EAP_NOTIFICATION_TEMP_DENIED_ACCESS,
  DSS_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NOT_SUBSCRIBED =
    PS_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NOT_SUBSCRIBED,
  DSS_NET_DOWN_REASON_EAP_SUCCESS =
    PS_NET_DOWN_REASON_EAP_SUCCESS,
  DSS_NET_DOWN_REASON_EAP_NOTIFICATION_REALM_UNAVAILABLE =
    PS_NET_DOWN_REASON_EAP_NOTIFICATION_REALM_UNAVAILABLE,
  DSS_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NAME_UNAVAILABLE =
    PS_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NAME_UNAVAILABLE,
  DSS_NET_DOWN_REASON_EAP_NOTIFICATION_CALL_BARRED =
    PS_NET_DOWN_REASON_EAP_NOTIFICATION_CALL_BARRED,
  DSS_NET_DOWN_REASON_EAP_MAX =
    PS_NET_DOWN_REASON_EAP_MAX,

  /* IPSEC Error Codes. */
  DSS_NET_DOWN_REASON_IPSEC_MIN =
    PS_NET_DOWN_REASON_IPSEC_MIN,
  DSS_NET_DOWN_REASON_IPSEC_GW_UNREACHABLE =
    PS_NET_DOWN_REASON_IPSEC_GW_UNREACHABLE,
  DSS_NET_DOWN_REASON_IPSEC_AUTH_FAILED =
    PS_NET_DOWN_REASON_IPSEC_AUTH_FAILED,
  DSS_NET_DOWN_REASON_IPSEC_CERT_INVALID =
    PS_NET_DOWN_REASON_IPSEC_CERT_INVALID,
  DSS_NET_DOWN_REASON_IPSEC_INTERNAL_ERROR =
    PS_NET_DOWN_REASON_IPSEC_INTERNAL_ERROR,
  DSS_NET_DOWN_REASON_IPSEC_MAX =
    PS_NET_DOWN_REASON_IPSEC_MAX,

  /* MIP Error codes. */
  DSS_NET_DOWN_REASON_MIP_FA_ERR_REASON_UNSPECIFIED =
    PS_NET_DOWN_REASON_MIP_FA_ERR_REASON_UNSPECIFIED,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_ADMINISTRATIVELY_PROHIBITED =
    PS_NET_DOWN_REASON_MIP_FA_ERR_ADMINISTRATIVELY_PROHIBITED,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_INSUFFICIENT_RESOURCES =
    PS_NET_DOWN_REASON_MIP_FA_ERR_INSUFFICIENT_RESOURCES,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_MOBILE_NODE_AUTHENTICATION_FAILURE =
    PS_NET_DOWN_REASON_MIP_FA_ERR_MOBILE_NODE_AUTHENTICATION_FAILURE,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_HA_AUTHENTICATION_FAILURE =
    PS_NET_DOWN_REASON_MIP_FA_ERR_HA_AUTHENTICATION_FAILURE,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_REQUESTED_LIFETIME_TOO_LONG =
    PS_NET_DOWN_REASON_MIP_FA_ERR_REQUESTED_LIFETIME_TOO_LONG,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_MALFORMED_REQUEST =
    PS_NET_DOWN_REASON_MIP_FA_ERR_MALFORMED_REQUEST,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_MALFORMED_REPLY =
    PS_NET_DOWN_REASON_MIP_FA_ERR_MALFORMED_REPLY,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_ENCAPSULATION_UNAVAILABLE =
    PS_NET_DOWN_REASON_MIP_FA_ERR_ENCAPSULATION_UNAVAILABLE,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_VJHC_UNAVAILABLE =
    PS_NET_DOWN_REASON_MIP_FA_ERR_VJHC_UNAVAILABLE,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_UNKNOWN_CHALLENGE =
    PS_NET_DOWN_REASON_MIP_FA_ERR_UNKNOWN_CHALLENGE,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_MISSING_CHALLENGE =
    PS_NET_DOWN_REASON_MIP_FA_ERR_MISSING_CHALLENGE,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_STALE_CHALLENGE =
    PS_NET_DOWN_REASON_MIP_FA_ERR_STALE_CHALLENGE,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_REVERSE_TUNNEL_UNAVAILABLE =
    PS_NET_DOWN_REASON_MIP_FA_ERR_REVERSE_TUNNEL_UNAVAILABLE,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_REVERSE_TUNNEL_IS_MANDATORY_AND_T_BIT_NOT_SET =
    PS_NET_DOWN_REASON_MIP_FA_ERR_REVERSE_TUNNEL_IS_MANDATORY_AND_T_BIT_NOT_SET,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_DELIVERY_STYLE_NOT_SUPPORTED =
    PS_NET_DOWN_REASON_MIP_FA_ERR_DELIVERY_STYLE_NOT_SUPPORTED,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_MISSING_NAI =
    PS_NET_DOWN_REASON_MIP_FA_ERR_MISSING_NAI,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_MISSING_HA =
    PS_NET_DOWN_REASON_MIP_FA_ERR_MISSING_HA,
  DSS_NET_DOWN_REASON_MIP_FA_ERR_MISSING_HOME_ADDR =
    PS_NET_DOWN_REASON_MIP_FA_ERR_MISSING_HOME_ADDR,
  DSS_NET_DOWN_REASON_MIP_HA_ERR_REASON_UNSPECIFIED =
    PS_NET_DOWN_REASON_MIP_HA_ERR_REASON_UNSPECIFIED,
  DSS_NET_DOWN_REASON_MIP_HA_ERR_ADMINISTRATIVELY_PROHIBITED =
    PS_NET_DOWN_REASON_MIP_HA_ERR_ADMINISTRATIVELY_PROHIBITED,
  DSS_NET_DOWN_REASON_MIP_HA_ERR_INSUFFICIENT_RESOURCES =
    PS_NET_DOWN_REASON_MIP_HA_ERR_INSUFFICIENT_RESOURCES,
  DSS_NET_DOWN_REASON_MIP_HA_ERR_MOBILE_NODE_AUTHENTICATION_FAILURE =
    PS_NET_DOWN_REASON_MIP_HA_ERR_MOBILE_NODE_AUTHENTICATION_FAILURE,
  DSS_NET_DOWN_REASON_MIP_HA_ERR_FA_AUTHENTICATION_FAILURE =
    PS_NET_DOWN_REASON_MIP_HA_ERR_FA_AUTHENTICATION_FAILURE,
  DSS_NET_DOWN_REASON_MIP_HA_ERR_REGISTRATION_ID_MISMATCH =
    PS_NET_DOWN_REASON_MIP_HA_ERR_REGISTRATION_ID_MISMATCH,
  DSS_NET_DOWN_REASON_MIP_HA_ERR_MALFORMED_REQUEST =
    PS_NET_DOWN_REASON_MIP_HA_ERR_MALFORMED_REQUEST,
  DSS_NET_DOWN_REASON_MIP_HA_ERR_UNKNOWN_HA_ADDR =
    PS_NET_DOWN_REASON_MIP_HA_ERR_UNKNOWN_HA_ADDR,
  DSS_NET_DOWN_REASON_MIP_HA_ERR_REVERSE_TUNNEL_UNAVAILABLE =
    PS_NET_DOWN_REASON_MIP_HA_ERR_REVERSE_TUNNEL_UNAVAILABLE,
  DSS_NET_DOWN_REASON_MIP_HA_ERR_REVERSE_TUNNEL_IS_MANDATORY_AND_T_BIT_NOT_SET =
    PS_NET_DOWN_REASON_MIP_HA_ERR_REVERSE_TUNNEL_IS_MANDATORY_AND_T_BIT_NOT_SET,
  DSS_NET_DOWN_REASON_MIP_HA_ERR_ENCAPSULATION_UNAVAILABLE =
    PS_NET_DOWN_REASON_MIP_HA_ERR_ENCAPSULATION_UNAVAILABLE,
  DSS_NET_DOWN_REASON_MIP_ERR_REASON_UNKNOWN =
    PS_NET_DOWN_REASON_MIP_ERR_REASON_UNKNOWN,

  /* PPP error codes */
  DSS_NET_DOWN_REASON_PPP_ERR_TIMEOUT = PS_NET_DOWN_REASON_PPP_ERR_TIMEOUT,
  DSS_NET_DOWN_REASON_PPP_ERR_AUTH_FAILURE = 
    PS_NET_DOWN_REASON_PPP_ERR_AUTH_FAILURE, 
  DSS_NET_DOWN_REASON_PPP_ERR_OPTION_MISMATCH = 
    PS_NET_DOWN_REASON_PPP_ERR_OPTION_MISMATCH, 
  DSS_NET_DOWN_REASON_PPP_ERR_REASON_UNKNOWN = 
    PS_NET_DOWN_REASON_PPP_ERR_REASON_UNKNOWN,
  DSS_NET_DOWN_REASON_PPP_ERR_PAP_FAILURE = 
    PS_NET_DOWN_REASON_PPP_ERR_PAP_FAILURE, 
  DSS_NET_DOWN_REASON_PPP_ERR_CHAP_FAILURE = 
    PS_NET_DOWN_REASON_PPP_ERR_CHAP_FAILURE,
  DSS_NET_DOWN_REASON_PPP_ERR_CLOSE_IN_PROGRESS =
    PS_NET_DOWN_REASON_PPP_ERR_CLOSE_IN_PROGRESS,

  /* eHRPD error codes */
  DSS_NET_DOWN_REASON_EHRPD_ERR_SUBS_LIMITED_TO_V4 = 
                                PS_NET_DOWN_REASON_EHRPD_ERR_SUBS_LIMITED_TO_V4,
  DSS_NET_DOWN_REASON_EHRPD_ERR_SUBS_LIMITED_TO_V6 =
                                PS_NET_DOWN_REASON_EHRPD_ERR_SUBS_LIMITED_TO_V6,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_TIMEOUT = 
                                     PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_TIMEOUT,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_FAILURE = 
                                     PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_FAILURE,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_CONFIG_FAILURE =
                                    DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_FAILURE,
  /* Alias for DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_FAILURE*/
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_GEN_ERROR =
                            PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_GEN_ERROR,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_UNAUTH_APN = 
                           PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_UNAUTH_APN,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_LIMIT_EXCEED = 
                     PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_LIMIT_EXCEED,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_NO_PDN_GW = 
                            PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_NO_PDN_GW,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_GW_UNREACH = 
                       PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_GW_UNREACH,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_GW_REJ = 
                           PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_GW_REJ,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_INSUFF_PARAM = 
                         PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_INSUFF_PARAM,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_RESOURCE_UNAVAIL = 
                     PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_RESOURCE_UNAVAIL,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_ADMIN_PROHIBIT = 
                       PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_ADMIN_PROHIBIT,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_ID_IN_USE = 
                        PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_ID_IN_USE,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_SUBSCR_LIMITATION =
                    PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_SUBSCR_LIMITATION,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_EXISTS_FOR_THIS_APN = 
              PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_PDN_EXISTS_FOR_THIS_APN,
  DSS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED = 
                PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED,
  DSS_NET_DOWN_REASON_EHRPD_ERR_MAX = 
                            PS_NET_DOWN_REASON_EHRPD_ERR_MAX,

  /* IPv6 error codes */
  DSS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE = 
                                 PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE,
  DSS_NET_DOWN_REASON_IPV6_ERR_HRPD_IPV6_DISABLED = 
                                 PS_NET_DOWN_REASON_IPV6_ERR_HRPD_IPV6_DISABLED,
  DSS_NET_DOWN_REASON_IPV6_DISABLED =
                                 PS_NET_DOWN_REASON_IPV6_DISABLED,
  DSS_NET_DOWN_REASON_IPV6_ERR_MAX = PS_NET_DOWN_REASON_IPV6_ERR_MAX,

  /* Tunnel mode error codes */
  DSS_NET_DOWN_REASON_TUN_CONN_DENIED = 
                                 PS_NET_DOWN_REASON_TUN_CONN_DENIED,
  DSS_NET_DOWN_REASON_TUN_CONN_CR_ROR_FAILED = 
                                 PS_NET_DOWN_REASON_TUN_CONN_CR_ROR_FAILED,
  DSS_NET_DOWN_REASON_TUN_CONN_SAP_L2_FAILED = 
                                 PS_NET_DOWN_REASON_TUN_CONN_SAP_L2_FAILED,
  DSS_NET_DOWN_REASON_TUN_LCP_THROTTLED = 
                                 PS_NET_DOWN_REASON_TUN_LCP_THROTTLED,
  DSS_NET_DOWN_REASON_TUN_VIRTUAL_CONN_THROTTLED = 
                                 PS_NET_DOWN_REASON_TUN_VIRTUAL_CONN_THROTTLED,
  DSS_NET_DOWN_REASON_TUN_NON_EHPRD_MODE = 
                                 PS_NET_DOWN_REASON_TUN_NON_EHPRD_MODE,
  DSS_NET_DOWN_REASON_TUN_NON_TUNNEL_MODE = 
                                 PS_NET_DOWN_REASON_TUN_NON_TUNNEL_MODE,
  DSS_NET_DOWN_REASON_TUN_PREREG_DISALLOWED = 
                                 PS_NET_DOWN_REASON_TUN_PREREG_DISALLOWED,
  DSS_NET_DOWN_REASON_TUN_CONN_SAP_L3_FAILED = 
                                 PS_NET_DOWN_REASON_TUN_CONN_SAP_L3_FAILED,
  DSS_NET_DOWN_REASON_TUN_CONN_SAP_TIMEOUT = 
                                 PS_NET_DOWN_REASON_TUN_CONN_SAP_TIMEOUT,
  DSS_NET_DOWN_REASON_TUN_CONN_SAP_NO_SESSION = 
                                 PS_NET_DOWN_REASON_TUN_CONN_SAP_NO_SESSION,
  DSS_NET_DOWN_REASON_TUN_CONN_SAP_PREREG_NOT_ALLOW = 
                                 PS_NET_DOWN_REASON_TUN_CONN_SAP_PREREG_NOT_ALLOW,
  DSS_NET_DOWN_REASON_TUN_CONN_SAP_ACTIVE_HO = 
                                 PS_NET_DOWN_REASON_TUN_CONN_SAP_ACTIVE_HO,
  DSS_NET_DOWN_REASON_TUN_CONN_SAP_NO_SRV = 
                                 PS_NET_DOWN_REASON_TUN_CONN_SAP_NO_SRV,
  DSS_NET_DOWN_REASON_TUN_NO_LTE_SRV   = 
                                 PS_NET_DOWN_REASON_TUN_NO_LTE_SRV,
  DSS_NET_DOWN_REASON_TUN_HDR_SESSION_CLOSED  = 
                                 PS_NET_DOWN_REASON_TUN_HDR_SESSION_CLOSED,

  /* IWLAN error codes */
  DSS_NET_DOWN_REASON_IWLAN_S2B_PDN_CONN_REJ = 
    PS_NET_DOWN_REASON_IWLAN_S2B_PDN_CONN_REJ,
  DSS_NET_DOWN_REASON_IWLAN_S2B_PDN_CONN_REJ_WITH_IP_ADDR_INFO = 
    PS_NET_DOWN_REASON_IWLAN_S2B_PDN_CONN_REJ_WITH_IP_ADDR_INFO,
  DSS_NET_DOWN_REASON_IWLAN_S2B_MAX_CONN_REACHED = 
    PS_NET_DOWN_REASON_IWLAN_S2B_MAX_CONN_REACHED,
  DSS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_CONFIG_FAILURE = 
    PS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_CONFIG_FAILURE,
  DSS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_AUTH_FAILURE = 
    PS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_AUTH_FAILURE,
  DSS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_MSG_TIMEOUT = 
    PS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_MSG_TIMEOUT,
  DSS_NET_DOWN_REASON_IWLAN_S2B_DNS_RESOLUTION_CONFIG_FAILURE = 
    PS_NET_DOWN_REASON_IWLAN_S2B_DNS_RESOLUTION_CONFIG_FAILURE,
  DSS_NET_DOWN_REASON_IWLAN_S2B_DNS_RESOLUTION_NAME_FAILURE = 
    PS_NET_DOWN_REASON_IWLAN_S2B_DNS_RESOLUTION_NAME_FAILURE,
  DSS_NET_DOWN_REASON_IWLAN_S2B_DNS_RESOLUTION_TIMEOUT = 
    PS_NET_DOWN_REASON_IWLAN_S2B_DNS_RESOLUTION_TIMEOUT,
  DSS_NET_DOWN_REASON_IWLAN_S2B_DNS_RESOLUTION_FAILURE = 
    PS_NET_DOWN_REASON_IWLAN_S2B_DNS_RESOLUTION_FAILURE,
  DSS_NET_DOWN_REASON_IWLAN_S2B_BLOCKING_FAILURE = 
    PS_NET_DOWN_REASON_IWLAN_S2B_BLOCKING_FAILURE,    
  DSS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_PROTOCOL_FAILURE = 
    PS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_PROTOCOL_FAILURE,
  DSS_NET_DOWN_REASON_IWLAN_S2B_NW_INIT_CONN_DOWN =
    PS_NET_DOWN_REASON_IWLAN_S2B_NW_INIT_CONN_DOWN,
  DSS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_DPD_FAILURE =
    PS_NET_DOWN_REASON_IWLAN_S2B_IKEV2_DPD_FAILURE,    
  DSS_NET_DOWN_REASON_IWLAN_S2B_IKE_REKEY_FAILURE =
    PS_NET_DOWN_REASON_IWLAN_S2B_IKE_REKEY_FAILURE,
  DSS_NET_DOWN_REASON_IWLAN_S2B_IPSEC_REKEY_FAILURE =
    PS_NET_DOWN_REASON_IWLAN_S2B_IPSEC_REKEY_FAILURE,
  DSS_NET_DOWN_REASON_IWLAN_S2B_NETWORK_FAILURE = 
    PS_NET_DOWN_REASON_IWLAN_S2B_NETWORK_FAILURE,
  DSS_NET_DOWN_REASON_IWLAN_S2B_ROAMING_NOT_ALLOWED =
    PS_NET_DOWN_REASON_IWLAN_S2B_ROAMING_NOT_ALLOWED,
  DSS_NET_DOWN_REASON_IWLAN_S2B_RAT_DISALLOWED = 
    PS_NET_DOWN_REASON_IWLAN_S2B_RAT_DISALLOWED,
  DSS_NET_DOWN_REASON_IWLAN_S2B_NO_SUBSCRIPTION = 
    PS_NET_DOWN_REASON_IWLAN_S2B_NO_SUBSCRIPTION,
  DSS_NET_DOWN_REASON_IWLAN_S2B_NETWORK_TOO_BUSY = 
    PS_NET_DOWN_REASON_IWLAN_S2B_NETWORK_TOO_BUSY,

  /*Handoff reasons- Type 12*/
  DSS_NET_DOWN_REASON_HANDOFF_PREF_SYS_BACK_TO_SRAT = 
    PS_NET_DOWN_REASON_HANDOFF_PREF_SYS_BACK_TO_SRAT,

  DSS_NET_DOWN_REASON_MAX = PS_NET_DOWN_REASON_MAX
} dss_net_down_reason_type;


/*===========================================================================

                          PUBLIC MACRO DECLARATIONS

===========================================================================*/
/*===========================================================================

MACRO DSS_CLOSE_NETLIB2()

DESCRIPTION
  This macro makes a dss_close_netlib2() call available corresponding to
  dss_open_netlib2() for the sake of uniformity. Since dss_close_netlib2()
  is no different from dss_close_netlib(), it simply calls the latter with
  the passed parameters.

DEPENDENCIES
  None.

RETURN VALUE
  Same as dss_close_netlib()

SIDE EFFECTS
===========================================================================*/
#define dss_close_netlib2(dss_nethandle, dss_errno) dss_close_netlib(dss_nethandle, dss_errno)

/*===========================================================================

MACRO DSNET_GET_HANDLE()

DESCRIPTION
    Macro to change to the new DSNET naming scheme. This macro maps to
    dss_open_netlib2().

DEPENDENCIES
  None.

RETURN VALUE
  Same as dss_open_netlib2()

SIDE EFFECTS
===========================================================================*/
#define dsnet_get_handle(net_cb, net_cb_user_data, sock_cb, \
                         sock_cb_user_data, policy_info_ptr, dss_errno) \
        dss_open_netlib2(net_cb, net_cb_user_data, sock_cb, \
                         sock_cb_user_data, policy_info_ptr, dss_errno)


/*===========================================================================

MACRO DSNET_RELEASE_HANDLE()

DESCRIPTION
    Macro to change to the new DSNET naming scheme. This macro maps to
    dss_close_netlib().

DEPENDENCIES
  None.

RETURN VALUE
  Same as dss_close_netlib()

SIDE EFFECTS
===========================================================================*/
#define dsnet_release_handle(dss_nethandle, dss_errno) dss_close_netlib(dss_nethandle, dss_errno)

/*===========================================================================

MACRO DSNET_START()

DESCRIPTION
  Macro to change to the new DSNET naming scheme. This macro maps to
  dss_pppopen().

DEPENDENCIES
  None.

RETURN VALUE
  Same as dss_pppopen()

SIDE EFFECTS
===========================================================================*/
#define dsnet_start(dss_nethandle, dss_errno) dss_pppopen(dss_nethandle, dss_errno)

/*===========================================================================

MACRO DSNET_STOP()

DESCRIPTION
  Macro to change to the new DSNET naming scheme. This macro maps to
  dss_pppclose().

DEPENDENCIES
  None.

RETURN VALUE
  Same as dss_pppclose()

SIDE EFFECTS
===========================================================================*/
#define dsnet_stop(dss_nethandle, dss_errno) dss_pppclose(dss_nethandle, dss_errno)

/*===========================================================================

MACRO DSNET_GET_POLICY()

DESCRIPTION
  Macro to change to the new DSNET naming scheme. This macro maps to
  dss_get_app_net_policy().

DEPENDENCIES
  None.

RETURN VALUE
  Same as dss_get_app_net_policy()

SIDE EFFECTS
===========================================================================*/
#define dsnet_get_policy(dss_nethandle, policy_info_ptr, dss_errno) \
        dss_get_app_net_policy(dss_nethandle, policy_info_ptr, dss_errno)

/*===========================================================================

MACRO DSNET_SET_POLICY()

DESCRIPTION
  Macro to change to the new DSNET naming scheme. This macro maps to
  dss_set_app_net_policy().

DEPENDENCIES
  None.

RETURN VALUE
  Same as dss_set_app_net_policy()

SIDE EFFECTS
===========================================================================*/
#define dsnet_set_policy(dss_nethandle, policy_info_ptr, dss_errno) \
        dss_set_app_net_policy(dss_nethandle, policy_info_ptr, dss_errno)


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION DSS_SOCKET()

DESCRIPTION
 Create a socket and related data structures, and return a socket descriptor.

 The mapping to actual protocols is as follows:

 ADDRESS FAMILY         Stream          Datagram

 DSS_AF_INET                TCP             UDP

  Note this function must be called to obtain a valid socket descriptor, for
  use with all other socket-related functions.  Thus, before any socket
  functions can be used (e.g. I/O, asynchronous notification, etc.), this
  call must have successfully returned a valid socket descriptor.  The
  application must also have made a call to dss_open_netlib() to obtain
  a valid application ID, and to put the Data Services task into "sockets"
  mode.

  Note:  This implementation version has no support for Raw IP sockets, and
         will return an error, if the application attempts to create one.

         Sockets created using this call are bound to the dss_nethandle used in
         creating this socket.

DEPENDENCIES
  The function dss_open_netlib() must be called to open the network library
  and put the DS/PS managers into sockets mode.

RETURN VALUE
  On successful creation of a socket, this function returns socket file
  descriptor which is a sint15 value between 0x1000 and 0x1FFF.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EAFNOSUPPORT     address family not supported
  DS_EBADAPP          invalid application ID
  DS_EPROTOTYPE       specified protocol invalid for socket type
  DS_ESOCKNOSUPPORT   invalid or unsupported socket parameter specified
  DS_EPROTONOSUPPORT  specified protocol not supported
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_socket
(
  sint15 dss_nethandle,                                   /* application ID */
  byte   family,                       /* Address family - DSS_AF_INET only */
  byte   type,                                              /* socket type */
  byte   protocol,                                        /* protocol type */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSS_SOCKET2()

DESCRIPTION
 Create a socket and related data structures, and return a socket descriptor.

 The mapping to actual protocols is as follows:

 ADDRESS FAMILY         Stream          Datagram

 DSS_AF_INET                TCP             UDP

  Note this function must be called to obtain a valid socket descriptor, for
  use with all other socket-related functions.  Thus, before any socket
  functions can be used (e.g. I/O, asynchronous notification, etc.), this
  call must have successfully returned a valid socket descriptor.  The
  application must also have made a call to dss_open_netlib() to obtain
  a valid application ID, and to put the Data Services task into "sockets"
  mode.

  Note:  This implementation version has no support for Raw IP sockets, and
         will return an error, if the application attempts to create one.

         Sockets created using socket2 are not bound to any particular dss_nethandle.

DEPENDENCIES
  Netpolicy structure needs to be initialized by calling dss_init_netpolicy.

RETURN VALUE
  On successful creation of a socket, this function returns socket file
  descriptor which is a sint15 value between 0x1000 and 0x1FFF.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EAFNOSUPPORT     address family not supported
  DS_EBADAPP          invalid application ID
  DS_EPROTOTYPE       specified protocol invalid for socket type
  DS_ESOCKNOSUPPORT   invalid or unsupported socket parameter specified
  DS_EPROTONOSUPPORT  specified protocol not supported
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_socket2
(
  byte   family,                      /* Address family - DSS_AF_INET only */
  byte   type,                                              /* socket type */
  byte   protocol,                                        /* protocol type */
  dss_sock_cb_fcn sock_cb,                     /* socket callback function */
  void * sock_cb_user_data,              /* User data for socket call back */
  dss_net_policy_info_type * policy_info_ptr,       /* Network policy info */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSS_CONNECT()

DESCRIPTION
  For TCP, attempts to establish the TCP connection.  Upon
  successful connection, calls the socket callback function asserting that
  the DS_WRITE_EVENT is TRUE.  The implementation does not support connected
  UDP sockets and will return an error.  The function must receive
  (as a parameter) a valid socket descriptor, implying a previous successful
  call to dss_socket().

DEPENDENCIES
  Network subsystem must be established and available.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block
  DS_EBADF            invalid socket descriptor is specfied
  DS_ECONNREFUSED     connection attempt refused
  DS_ETIMEDOUT        connection attempt timed out
  DS_EFAULT           addrlen parameter is invalid
  DS_EIPADDRCHANGED   IP address changed due to PPP resync
  DS_EINPROGRESS      connection establishment in progress
  DS_EISCONN          a socket descriptor is specified that is already
                      connected
  DS_ENETDOWN         network subsystem unavailable
  DS_EOPNOTSUPP       invalid server address specified
  DS_EADDRREQ         destination address is required.
  DS_NOMEM            not enough memory to establish connection

SIDE EFFECTS
  For TCP, initiates active open for connection.

===========================================================================*/
extern sint15 dss_connect
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct ps_sockaddr *servaddr,                     /* destination address */
  uint16 addrlen,                                    /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSS_LISTEN()

DESCRIPTION

  For TCP, this starts a passive open for connections.  Upon a
  sucessful connection, the socket callback function is invoked
  asserting DS_ACCEPT_EVENT as TRUE.  The application should respond
  with a call to dss_accept(). If a connection is recieved and there
  are no free queue slots the new connection is rejected
  (ECONNREFUSED).  The backlog queue is for ALL unaccepted sockets
  (half-open, or completely established).

  A listening UDP doesn't make sense, and as such isn't supported.
  DS_EOPNOTSUPP is returned.

  The sockfd parameter is a created (dss_socket) and bound (dss_bind)
  socket that will become the new listening socket.  The backlog
  parameter indiates the maximum length for the queue of pending
  sockets.  If backlog is larger than the maximum, it will be
  reduced to the maximum (see DSS_SOMAXCONN).

  The argument dss_error should point to a memory location in which
  error conditions can be recorded.

DEPENDENCIES

  Network subsystem must be established and available.

  The sockfd should get a valid socket descriptor (implying a
  previously successful call to dss_socket) This socket should be
  bound to a specific port number (implying a previously successful
  call to dss_bind) .

RETURN VALUE

  Returns DSS_SUCCESS on success.  If the backlog was truncated
  DS_EFAULT will be set in errno, but the call will still be
  successful.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block (PJ: I don't think this CAN happen)
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       The socket is not capable of listening (UDP)
  DS_EFAULT           backlog parameter is invalid
  DS_ENETDOWN         network subsystem unavailable
  DS_NOMEM            not enough memory to establish backlog connections.
  DS_EINVAL           Socket already open, closed, unbound or not one
                      you can listen on.

SIDE EFFECTS
  For TCP, initiates passive open for new connections.

===========================================================================*/
extern sint15 dss_listen
(
  sint15 sockfd,                                      /* Socket descriptor */
  sint15 backlog,                      /* Number of connections to backlog */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSS_ACCEPT()

DESCRIPTION

  The accept function is used on listening sockets to respond when
  DS_ACCEPT_EVENT is asserted.  The first backlog queued connection is
  removed from the queue, and bound to a new connected socket (as if
  you called dss_socket).  The newly created socket is in the
  connected state.  The listening socket is unaffect the queue size is
  maintained (ie. there is not need to call listen again.)

  The argument sockfd is the file descriptor of the listening socket

  The argument remote addr is a pointer to a struct ps_sockaddr.  This
  structure is populated with the address information for the remote
  end of the new connection. addrlen should initially contain the
  length of the struct ps_sockaddr passed in.  The length of the real
  address is placed in this location when the struct is populated.

  The argument dss_error should point to a memory location in which
  error conditions can be recorded.

DEPENDENCIES

  Network subsystem must be established and available.

  The sockfd should get a valid socket descriptor (implying a
  previously successful call to dss_socket) This socket should be
  bound to a specific port number (implying a previously successful
  call to dss_bind).  The socket should be listening (implying a
  previously successful call to dss_listen).

RETURN VALUE
  Returns the socket descriptor of the new socket on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       The socket is not of type DSS_SOCK_STREAM
  DS_EINVAL           Socket is not listening.
  DS_EFAULT           The addr parameter is bogus.
  DS_ENETDOWN         network subsystem unavailable
  DS_NOMEM            not enough memory to establish backlog connections.

SIDE EFFECTS

  The head backlog item from the queue of the listening socket is
  removed from that queue.

===========================================================================*/
extern sint15 dss_accept
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct ps_sockaddr *remoteaddr,                    /* new remote address */
  uint16 *addrlen,                                   /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSS_OPEN_NETLIB()

DESCRIPTION

  Opens up the network library.  Assigns application ID and sets the
  application-defined callback functions to be called when library and
  socket calls would make progress.  The callback are called with a pointer
  to a sint15 containing the application ID for the callback.
  NOTE: the memory for the application ID is ephemeral and likely will not be
    available after the callback returns - if it is desired to use this
    information outside the scope of the callback it should be COPIED, a
    pointer MUST not be used.

  Puts data services manager into "socket" mode.

  This function is called from the context of the socket client's task.

DEPENDENCIES
  None.

RETURN VALUE
  Returns application ID on success.

  On error, return DSS_SUCCESS and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EMAPP      no more applications available - max apps exceeded.

SIDE EFFECTS
  Puts data services manager into "socket" mode.

===========================================================================*/
extern sint15 dss_open_netlib
(
  void   (*net_callback_fcn)(void *),         /* network callback function */
  void   (*socket_callback_fcn)(void *),       /* socket callback function */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSS_OPEN_NETLIB2()

DESCRIPTION

  Opens up the network library.  Assigns application ID and sets the
  application-defined callback functions to be called when library and
  socket calls would make progress. Stores the network policy info and
  uses it in further calls.

  Puts data services manager into "socket" mode.

  This function is called from the context of the socket client's task.

DEPENDENCIES
  None.

RETURN VALUE
  Returns application ID on success.

  On error, return DSS_SUCCESS and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EMAPP      no more applications available - max apps exceeded.

SIDE EFFECTS
  Puts data services manager into "socket" mode.

===========================================================================*/
extern sint15 dss_open_netlib2
(
  dss_net_cb_fcn net_cb,                      /* network callback function */
  void *  net_cb_user_data,             /* User data for network call back */
  dss_sock_cb_fcn sock_cb,                     /* socket callback function */
  void * sock_cb_user_data,              /* User data for socket call back */
  dss_net_policy_info_type * policy_info_ptr,       /* Network policy info */
  sint15 *dss_errno                               /* error condition value */
);
/*===========================================================================

FUNCTION DSS_CLOSE_NETLIB()

DESCRIPTION

  Closes the network library for the application.  All sockets must have
  been closed for the application, prior to closing.  If this is the last
  remaining application, the network subsytem (PPP/traffic channel) must
  have been brought down, prior to closing the network library.  If this
  is the last active application using the network library, this function
  takes the data services manager out of "socket" mode.

  This function is called from the context of the socket client's task.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP        invalid application ID
  DS_ESOCKEXIST     there are existing sockets
  DS_ENETEXIST      the network subsystem exists

SIDE EFFECTS
  Puts data services manager into "autodetect" mode.

===========================================================================*/
extern sint15 dss_close_netlib
(
  sint15 dss_nethandle,                                         /* application ID */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_PPPOPEN()

DESCRIPTION
  Starts up the network subsystem (CDMA data service and PPP) over the Um
  interface for all sockets.

DEPENDENCIES
  dss_pppopen() cannot be called by the application if the network is in the
  process of closing. The network layer cannot queue the open request until
  the close is completely finished.  Therefore, the application should wait
  for the net_callback_fn() to be called (after dss_pppclose() has
  completed), before making a call to dss_pppopen().  Note that a valid
  application ID must be specified as a parameter, obtained by a successful
  return of dss_open_netlib().

RETURN VALUE
  If the network subsytem is already established, return DSS_SUCCESS.

  Return DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP               invalid application ID specified
  DS_EWOULDBLOCK           the operation would block
  DS_ENETCLOSEINPROGRESS   network close in progress. The application
                           should only call dss_pppopen() after the
                           close/abort has completed.

SIDE EFFECTS
  Initiates call origination and PPP negotiation.

===========================================================================*/
extern sint15 dss_pppopen
(
  sint15 dss_nethandle,                                         /* application id */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_BIND()

DESCRIPTION
  For all client sockets, attaches a local address and port value to the
  socket.  If the call is not explicitly issued, the socket will implicitly
  bind in calls to dss_connect() or dss_sendto().  Note that this function
  does not support binding a local IP address, but rather ONLY a local port
  number.  The local IP address is assigned automatically by the sockets
  library.  The function must receive (as a parameter) a valid socket
  descriptor, implying a previous successful call to dss_socket().

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       operation not supported
  DS_EADDRINUSE       the local address is already in use.
  DS_EINVAL           the socket is already attached to a local name
  DS_EFAULT           invalid address parameter has been specified

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_bind
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_sockaddr *localaddr,                          /* local address */
  uint16 addrlen,                                     /* length of address */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_CLOSE()

DESCRIPTION
  Non-blocking close of a socket.  Performs all necessary clean-up of data
  structures and frees the socket for re-use.  For TCP initiates the active
  close for connection termination.  Once TCP has closed, the DS_CLOSE_EVENT
  will become TRUE, and the application can call dss_close() again to free
  the socket for re-use.  UDP sockets also need to call this to
  clean-up the socket and free it for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block - TCP close in progress
  DS_EBADF            invalid socket descriptor is specfied

SIDE EFFECTS
  Initiates active close for TCP connections.

===========================================================================*/
extern sint15 dss_close
(
  sint15 sockfd,                                      /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_PPPCLOSE()

DESCRIPTION
  Initiates termination to bring down PPP and the traffic channel.  Upon
  successful close of the network subsystem, invokes the network callback
  function.

DEPENDENCIES
  None.

RETURN VALUE
  If the network subsytem is already closed, return DSS_SUCCESS.

  Returns DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP               invalid application ID specified
  DS_EWOULDBLOCK           operation would block
  DS_ENETCLOSEINPROGRESS   network close in progress. A call to
                           dss_pppclose() has already been issued.

SIDE EFFECTS
  Initiates termination of PPP.  Brings down PPP and traffic channel.

===========================================================================*/
extern sint15 dss_pppclose
(
  sint15 dss_nethandle,                                         /* application id */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_NETSTATUS()

DESCRIPTION
  Provides status of network subsystem.  Called in response to DS_ENETDOWN
  errors.  Note that origination status is based on the last attempted
  origination.

DEPENDENCIES
  None.

RETURN VALUE

  Returns DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP               invalid application ID specified
  DS_ENETNONET             network subsystem unavailable for some unknown
                           reason
  DS_ENETISCONN            network subsystem is connected and available
  DS_ENETINPROGRESS        network subsystem establishment currently in
                           progress
  DS_ENETCLOSEINPROGRESS   network subsystem close in progress.

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_netstatus
(
  sint15 dss_nethandle,                                         /* application ID */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_ASYNC_SELECT()

DESCRIPTION
  Enables the events to be notified about through the asynchronous
  notification mechanism.  Application specifies a bitmask of events that it
  is interested in, for which it will receive asynchronous notification via
  its application callback function.  This function also performs a real-time
  check to determine if any of the events have already occurred, and if so
  invokes the application callback.

DEPENDENCIES
  None.

RETURN VALUE

  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied

SIDE EFFECTS
  Sets the relevant event mask in the socket control block.  Will also
  notify the application via the callback function.

===========================================================================*/
extern sint31 dss_async_select
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 interest_mask,                        /* bitmask of events to set */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_ASYNC_DESELECT()

DESCRIPTION
  Clears events of interest in the socket control block interest mask.  The
  application specifies a bitmask of events that it wishes to clear; events
  for which it will no longer receive notification.

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied


SIDE EFFECTS
  Clears specified events from the relevant event mask.

===========================================================================*/
extern sint15 dss_async_deselect
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 clr_interest_mask,                  /* bitmask of events to clear */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_GETNEXTEVENT()

DESCRIPTION
  This function performs a real-time check to determine if any of the events
  of interest specified in the socket control block's event mask have
  occurred.  It also clears any bits in the event mask that have occurred.
  The application must re-enable these events through a subsequent call to
  dss_async_select().  The application may pass in a pointer to a single
  socket descriptor to determine if any events have occurred for that socket.

  Alternatively, the application may set this pointer's value to NULL (0)
  (note, not to be confused with a NULL pointer, but rather a pointer whose
  value is 0) in which case the function will return values for the next
  available socket.  The next available socket's descriptor will be placed
  in the socket descriptor pointer, and the function will return.  If no
  sockets are available (no events have occurred across all sockets for
  that application) the pointer value will remain NULL (originally value
  passed in), and the function will return 0, indicating that no events
  have occurred.

DEPENDENCIES
  None.

RETURN VALUE
  Returns an event mask of the events that were asserted.  A value of 0
  indicates that no events have occurred.

  On passing a pointer whose value is NULL into the function for
  the socket descriptor (not to be confused with a NULL pointer), places
  the next available socket descriptor in *sockfd_ptr and returns the
  event mask for that socket. If no sockets are available (no events have
  occurred across all sockets for that application) the pointer value
  will remain NULL (originally value passed in), and the function will
  return 0, indicating that no events have occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP           invalid app descriptor is specfied
  DS_EBADF             invalid socket descriptor is specfied

SIDE EFFECTS
  Clears the bits in the socket control block event mask, corresponding to
  the events that have occurred.

===========================================================================*/
extern sint31 dss_getnextevent
(
  sint15 dss_nethandle,                                         /* application ID */
  sint15 *sockfd_ptr,                                 /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_WRITE()

DESCRIPTION
  Sends specified number of bytes in the buffer over the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written, which could be less than the number of
      bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required - connectionless socket
                      did not call dss_connect()
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_write
(
  sint15 sockfd,                                      /* socket descriptor */
  const void *buffer,               /* user buffer from which to copy data */
  uint16 nbytes,                /* number of bytes to be written to socket */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_WRITEV()

DESCRIPTION
  Provides the gather write variant of the dss_write() call, which
  allows the application to write from non-contiguous buffers.    Sends
  specified number of bytes in the buffer over the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written, which could be less than the number of
      bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required - connectionless socket
                      did not call dss_connect()
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_writev
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_iovec iov[],  /* array of data buffers from which to copy data */
  sint15 iovcount,                                /* number of array items */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_READ()

DESCRIPTION
  Reads specified number of bytes into buffer from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of
      bytes specified.  A return of 0 indicates that an End-of-File condition
      has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required - connectionless socket
                      did not call dss_connect()
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_read
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,                     /* user buffer to which to copy data */
  uint16 nbytes,                 /* number of bytes to be read from socket */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_READV()

DESCRIPTION
  Provides the scatter read variant of the dss_read() call, which
  allows the application to read into non-contiguous buffers.    Reads
  specified number of bytes into the buffer from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of
      bytes specified.  A return of 0 indicates that an End-of-File condition
      has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required - connectionless socket
                      did not call dss_connect()
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_readv
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_iovec iov[],        /* array of data buffers to copy data into */
  sint15 iovcount,                                /* number of array items */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_SENDTO()

DESCRIPTION
  Sends 'nbytes' bytes in the buffer over the UDP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EOPNOSUPPORT     option not supported

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_sendto
(
  sint15 sockfd,                                      /* socket descriptor */
  const void *buffer,           /* user buffer from which to copy the data */
  uint16 nbytes,                          /* number of bytes to be written */
  uint32 flags,                                                  /* unused */
  struct ps_sockaddr *toaddr,                       /* destination address */
  uint16 addrlen,                                        /* address length */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_RECVFROM()

DESCRIPTION
  Reads 'nbytes' bytes in the buffer from the UDP transport.  Fills in
  address structure with values from who sent the data.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EOPNOSUPPORT     option not supported

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_recvfrom
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,               /* user buffer from which to copy the data */
  uint16 nbytes,                          /* number of bytes to be written */
  uint32 flags,                                                  /* unused */
  struct ps_sockaddr *fromaddr,                     /* destination address */
  uint16 *addrlen,                                       /* address length */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================
FUNCTION DSS_GET_IFACE_STATUS()

DESCRIPTION Retrieve iface_status

===========================================================================*/
#define dss_get_iface_status dssnet_get_iface_status


/*===========================================================================
FUNCTION DSS_ENABLE_DORM_TIMER()

DESCRIPTION
  NOTE - THIS FUNCTION IS NOW DEPRECATED.
  Applications should use the new ioctl interface - dss_iface_ioctl() -
  to enable/disable dormancy timer. Refer to dss_iface_ioctl.h more details.

  Applications can call this function to enable or disable
  dormancy timer for an IS707 (1X) packet data call only.

DEPENDENCIES
  If multiple applications call this function, principle used is last
  caller wins.
  If this function is invoked to disable timer, the disabling of timer
  remains in effect until next data call (or if the function is again
  invoked in between to enable timer).

PARAMETERS
  dss_nethandle           application ID
  flag             true/false - enable/disable dorm timer

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void dss_enable_dorm_timer
(
  sint15 dss_nethandle,
  boolean flag
);

/*===========================================================================

FUNCTION DSS_SETSOCKOPT

DESCRIPTION
  Set the options associated with a socket. This fuction expects the
  following parameters:

DEPENDENCIES
  None.

PARAMETERS
  int sockfd        -     Socket file descriptor.
  int level         -     Socket option level.
  int optname,      -     Option name.
  void *optval      -     Pointer to the option value.
  uint32 *optlen    -     Pointer to the size of the option value.
  sint15 *dss_errno     -     Error condition value.


RETURN VALUE
  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value
  DS_EFAULT               Invalid buffer or argument

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_setsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                       /* error condition value          */
);


/*===========================================================================
FUNCTION DSS_GETSOCKOPT

DESCRIPTION
  Return an option associated with a socket. This fuction expects the
  following parameters:

DEPENDENCIES
  None.

PARAMETERS
  int sockfd        -     Socket file descriptor.
  int level         -     Socket option level.
  int optname,      -     Option name.
  void *optval      -     Pointer to the option value.
  uint32 *optlen    -     Pointer to the size of the option value.
  sint15 *dss_errno     -     Error condition value.

RETURN VALUE
  optlen is a value-result parameter, initially containing the size of
  the buffer pointed to by optval, and modified on return to indicate the
  actual  size  of the value returned. On error, return DSS_ERROR and places
  the error condition value in *errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value
  DS_EFAULT               Invalid buffer or argument

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_getsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                      /* error condition value          */
);

/*===========================================================================
FUNCTION DSS_GETSOCKNAME

DESCRIPTION
  Returns the current local address assigned to the specified socket.

DEPENDENCIES
  None.

PARAMETERS
  sockfd    -  socket file descriptor
  addr      -  local address currently associated with the socket
  addrlen   -  address length. This parameter is initialized to indicate
               the amount of space pointed by addr and on return, it
               contains the actual size of the address returned.
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion and places the socket
  address and the address length in addr and addrlen parameters, resp.

  If the address is larger than the supplied buffer then it is silently
  truncated. The value returned in addrlen indicates the size prior to
  truncation, if any.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_EFAULT               addr parameter points to an invalid memory
                          location

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_getsockname
(
  sint15              sockfd,               /* socket descriptor           */
  struct ps_sockaddr* addr,                 /* address of the socket       */
  uint16*             addrlen,              /* address length              */
  sint15*             dss_errno             /* error number                */
);

/*===========================================================================
FUNCTION DSS_GETPEERNAME

DESCRIPTION
  Returns the address of the peer connected to the specified socket.

DEPENDENCIES
  None.

PARAMETERS
  sockfd    -  socket file descriptor
  addr      -  address of the peer connected with the socket
  addrlen   -  address length. This parameter is initialized to indicate
               the amount of space pointed by addr and on return, it
               contains the actual size of the address returned.
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion and places the peer
  address and the address length in addr and addrlen parameters, resp.

  If the address is larger than the supplied buffer then it is silently
  truncated. The value returned in addrlen indicates the size prior to
  truncation, if any.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_EFAULT               addr parameter points to an invalid memory
                          location
  DS_ENOTCONN             the socket is not connected

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_getpeername
(
  sint15              sockfd,               /* socket descriptor           */
  struct ps_sockaddr* addr,                 /* address of the socket       */
  uint16*             addrlen,              /* address length              */
  sint15*             dss_errno             /* error number                */
);

/*===========================================================================
FUNCTION DSS_SHUTDOWN

DESCRIPTION
  Shuts down the connection of the specified socket depending on the
  'how' parameter as follows:

  DSS_SHUT_RD:   Disallow subsequent calls to recv function
  DSS_SHUT_WR:   Disallow subsequent calls to send function
  DSS_SHUT_RDWR: Disallow subseuqnet calls to both recv and send functions

DEPENDENCIES
  None.

PARAMETERS
  sockfd    -  socket file descriptor
  how       -  action to be performed: shutdown read-half, write-half or
               both
  dss_errno -  error number

RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS. Otherwise, returns
  DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOTCONN             the socket is not connected
  DS_EINVAL               invalid operation (e.g., how parameter is invalid)
  DS_ENOMEM               insufficient memory available to complete the
                          operation

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_shutdown
(
  sint15           sockfd,                  /* socket descriptor           */
  uint16           how,                     /* what action to perform      */
  sint15*          dss_errno                /* error number                */
);

/*===========================================================================
FUNCTION DSS_INIT_NET_POLICY_INFO

DESCRIPTION
  Populates the policy info structure with default values.
DEPENDENCIES
  None.
PARAMETERS
  POLICY_INFO_PTR  pointer to policy info data structure.
RETURN VALUE
  None
SIDE EFFECTS
  Initializes the fields in the policy info data structure.

===========================================================================*/
void dss_init_net_policy_info
(
  dss_net_policy_info_type * policy_info_ptr       /* policy info structure */
);

/*===========================================================================
FUNCTION DSS_GET_APP_NET_POLICY

DESCRIPTION
 Fills in the policy info structure with the current net policy of the
 application.

DEPENDENCIES
  None.

PARAMETERS
  DSS_NETHANDLE            application id
  POLICY_INFO_PTR  pointer to policy info data structure.
  DSS_ERRNO        error number

RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS. Otherwise, returns
  DSS_ERROR and places the error number in dss_errno.
  Errno Values
  ------------
  DS_EBADAPP              Invalid application ID is specfied
  DS_EFAULT               Invalid policy_info_ptr is specified.

SIDE EFFECTS
  Initializes the fields in the policy info data structure.

===========================================================================*/
sint15 dss_get_app_net_policy
(
  sint15 dss_nethandle,                                          /* Application id */
  dss_net_policy_info_type * policy_info_ptr,     /* policy info structure */
  sint15 * dss_errno                                       /* error number */
);
/*===========================================================================
FUNCTION DSS_SET_APP_NET_POLICY

DESCRIPTION
   Sets the appliation netpolicy to the user specified value.

DEPENDENCIES
  None.

PARAMETERS
  DSS_NETHANDLE            application id
  POLICY_INFO_PTR  pointer to policy info data structure.
  DSS_ERRNO        error number

RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS. Otherwise, returns
  DSS_ERROR and places the error number in dss_errno.
  Errno Values
  ------------
  DS_EBADAPP              Invalid application ID is specfied
  DS_EFAULT               Invalid policy_info_ptr is specified.

SIDE EFFECTS
  Sets the appliation netpolicy to the user specified value.

===========================================================================*/
sint15 dss_set_app_net_policy
(
  sint15 dss_nethandle,                                          /* Application id */
  dss_net_policy_info_type * policy_info_ptr,     /* policy info structure */
  sint15 * dss_errno                                       /* error number */
);

/*===========================================================================
FUNCTION DSS_GET_IFACE_ID_BY_QOS_HANDLE()

DESCRIPTION
  This function returns the iface_id pointing to the iface and the
  corresponding link providing a specific QOS.  The QOS instance is
  identified by the handle previously returned by
  DSS_IFACE_IOCTL_QOS_REQUEST.  The iface_id thus returned is a handle to
  the secondary link of the iface which is providing this QOS instance and
  can be used to perform an IOCTL on that particular link, for example
  registering a PHYS_LINK event on the secondary link.  The handle to the
  secondary link can also be used for DSS_IFACE_IOCTL_QOS_GET_FLOW_SPEC as
  described below.  Only the application (identified by dss_nethandle) which
  previously requested QOS identified by the handle is allowed to retrieve
  the iface_id for the secondary link.

DEPENDENCIES
  None.

PARAMETERS
  handle: qos handle.

RETURN VALUE
  DSS_IFACE_INVALID_ID: If no valid iface could be obtained based on id_info
  iface_id: Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
dss_iface_id_type dss_get_iface_id_by_qos_handle
(
  dss_qos_handle_type  handle     // Handle to QOS instance
);

/*===========================================================================
FUNCTION DSS_RECVMSG()

DESCRIPTION
  This function is a common read function for all the socket input
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and an address field for filling the source address
  of the received packet.The function reads data into the scattered buffers
  over the transport specified by the socket descriptor

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EINVAL           can't recv from a listen socket.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_recvmsg
(
  sint15                  sockfd,   /* socket descriptor                   */
  struct dss_msghdr     * msg,      /* Message header for filling in data  */
  int                     flags,    /* flags from dss_recvfrom             */
  sint15                * dss_errno /* error condition value               */
);

/*===========================================================================
FUNCTION DSS_SENDMSG()

DESCRIPTION
  This function is a common write function for all the socket output
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and destination address for unconnected udp sockets.
  The function writes data from the scattered buffers over the transport
  specified by the socket descriptor.
DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, in case of tcp it could be less
  than the number of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ---------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EMSGSIZE         the msg is too large to be sent all at once
  DS_EISCONN          if the socket is connected and the destination
                      address is other than it is connected to.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_sendmsg
(
  sint15                  sockfd,  /* socket descriptor                    */
  struct dss_msghdr     * msg,     /* Header containing data and dest addr */
  int                     flags,   /* flags used for SDB (if enabled)      */
  sint15                * dss_errno /* error condition value               */
);

/*===========================================================================
FUNCTION DSS_GET_IFACE_ID_BY_ADDR()

DESCRIPTION
  This function return the interface id matching an IP address

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  iface_id: If a valid iface could be obtained based on address
  On error, return DSS_IFACE_INVALID_ID and places the error condition value
  in *dss_errno.

  dss_errno Values
  ----------------
  DS_EFAULT      Invalid arguments passed to the function.
  DS_ENOROUTE    No interface can be determined for the address.
  DS_EOPNOTSUPP  This operation is not supported.

SIDE EFFECTS
  None.
===========================================================================*/
dss_iface_id_type dss_get_iface_id_by_addr
(
  ip_addr_type * ip_addr_ptr,                     /* IP address info       */
  sint15       * dss_errno                        /* error condition value */
);

/*===========================================================================
FUNCTION DSS_GET_SCOPE_ID_BY_IFACE_ID()

DESCRIPTION
  This function allows to retrieve a route_scope from the iface_id.
  Currently, for applications the notion of scope id is basically same as
  iface id as we do not support sitelocal addresses. However, applications
  need not know that scopeid and ifaceid are same as the interpretation can
  change in future when sitelocal multicast is supported.

DEPENDENCIES
  None.

PARAMETERS
  uint32  - Iface id.
  sint15* - Errno.

RETURN VALUE
  On success - Scope Id
  On failure - 0

  dss_errno Values
  ----------------
  DS_EINVAL      Invalid iface id.

SIDE EFFECTS
  None
===========================================================================*/
dss_scope_id_type dss_get_scope_id_by_iface_id
(
  dss_iface_id_type    iface_id,
  sint15 *dss_errno
);

/*===========================================================================
FUNCTION DSS_LAST_NETDOWNREASON()

DESCRIPTION
  This function provides an interface to the applications for retrieving the
  reason for the network interface going down.

DEPENDENCIES
  None.

  PARAMETERS
  dss_nethandle     -  application id calling this function.
  reason    -  network down reason.
  dss_errno -  error number.


RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS and places the
  network down reason in reason. Otherwise, returns DSS_ERROR and places
  the error number in dss_errno.

  dss_errno Values
  ---------------
  DS_EBADAPP               invalid application ID specified

SIDE EFFECTS
  None.
===========================================================================*/

sint15
dss_last_netdownreason
(
  sint15  dss_nethandle,                              /* Application id    */
  dss_net_down_reason_type  *reason,                /* network down reason */
  sint15  *dss_errno                                  /* error number      */
);

/*===========================================================================
FUNCTION DSS_GET_QOS_SHARE_HANDLE()

DESCRIPTION
  This function returns a qos share handle associated with  the set of QOS
  instances requested using the nethandle. Applications can only use this
  handle to set the QOS_SHARE_HANDLE socket option for sockets created using
  socket2. Setting the share handle will enable sockets to use the QOS
  instances created by the nethandle.

DEPENDENCIES
  None.

PARAMETERS
  net_handle: network handle
  dss_errno -  error number.

RETURN VALUE

 dss_errno Values
  ---------------
  DS_EBADAPP               invalid Network handle specified

SIDE EFFECTS
  None.
===========================================================================*/
int32 dss_get_qos_share_handle
(
  sint15 net_handle,
  sint15 *dss_errno
);

/*===========================================================================
FUNCTION DSS_GET_APP_PROFILE_ID

DESCRIPTION
   Gets the application profile id.

DEPENDENCIES
  None.

PARAMETERS
  app_type            application type

RETURN VALUE
  Returns profile_id.

SIDE EFFECTS
  None.

===========================================================================*/
int32 dss_get_app_profile_id
(
  uint32 app_type
);

/*===========================================================================
FUNCTION DSS_GET_IFACE_ID()

DESCRIPTION
  This function is used to get the interface name and instance of a specific
  interface. The interface can be specified using an an dss_nethandle . Note that the
  interface would be the one that the app is currently using.

DEPENDENCIES
  None.

PARAMETERS
  dss_nethandle: application id.

RETURN VALUE
  DSS_IFACE_INVALID_ID: If no valid iface could be obtained based on id_info
  iface_id: Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
#define DSS_GET_IFACE_ID_VERS (3)
dss_iface_id_type
dss_get_iface_id
(
  sint15  dss_nethandle
);

/*===========================================================================
FUNCTION DSS_ENABLE_NAT

DESCRIPTION
  This function is used to enable NAT(Network Address Translation) on the
  modem.

DEPENDENCIES
  None.

PARAMETERS
  net_policy_ptr (IN) - Pointer to the network policy that is used to bring 
                        up the WWAN iface.
  dss_errno (OUT)     - Pointer to the error code if the function returns 
                        NULL.

RETURN VALUE
  handle (OUT)        - Opaque handle returned to the client. 
                        Client should use when disabling NAT by invoking 
                        dss_disable_nat().
                        On Error, NULL will be returned. 
SIDE EFFECTS
  None.

===========================================================================*/
void * dss_enable_nat
(
  dss_net_policy_info_type  *net_policy_ptr, 
  int16                     *dss_errno
);

/*===========================================================================
FUNCTION DSS_DISABLE_NAT

DESCRIPTION
   This function is used to disable NAT that was previously started by 
   invoking dss_enable_nat(). The handle passed to this function should be
   the same handle that was returned in dss_enable_nat().

DEPENDENCIES
  None.

PARAMETERS
  handle (IN)      - Opaque handle that was returned to the client 
                     when it invoked dss_enable_nat().
  dss_errno (OUT)  - Pointer to the error code if the function returns 
                     DSS_ERROR.

RETURN VALUE
  DSS_SUCCESS : On Success.
  DSS_ERROR   : On Failure.

SIDE EFFECTS
  None.

===========================================================================*/
int32 dss_disable_nat
(
  void    *handle,
  int16   *dss_errno
);

/*===========================================================================
FUNCTION DSS_SOFTAP_ENABLE_ROAMING_AUTOCONECT

DESCRIPTION
   This function is used to enable autoconnect when SS is in roaming.

DEPENDENCIES
  None.

PARAMETERS
  dss_errno (OUT)  - Pointer to the error code if the function returns 
                     DSS_ERROR.

RETURN VALUE
  DSS_SUCCESS : On Success.
  DSS_ERROR   : On Failure.

SIDE EFFECTS
  None.

===========================================================================*/
int32 dss_softap_enable_roaming_autoconnect
(
  int16   *dss_errno
);

/*===========================================================================
FUNCTION DSS_SOFTAP_DISABLE_ROAMING_AUTOCONECT

DESCRIPTION
   This function is used to disable autoconnect when SS is in roaming.

DEPENDENCIES
  None.

PARAMETERS
  dss_errno (OUT)  - Pointer to the error code if the function returns 
                     DSS_ERROR.

RETURN VALUE
  DSS_SUCCESS : On Success.
  DSS_ERROR   : On Failure.

SIDE EFFECTS
  None.

===========================================================================*/
int32 dss_softap_disable_roaming_autoconnect
(
  int16   *dss_errno
);

#ifdef __cplusplus
}
#endif

#endif  /* _DSSOCKET_H */
