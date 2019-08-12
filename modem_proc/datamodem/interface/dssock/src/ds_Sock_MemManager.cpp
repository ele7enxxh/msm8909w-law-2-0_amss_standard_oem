/*===========================================================================
  FILE: ds_Sock_MemManager.cpp

  OVERVIEW: This file provides implementation of the Socket class.

  DEPENDENCIES: None

  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_MemManager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-10-11 su  Increase the metainfo buffer counts 
  2010-10-07 am  Increased meta info buf nums for SoftAp modem.
  2010-01-10 ts  Adding SCRATCHPAD for ancillary data buffers.
  2008-05-14 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

#include "ds_Sock_RecvIFInfo.h"
#include "ds_Sock_ICMPErrInfo.h"
#include "ds_Sock_TCPSocket.h"
#include "ds_Sock_UDPSocket.h"
#include "ds_Sock_ICMPSocket.h"
#include "ds_Sock_SocketFactory.h"
#include "ds_Sock_SocketFactoryClient.h"
#include "ds_Sock_Sock1NetworkCtl.h"
#include "ds_Sock_Sock2NetworkCtl.h"
#include "ds_Sock_MemManager.h"
#include "ds_Sock_SocketDef.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_mem.h"
#include "ps_rt_meta_info.h"
#include "ps_pkt_meta_info.h"
#include "ds_Net_MemConfig.h"
#include "ds_Utils_DebugMsg.h"

using namespace ds::Sock;
using namespace ds::Utils;

/*===========================================================================

                         INTERNAL DATA DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Macros for sizes of objects of these classes.
---------------------------------------------------------------------------*/
#define RECV_IF_INFO_SIZE                (( sizeof( RecvIFInfo) + 3) & ~3)
#define ICMP_ERR_INFO_SIZE               (( sizeof( ICMPErrInfo) + 3) & ~3)
#define TCP_SOCKET_SIZE                  (( sizeof( TCPSocket) + 3) & ~3)
#define SOCK_2_NET_CTL_SIZE              (( sizeof( Sock2NetworkCtl) + 3) & ~3)
#define SOCK_1_NET_CTL_SIZE              (( sizeof( Sock1NetworkCtl) + 3) & ~3)
#define UDP_SOCKET_SIZE                  (( sizeof( UDPSocket) + 3) & ~3)
#define ICMP_SOCKET_SIZE                 (( sizeof( ICMPSocket) + 3) & ~3)
#define SOCKET_FACTORY_CLIENT_SIZE       (( sizeof( SocketFactoryClient) + 3) & ~3)
#define SOCKET_FACTORY_SIZE              (( sizeof( SocketFactory) + 3) & ~3)
#define PS_RT_META_INFO_BUF_SIZE         (( sizeof( ps_rt_meta_info_type) + 3) & ~3)
#define PS_PKT_META_INFO_BUF_SIZE        (( sizeof( ps_pkt_meta_info_type) + 3) & ~3)

/*---------------------------------------------------------------------------
  Macros for number of bufferers needed, high and low watermarks.
  These are valid for both high end and low end targets.
---------------------------------------------------------------------------*/
#define TCP_SOCKET_NUM_BUF                 DSS_MAX_TCBS
#define UDP_SOCKET_NUM_BUF                 DSS_MAX_UDP_SOCKS
#define ICMP_SOCKET_NUM_BUF                DSS_MAX_ICMP_SOCKS
#define SOCKET_FACTORY_NUM_BUF             MAX_SOCKET_FACTORY
#define SOCKET_FACTORY_CLIENT_NUM_BUF      DSS_MAX_SOCKS
#define SOCKET_OVERALL_NUM_BUF             (TCP_SOCKET_NUM_BUF + UDP_SOCKET_NUM_BUF + ICMP_SOCKET_NUM_BUF)
#define SOCK_2_NET_CTL_NUM_BUF             SOCKET_OVERALL_NUM_BUF
#define SOCK_1_NET_CTL_NUM_BUF             SOCKET_OVERALL_NUM_BUF

