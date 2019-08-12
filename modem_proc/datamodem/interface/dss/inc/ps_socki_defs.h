#ifndef PS_SOCKI_DEFS_H
#define PS_SOCKI_DEFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S O C K I _ D E F S . H

DESCRIPTION
  Header containing declarations pertaining to sockets which are used through
  Data Common layers ONLY.

Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/inc/ps_socki_defs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/23/08    pp     Created the file as part of Common Mode Interface:
                   Public / Private API split.
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "queue.h"
#include "ps_in.h"
#include "ps_iface_defs.h"
#include "ps_socket_defs.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Value used to abstract the actual primary phys link instance at DSS level
---------------------------------------------------------------------------*/
#define DSSOCKI_PRIMARY_PHYS_LINK_ID  0xFF

/*---------------------------------------------------------------------------
  Socket allocation constants.
---------------------------------------------------------------------------*/
#define SOCKAVAIL            0                   /* socket is free for use */
#define SOCKFDMAX       0x1FFF                    /* socket descriptor max */
#define APPAVAIL             0              /* application is free for use */
#define DSS_NETHANDLEMAX        0x2FFF            /* socket descriptor max */
#define PORTBASE        (1025)                      /* ephemeral port base */
#define PORTOFFSET      (0xFFFF  - PORTBASE)      /* ephemeral port offset */

/*---------------------------------------------------------------------------
  Socket Half Open Interval
---------------------------------------------------------------------------*/
#define DSS_HALF_OPEN_INTERVAL 15000          /* Max time to ack a syn/ack */

/*---------------------------------------------------------------------------
  Socket structure
---------------------------------------------------------------------------*/
struct socket
{
  struct ps_in6_addr address;                                /* IP address */
  uint16 port;                                              /* port number */
};

/*---------------------------------------------------------------------------
  Connection structure (two sockets)
---------------------------------------------------------------------------*/
struct connection
{
  struct socket local;
  struct socket remote;
};

/*---------------------------------------------------------------------------
  Definitions for pkt_read_info
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 ifaceId;
  uint32 length;
  struct ps_sockaddr_in6 v6RemoteSockAddr;
} ps_pkt_read_info;

/*---------------------------------------------------------------------------
  Definitions for meta data for ICMP err Q
---------------------------------------------------------------------------*/
typedef struct
{
  struct ps_in6_addr           v6_offender;
  struct ps_sock_extended_err  extended_err;
  struct ps_sockaddr_in6       original_dgram_dest_addr;
} ps_icmp_errq_meta_info;

/*---------------------------------------------------------------------------
  Socket Config Structure
---------------------------------------------------------------------------*/
extern struct dss_socket_config_s
{
  /*-------------------------------------------------------------------------
    Flag to determine the any network policy for sockets.
  -------------------------------------------------------------------------*/
  ps_iface_addr_family_type any_network_policy;

  /*-------------------------------------------------------------------------
    Flag to determine whether to do a graceful close or abort the
    connection when the associated interface is dormant.
  -------------------------------------------------------------------------*/
  boolean                   tcp_graceful_dormant_close;

  /*-------------------------------------------------------------------------
    Idle time (in milliseconds) for a TCP connection before TCP starts sending
    keepalive probes.
  -------------------------------------------------------------------------*/
  uint32                    tcp_keepalive_idle_time;

  /*-------------------------------------------------------------------------
    Default Round trip time (in milliseconds) for a TCP connection
  -------------------------------------------------------------------------*/
  uint32                    tcp_default_round_trip_time;

  q_type tcp_cb_q;
  q_type udp_cb_q;
  q_type icmp_cb_q;
  q_type rsvp_cb_q;

} sock_config_cb;

/*---------------------------------------------------------------------------
  protocol control block union type - contains pointers to protocol
  specific control blocks
---------------------------------------------------------------------------*/
struct icmp_cb;
struct tcb;
struct udp_cb;

typedef union
{
  struct icmp_cb *icb;                                             /* ICMP */
  struct tcb *tcb;                                                  /* TCP */
  struct udp_cb *ucb;                                               /* UDP */
  void *p;                                                       /* dunno? */
} protocol_ctl_block_type;

#endif /* PS_SOCKI_DEFS_H */
