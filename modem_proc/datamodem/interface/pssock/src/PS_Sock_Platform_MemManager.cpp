/*===========================================================================
  FILE: PS_Sock_Platform_MemManager.cpp

  OVERVIEW: This file provides implementation of the Socket class.

  DEPENDENCIES: None

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_Platform_MemManager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-14 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ds_Sock_SocketDef.h"
#include "PS_Sock_Platform_MemManager.h"
#include "PS_Sock_Platform_Socket.h"
#include "PS_Sock_Platform_SocketFactory.h"
#include "PS_Sock_IO_ByteStream.h"
#include "PS_Sock_IO_DgramQueue.h"
#include "ps_mem.h"
#include "ps_socket_cmd.h"
#include "ds_Utils_DebugMsg.h"
#include "dss_config.h"

using namespace PS::Sock::IO;
using namespace PS::Sock::Platform;


/*===========================================================================

                         INTERNAL DATA DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Macros for sizes of objects of these classes.
---------------------------------------------------------------------------*/
#define PLATFORM_SOCKET_SIZE         (( sizeof(Socket) + 3) & ~3)
#define PLATFORM_SOCKET_FACTORY_SIZE (( sizeof(SocketFactory) + 3) & ~3)
#define PS_SOCKET_CMD_BUF_SIZE       (( sizeof(ps_socket_cmd_type) + 3) & ~3)
#define PS_TCP_BYTE_STREAM_BUF_SIZE  (( sizeof(ByteStream)+3) & ~3)
#define PS_UDP_DGRAM_QUEUE_BUF_SIZE  (( sizeof(DgramQueue)+3) & ~3)


/*---------------------------------------------------------------------------
  Macros for number of bufferes needed, high and low watermarks.
  These are valid for both high end and low end targets.
---------------------------------------------------------------------------*/
#define PLATFORM_SOCKET_NUM_BUF       (DSS_MAX_SOCKS)
#define PLATFORM_SOCKET_HIGH_WM       (DSS_MAX_SOCKS - 5)
#define PLATFORM_SOCKET_LOW_WM        5

#define PLATFORM_SOCKET_FACTORY_NUM_BUF       1
#define PLATFORM_SOCKET_FACTORY_HIGH_WM       1
#define PLATFORM_SOCKET_FACTORY_LOW_WM        0

#define PS_TCP_BYTE_STREAM_NUM_BUF    ((DSS_MAX_TCBS) * 2)
#define PS_TCP_BYTE_STREAM_HIGH_WM    ( PS_TCP_BYTE_STREAM_NUM_BUF - 5)
#define PS_TCP_BYTE_STREAM_LOW_WM     (5)

#define PS_UDP_DGRAM_QUEUE_NUM_BUF    ((DSS_MAX_UDP_SOCKS + DSS_MAX_ICMP_SOCKS) * 3)
#define PS_UDP_DGRAM_QUEUE_HIGH_WM    ( PS_UDP_DGRAM_QUEUE_NUM_BUF - 5)
#define PS_UDP_DGRAM_QUEUE_LOW_WM     (5)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET
  #define PS_SOCKET_CMD_BUF_NUM        150
  #define PS_SOCKET_CMD_BUF_HIGH_WM    120
  #define PS_SOCKET_CMD_BUF_LOW_WM     15
#else
  #define PS_SOCKET_CMD_BUF_NUM        50
  #define PS_SOCKET_CMD_BUF_HIGH_WM    40
  #define PS_SOCKET_CMD_BUF_LOW_WM      5
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*---------------------------------------------------------------------------
  Allocate memory to hold different inet objects along with ps_mem header.
---------------------------------------------------------------------------*/
static int platformSocketFactoryBuf[ PS_MEM_GET_TOT_SIZE
                                     (
                                       PLATFORM_SOCKET_FACTORY_NUM_BUF,
                                       PLATFORM_SOCKET_FACTORY_SIZE
                                     )];

static int platformSocketBuf[ PS_MEM_GET_TOT_SIZE_OPT( PLATFORM_SOCKET_NUM_BUF,
                                                       PLATFORM_SOCKET_SIZE)];

static int ps_socket_cmd_buf_mem[PS_MEM_GET_TOT_SIZE_OPT( PS_SOCKET_CMD_BUF_NUM,
                                                          PS_SOCKET_CMD_BUF_SIZE)];

static int ps_tcp_byte_stream_buf_mem[ PS_MEM_GET_TOT_SIZE_OPT
                                       (
                                         PS_TCP_BYTE_STREAM_NUM_BUF,
                                         PS_TCP_BYTE_STREAM_BUF_SIZE
                                       )];

