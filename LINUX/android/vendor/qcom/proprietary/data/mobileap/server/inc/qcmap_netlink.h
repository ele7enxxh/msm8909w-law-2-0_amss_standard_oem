#ifndef _QCMAP_NETLINK_H_
#define _QCMAP_NETLINK_H_

/******************************************************************************

                           QCMAP_NETLINK.H

******************************************************************************/

/******************************************************************************

  @file    qcmap_netlink.h
  @brief   Mobile AP Netlink Socket API

  DESCRIPTION
  Header file for QCMAP NETLINK.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        -------------------------------------------------------
10/01/13   pd         Created. Followed IPACM and QTI_V2 coding conventions.
03/27/14   cp         Added support to DUN+SoftAP.
******************************************************************************/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/if.h>
#include <linux/if_addr.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <asm/types.h>
#include <netinet/ether.h>
#include "qcmap_utils.h"

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/
#define QCMAP_NL_MSG_MAX_LEN                1024
#define IF_NAME_LEN                         16
#define QCMAP_NL_SUCCESS                    0
#define QCMAP_NL_FAILURE                    (-1)
#define QCMAP_NL_INVALID_NW_INTF            -1


/*===========================================================================
                              VARIABLE DECLARARTIONS
===========================================================================*/
/*--------------------------------------------------------------------------
   Stoes the Route info present in the incoming netlink message
---------------------------------------------------------------------------*/
#define QCMAP_NL_ROUTE_INFO_DST_ADDR 0x0001
#define QCMAP_NL_ROUTE_INFO_IFINDEX 0x0002


/*===========================================================================
                       FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
FUNCTION QCMAP_NA_PACKET_SOCKET_INIT()

DESCRIPTION

  This function initializes packet socket to listen for RA packets.
DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_NL_SUCCESS on success
  QCMAP_NL_FAILURE on failure


SIDE EFFECTS
  None
=========================================================================*/
int qcmap_packet_socket_init
(
  qcmap_sk_fd_set_info_t * sk_fdset,
  qcmap_sock_thrd_fd_read_f read_f
);

/*===========================================================================
FUNCTION QCMAP_NL_RECV_MSG()

DESCRIPTION

  Function to receive incoming messages over the NETLINK routing socket.

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_NL_SUCCESS on success
  QCMAP_NL_FAILURE on failure


SIDE EFFECTS
  None
==========================================================================*/
int qcmap_nl_recv_msg(int fd);

/*===========================================================================

FUNCTION QCMAP_PACKET_SOCK_RECV_MSG()

DESCRIPTION

  This function
  - receives the packet socket message.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int qcmap_packet_sock_recv_msg(int fd);


/*===========================================================================
FUNCTION QCMAP_NL_SEND_GETNEIGH_EVENT()

DESCRIPTION

  This function
  - Send RTM_GETNEIGH message for bridge interface

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_NL_SUCCESS on success
  QCMAP_NL_FAILURE on failure


SIDE EFFECTS
  None
==========================================================================*/
int qcmap_nl_send_getneigh_event(void);

#endif /*_QCMAP_NETLINK_H_*/