#define TCP_SOCKET_HIGH_WM                 ( TCP_SOCKET_NUM_BUF - 5)
#define SOCK_2_NET_CTL_HIGH_WM             (SOCK_2_NET_CTL_NUM_BUF - 5)
#define SOCK_1_NET_CTL_HIGH_WM             (SOCK_1_NET_CTL_NUM_BUF - 5)
#define UDP_SOCKET_HIGH_WM                 ( UDP_SOCKET_NUM_BUF - 5)
#define ICMP_SOCKET_HIGH_WM                1
#define SOCKET_FACTORY_HIGH_WM             1
#define SOCKET_FACTORY_CLIENT_HIGH_WM      ( SOCKET_FACTORY_CLIENT_NUM_BUF - 1)

#define TCP_SOCKET_LOW_WM                  0
#define SOCK_2_NET_CTL_LOW_WM              0
#define SOCK_1_NET_CTL_LOW_WM              0
#define UDP_SOCKET_LOW_WM                  0
#define ICMP_SOCKET_LOW_WM                 0
#define SOCKET_FACTORY_LOW_WM              0
#define SOCKET_FACTORY_CLIENT_LOW_WM       0

#ifndef IMAGE_MODEM_PROC

  #define RECV_IF_INFO_NUM_BUF               10
  #define ICMP_ERR_INFO_NUM_BUF              10

  #define RECV_IF_INFO_HIGH_WM               8
  #define ICMP_ERR_INFO_HIGH_WM              8

  #define RECV_IF_INFO_LOW_WM                2
  #define ICMP_ERR_INFO_LOW_WM               2

#else

  #define RECV_IF_INFO_NUM_BUF               4
  #define ICMP_ERR_INFO_NUM_BUF              4

  #define RECV_IF_INFO_HIGH_WM               3
  #define ICMP_ERR_INFO_HIGH_WM              3

  #define RECV_IF_INFO_LOW_WM                1
  #define ICMP_ERR_INFO_LOW_WM               1

#endif /* ifndef IMAGE_MODEM_PROC */

#define PS_RT_META_INFO_BUF_NUM       100
#define PS_RT_META_INFO_BUF_HIGH_WM   80
#define PS_RT_META_INFO_BUF_LOW_WM    20

#define PS_PKT_META_INFO_BUF_NUM      100 
#define PS_PKT_META_INFO_BUF_HIGH_WM  80
#define PS_PKT_META_INFO_BUF_LOW_WM   20

/*---------------------------------------------------------------------------
  Allocate memory to hold different ds Net objects along with ps_mem header.
---------------------------------------------------------------------------*/
static int recvIFInfoBuf[ PS_MEM_GET_TOT_SIZE( RECV_IF_INFO_NUM_BUF,
                                               RECV_IF_INFO_SIZE)];

static int icmpErrInfoBuf[ PS_MEM_GET_TOT_SIZE( ICMP_ERR_INFO_NUM_BUF,
                                                ICMP_ERR_INFO_SIZE)];

static int tcpSocketBuf[ PS_MEM_GET_TOT_SIZE_OPT( TCP_SOCKET_NUM_BUF,
                                                  TCP_SOCKET_SIZE)];

static int sock2NetCtlBuf[ PS_MEM_GET_TOT_SIZE_OPT
                            (
                              SOCK_2_NET_CTL_NUM_BUF,
                              SOCK_2_NET_CTL_SIZE
                            )];

static int sock1NetCtlBuf[ PS_MEM_GET_TOT_SIZE_OPT
                            (
                               SOCK_1_NET_CTL_NUM_BUF,
                               SOCK_1_NET_CTL_SIZE
                            )];

static int udpSocketBuf[ PS_MEM_GET_TOT_SIZE_OPT( UDP_SOCKET_NUM_BUF,
                                                  UDP_SOCKET_SIZE)];