static int ps_udp_dgram_queue_buf_mem[ PS_MEM_GET_TOT_SIZE_OPT
                                       (
                                         PS_UDP_DGRAM_QUEUE_NUM_BUF,
                                         PS_UDP_DGRAM_QUEUE_BUF_SIZE
                                       )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*---------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to actual object array.
---------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type *  platformSocketBufHdrPtr[ PLATFORM_SOCKET_NUM_BUF];
static Socket *               platformSocketBufPtr[ PLATFORM_SOCKET_NUM_BUF];

static ps_mem_buf_hdr_type *  platformSocketFactoryBufHdrPtr[ PLATFORM_SOCKET_FACTORY_NUM_BUF];
static Socket *               platformSocketFactoryBufPtr[ PLATFORM_SOCKET_FACTORY_NUM_BUF];

static ps_mem_buf_hdr_type *  ps_socket_cmd_buf_hdr_ptr[PS_SOCKET_CMD_BUF_NUM];
static ps_socket_cmd_type  *  ps_socket_cmd_buf_ptr[PS_SOCKET_CMD_BUF_NUM];

static ps_mem_buf_hdr_type *  ps_tcp_byte_stream_buf_hdr_ptr[PS_TCP_BYTE_STREAM_NUM_BUF];
static ByteStream *           ps_tcp_byte_stream_buf_ptr[PS_TCP_BYTE_STREAM_NUM_BUF];

static ps_mem_buf_hdr_type *  ps_udp_dgram_queue_buf_hdr_ptr[PS_UDP_DGRAM_QUEUE_NUM_BUF];
static DgramQueue *           ps_udp_dgram_queue_buf_ptr[PS_UDP_DGRAM_QUEUE_NUM_BUF];

#endif /* FEATURE_DATA_PS_MEM_DEBUG */


/*===========================================================================

                         PUBLIC MEMBER FUNCTIONS

===========================================================================*/
void MemManager::Init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_MEM_POOL_INIT_OPT( PS_MEM_PLATFORM_SOCKET_TYPE,
                            platformSocketBuf,
                            PLATFORM_SOCKET_SIZE,
                            PLATFORM_SOCKET_NUM_BUF,
                            PLATFORM_SOCKET_HIGH_WM,
                            PLATFORM_SOCKET_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                            (int *) platformSocketBufHdrPtr,
                            (int *) platformSocketBufPtr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_1("MemManager::Init(): "
                    "Can't init pool %d", (int) PS_MEM_PLATFORM_SOCKET_TYPE);
  }

  if (ps_mem_pool_init( PS_MEM_PLATFORM_SOCKET_FACTORY_TYPE,
                        platformSocketFactoryBuf,
                        PLATFORM_SOCKET_FACTORY_SIZE,
                        PLATFORM_SOCKET_FACTORY_NUM_BUF,
                        PLATFORM_SOCKET_FACTORY_HIGH_WM,
                        PLATFORM_SOCKET_FACTORY_LOW_WM,
                        NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                        (int *) platformSocketFactoryBufHdrPtr,
                        (int *) platformSocketFactoryBufPtr
#else
                        NULL,
                        NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    LOG_MSG_ERROR_1("MemManager::Init(): "
                    "Can't init pool %d",
                    (int) PS_MEM_PLATFORM_SOCKET_FACTORY_TYPE);
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PS_SOCKET_CMD_TYPE,
                           ps_socket_cmd_buf_mem,
                           PS_SOCKET_CMD_BUF_SIZE,
                           PS_SOCKET_CMD_BUF_NUM,
                           PS_SOCKET_CMD_BUF_HIGH_WM,
                           PS_SOCKET_CMD_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_socket_cmd_buf_hdr_ptr,
                           (int *) ps_socket_cmd_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("MemManager::Init(): "
                    "Can't init the module");
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_TCP_BYTE_STREAM_TYPE,
                           ps_tcp_byte_stream_buf_mem,
                           PS_TCP_BYTE_STREAM_BUF_SIZE,
                           PS_TCP_BYTE_STREAM_NUM_BUF,
                           PS_TCP_BYTE_STREAM_HIGH_WM,
                           PS_TCP_BYTE_STREAM_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_tcp_byte_stream_buf_hdr_ptr,
                           (int *) ps_tcp_byte_stream_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("MemManager::Init(): "
                    "Can't init the module");
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_UDP_DGRAM_QUEUE_TYPE,
                           ps_udp_dgram_queue_buf_mem,
                           PS_UDP_DGRAM_QUEUE_BUF_SIZE,
                           PS_UDP_DGRAM_QUEUE_NUM_BUF,
                           PS_UDP_DGRAM_QUEUE_HIGH_WM,
                           PS_UDP_DGRAM_QUEUE_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_udp_dgram_queue_buf_hdr_ptr,
                           (int *) ps_udp_dgram_queue_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("MemManager::Init(): "
                    "Can't init the module");
  }

} /* MemManager::Init() */