static int icmpSocketBuf[ PS_MEM_GET_TOT_SIZE_OPT( ICMP_SOCKET_NUM_BUF,
                                                   ICMP_SOCKET_SIZE)];

static int socketFactoryBuf[ PS_MEM_GET_TOT_SIZE( SOCKET_FACTORY_NUM_BUF,
                                                  SOCKET_FACTORY_SIZE)];


static int socketFactoryClientBuf[ PS_MEM_GET_TOT_SIZE
                                    ( 
                                      SOCKET_FACTORY_CLIENT_NUM_BUF,
                                      SOCKET_FACTORY_CLIENT_SIZE
                                    )];

static int ps_rt_meta_info_buf_mem[ PS_MEM_GET_TOT_SIZE
                                    (
                                      PS_RT_META_INFO_BUF_NUM,
                                      PS_RT_META_INFO_BUF_SIZE
                                    )];

static int ps_pkt_meta_info_buf_mem[ PS_MEM_GET_TOT_SIZE
                                     (
                                       PS_PKT_META_INFO_BUF_NUM,
                                       PS_PKT_META_INFO_BUF_SIZE
                                     )];


#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*---------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to actual object array.
---------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type *  recvIFInfoBufHdrPtr[ RECV_IF_INFO_NUM_BUF];
static RecvIFInfo          *  recvIFInfoBufPtr[ RECV_IF_INFO_NUM_BUF];

static ps_mem_buf_hdr_type *  icmpErrInfoBufHdrPtr[ ICMP_ERR_INFO_NUM_BUF];
static ICMPErrInfo         *  icmpErrInfoBufPtr[ ICMP_ERR_INFO_NUM_BUF];

static ps_mem_buf_hdr_type *  tcpSocketBufHdrPtr[ TCP_SOCKET_NUM_BUF];
static TCPSocket           *  tcpSocketBufPtr[ TCP_SOCKET_NUM_BUF];

static ps_mem_buf_hdr_type *  sock2NetCtlBufHdrPtr[ SOCK_2_NET_CTL_NUM_BUF];
static Sock2NetworkCtl     *  sock2NetCtlBufPtr[ SOCK_2_NET_CTL_NUM_BUF];

static ps_mem_buf_hdr_type *  sock1NetCtlBufHdrPtr[ SOCK_1_NET_CTL_NUM_BUF];
static Sock1NetworkCtl     *  sock1NetCtlBufPtr[ SOCK_1_NET_CTL_NUM_BUF];

static ps_mem_buf_hdr_type *  udpSocketBufHdrPtr[ UDP_SOCKET_NUM_BUF];
static UDPSocket           *  udpSocketBufPtr[ UDP_SOCKET_NUM_BUF];

static ps_mem_buf_hdr_type *  icmpSocketBufHdrPtr[ ICMP_SOCKET_NUM_BUF];
static ICMPSocket          *  icmpSocketBufPtr[ ICMP_SOCKET_NUM_BUF];

static ps_mem_buf_hdr_type *  socketFactoryBufHdrPtr[ SOCKET_FACTORY_NUM_BUF];
static SocketFactory       *  socketFactoryBufPtr[ SOCKET_FACTORY_NUM_BUF];

static ps_mem_buf_hdr_type *  socketFactoryClientBufHdrPtr[ SOCKET_FACTORY_CLIENT_NUM_BUF];
static SocketFactoryClient *  socketFactoryClientBufPtr[ SOCKET_FACTORY_CLIENT_NUM_BUF];

static ps_mem_buf_hdr_type   * ps_rt_meta_info_buf_hdr[PS_RT_META_INFO_BUF_NUM];
static ps_rt_meta_info_type  * ps_rt_meta_info_buf_ptr[PS_RT_META_INFO_BUF_NUM];

static ps_mem_buf_hdr_type   * ps_pkt_meta_info_buf_hdr[PS_PKT_META_INFO_BUF_NUM];
static ps_pkt_meta_info_type * ps_pkt_meta_info_buf_ptr[PS_PKT_META_INFO_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */


/*---------------------------------------------------------------------------
  Local declarations.
---------------------------------------------------------------------------*/

/*===========================================================================

                         PUBLIC MEMBER FUNCTIONS

===========================================================================*/
void MemManager::MemPoolInit
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_0("MemManager::MemPoolInit(): "
                  "Initing Socket Lib mem pools");

  if (ps_mem_pool_init( PS_MEM_RECV_IF_INFO_TYPE,
                        recvIFInfoBuf,
                        RECV_IF_INFO_SIZE,
                        RECV_IF_INFO_NUM_BUF,
                        RECV_IF_INFO_HIGH_WM,
                        RECV_IF_INFO_LOW_WM,
                        NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                        (int *) recvIFInfoBufHdrPtr,
                        (int *) recvIFInfoBufPtr
#else
                        NULL,
                        NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    LOG_MSG_ERROR_1("MemManager::MemPoolInit(): "
                    "Can't init pool %d", (int) PS_MEM_RECV_IF_INFO_TYPE);
  }

  if (ps_mem_pool_init( PS_MEM_ICMP_ERR_INFO_TYPE,
                        icmpErrInfoBuf,
                        ICMP_ERR_INFO_SIZE,
                        ICMP_ERR_INFO_NUM_BUF,
                        ICMP_ERR_INFO_HIGH_WM,
                        ICMP_ERR_INFO_LOW_WM,
                        NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                        (int *) icmpErrInfoBufHdrPtr,
                        (int *) icmpErrInfoBufPtr
#else
                        NULL,
                        NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    LOG_MSG_ERROR_1("MemManager::MemPoolInit(): "
                    "Can't init pool %d", (int) PS_MEM_ICMP_ERR_INFO_TYPE);
  }

  if (PS_MEM_POOL_INIT_OPT( PS_MEM_TCP_SOCKET_TYPE,
                            tcpSocketBuf,
                            TCP_SOCKET_SIZE,
                            TCP_SOCKET_NUM_BUF,
                            TCP_SOCKET_HIGH_WM,
                            TCP_SOCKET_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                            (int *) tcpSocketBufHdrPtr,
                            (int *) tcpSocketBufPtr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_1("MemManager::MemPoolInit(): "
                    "Can't init pool %d", (int) PS_MEM_TCP_SOCKET_TYPE);
  }

  if (PS_MEM_POOL_INIT_OPT( PS_MEM_SOCK_2_NET_CTL_TYPE,
                            sock2NetCtlBuf,
                            SOCK_2_NET_CTL_SIZE,
                            SOCK_2_NET_CTL_NUM_BUF,
                            SOCK_2_NET_CTL_HIGH_WM,
                            SOCK_2_NET_CTL_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                            (int *) sock2NetCtlBufHdrPtr,
                            (int *) sock2NetCtlBufPtr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_1("MemManager::MemPoolInit(): "
                    "Can't init pool %d", (int)PS_MEM_SOCK_2_NET_CTL_TYPE);
  }

  if (PS_MEM_POOL_INIT_OPT( PS_MEM_SOCK_1_NET_CTL_TYPE,
                            sock1NetCtlBuf,
                            SOCK_1_NET_CTL_SIZE,
                            SOCK_1_NET_CTL_NUM_BUF,
                            SOCK_1_NET_CTL_HIGH_WM,
                            SOCK_1_NET_CTL_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                            (int *) sock1NetCtlBufHdrPtr,
                            (int *) sock1NetCtlBufPtr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_1("MemManager::MemPoolInit(): "
                    "Can't init pool %d", (int)PS_MEM_SOCK_1_NET_CTL_TYPE);
  }

  if (PS_MEM_POOL_INIT_OPT( PS_MEM_UDP_SOCKET_TYPE,
                            udpSocketBuf,
                            UDP_SOCKET_SIZE,
                            UDP_SOCKET_NUM_BUF,
                            UDP_SOCKET_HIGH_WM,
                            UDP_SOCKET_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                            (int *) udpSocketBufHdrPtr,
                            (int *) udpSocketBufPtr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_1("MemManager::MemPoolInit(): "
                    "Can't init pool %d", (int) PS_MEM_UDP_SOCKET_TYPE);
  }

  if (PS_MEM_POOL_INIT_OPT( PS_MEM_ICMP_SOCKET_TYPE,
                            icmpSocketBuf,
                            ICMP_SOCKET_SIZE,
                            ICMP_SOCKET_NUM_BUF,
                            ICMP_SOCKET_HIGH_WM,
                            ICMP_SOCKET_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                            (int *) icmpSocketBufHdrPtr,
                            (int *) icmpSocketBufPtr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_1("MemManager::MemPoolInit(): "
                    "Can't init pool %d", (int) PS_MEM_ICMP_SOCKET_TYPE);
  }

  if (ps_mem_pool_init( PS_MEM_SOCKET_FACTORY_CLIENT_TYPE,
                        socketFactoryClientBuf,
                        SOCKET_FACTORY_CLIENT_SIZE,
                        SOCKET_FACTORY_CLIENT_NUM_BUF,
                        SOCKET_FACTORY_CLIENT_HIGH_WM,
                        SOCKET_FACTORY_CLIENT_LOW_WM,
                        NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                        (int *) socketFactoryClientBufHdrPtr,
                        (int *) socketFactoryClientBufPtr
#else
                        NULL,
                        NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    LOG_MSG_ERROR_1("MemManager::MemPoolInit(): "
                    "Can't init pool %d",
                    (int)PS_MEM_SOCKET_FACTORY_CLIENT_TYPE);
  }

  if (ps_mem_pool_init( PS_MEM_SOCKET_FACTORY_TYPE,
                        socketFactoryBuf,
                        SOCKET_FACTORY_SIZE,
                        SOCKET_FACTORY_NUM_BUF,
                        SOCKET_FACTORY_HIGH_WM,
                        SOCKET_FACTORY_LOW_WM,
                        NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                        (int *) socketFactoryBufHdrPtr,
                        (int *) socketFactoryBufPtr
#else
                        NULL,
                        NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    LOG_MSG_ERROR_1("MemManager::MemPoolInit(): "
                    "Can't init pool %d", (int) PS_MEM_SOCKET_FACTORY_TYPE);
  }

  if (ps_mem_pool_init( PS_MEM_RT_META_INFO_TYPE,
                        ps_rt_meta_info_buf_mem,
                        PS_RT_META_INFO_BUF_SIZE,
                        PS_RT_META_INFO_BUF_NUM,
                        PS_RT_META_INFO_BUF_HIGH_WM,
                        PS_RT_META_INFO_BUF_LOW_WM,
                        NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                        (int *) ps_rt_meta_info_buf_hdr,
                        (int *) ps_rt_meta_info_buf_ptr
#else
                        NULL,
                        NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    LOG_MSG_ERROR_0("MemManager::MemPoolInit(): "
                    "Can't init the module");
  }

  if (ps_mem_pool_init( PS_MEM_PKT_META_INFO_TYPE,
                        ps_pkt_meta_info_buf_mem,
                        PS_PKT_META_INFO_BUF_SIZE,
                        PS_PKT_META_INFO_BUF_NUM,
                        PS_PKT_META_INFO_BUF_HIGH_WM,
                        PS_PKT_META_INFO_BUF_LOW_WM,
                        NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                        (int *) ps_pkt_meta_info_buf_hdr,
                        (int *) ps_pkt_meta_info_buf_ptr
#else
                        NULL,
                        NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    LOG_MSG_ERROR_0("MemManagerF::MemPoolInit(): "
                    "Can't init the module");
  }

} /* MemManager::MemPoolInit () */

