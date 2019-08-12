#ifndef PS_UDP_NEW_H
#define PS_UDP_NEW_H
/*===========================================================================

                                  U D P . H

DESCRIPTION
 The Data Services UDP protocol interface header file. This contains all the
 functions, declarations, etc.,  used by the data services to use the UDP
 protocol.

EXTERNALIZED FUNCTIONS
  udp_output()
    Sends UDP packet out to IP, performing pseudo-header checksum
    calculation.
  udp_input()
    Called from IP layer, and hands off UDP packet to UDP protocol layer.

Copyright (c) 1998-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_udp.h_v   1.0   08 Aug 2002 11:19:54   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_udp_io.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/14/08    pp     Common Modem Interface: Public/Private API split.
09/13/08    pp     Metainfo optimizations.
08/15/05    kr     Added support for stats
08/15/05    vp     Support for IP_RECVERR.
06/13/05    vp     Declaration for udp_notify().
08/16/04    ks/msr Added new field, link, to udp_cb structure.
06/11/04    vp     Removed inclusion of netuser.h. Included ps_socket_defs.h.
03/16/04    mct    Renamed acl_ipfilter_info_type to ip_pkt_info_type.
02/04/04    vp     Changed the signature of udp_input function to take
                   acl_ipfilter_info_type structure, dsm item and offset.
03/18/04    vp     Addition of rcvbuf and cant_recv_more to udp_cb.
01/26/04    sv     Changed UDP receive queue to be a watermark instead of
                   dsm item chain.
10/17/03    sv     Added sockfd field to UDP control block.
08/18/03    sv     Modified the code to use IPv6 addresses internally.
08/01/03    ss/sv  Restructured the code to support IPv6
05/28/03    ss     Added rcvcnt to ucb and made rcvq a dsm item chain
05/02/02    mvl    File cleanup and added meta_info to udp_output() prototype
02/07/02    vsk    Feature Wrapped udp_input() & udp_output() under
                   FEATURE_DS_SOCKETS
02/14/01    js     Added a pointer to udp_cb to the parameters list of
                   udp_output() to support DSS_IP_TTL.
02/14/01    js     Removed udp_cb type from the FEATURE_DS_SOCKETS to a
                   non-feature definition.
11/11/00    snn    Renamed scb_type as scb_s
04/05/99    hcg    Incorporated code review changes and comments.  Added
                   definition of maximum UDP payload length.
12/09/98   na/hcg  Created module.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "ps_ip.h"
#include "dsm.h"
#include "ps_tx_meta_info.h"
#include "ps_socket_defs.h"
#include "ps_socki_defs.h"
#include "ps_udp.h"


/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Maximum number of error packets allowed to be queued on the error queue.
---------------------------------------------------------------------------*/
#define UDP_MAX_ERRQ_PKTS 5


/*---------------------------------------------------------------------------
  Structure definition for the UDP Control block.
---------------------------------------------------------------------------*/
struct ps_event_info_s;

struct udp_cb
{
  q_link_type link;             /* Link to next UCB                        */

  ip_cb_type ipcb;
  /* fields for statistics */
  struct udp_cb_stats
  {
    uint32 dgram_rx;
    uint32 dgram_tx;
    uint64 bytes_rx;
    uint64 bytes_tx;
    uint32 dgram_dropped_rx;
    uint32  queried;
  } udp_instance_stats;
  struct connection conn;
  void *scb_ptr;                /* Ptr to socket ctl block                 */
  int32 sockfd;                 /* socket file descriptor                  */

  void   * reader;
  void   * writer;
  void   * errq_ptr;
};


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION UDP_OUTPUT

DESCRIPTION
  This function does the following:
  1. Converts from host to network all the elements of the UDP header
     and inserts the UDP header in the UDP packet.  Host to network
     conversion is implicit to the put16() function.
  2. Creates the pseudo-header and calculates UDP checksum.
  3. Sends UDP packet down to IP.

DEPENDENCIES
  The source IP address should be correct, i.e, IPCP should have negotiated
  the right IP address when this function is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void udp_output
(
  struct ps_in6_addr *dest_ip_ptr,                  /* Destination IP addr */
  udp_hdr_type       *udp_hdr_ptr,                  /* UDP header          */
  dsm_item_type      *bp,                           /* UDP payload         */
  struct udp_cb      *udp_cb_ptr,                   /* UDP control block   */
  ps_tx_meta_info_type  *meta_info_ptr              /* ptr to meta info    */
);

/*===========================================================================
FUNCTION UDP_INPUT

DESCRIPTION
  This function is called from the IP layer and hands off the UDP packet to
  the UDP protocol layer.

  This function runs in the context of the PS task.

  It strips off the UDP header, checks the checksum and then passes the UDP
  Payload to the application layer.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void udp_input
(
  ip_pkt_info_type*        ip_pkt_info_ptr,     /* IP Packet Info          */
  struct dsm_item_s        *data_ptr,           /* Data field, if any      */
  uint16                   offset              /* Offset of the UDP packet */
);

/*===========================================================================
FUNCTION UDP_NOTIFY()

DESCRIPTION
  This function notifies the passed ICMP error to the UDP application
  corresponding to the passed conn structure.

DEPENDENCIES
  None.

PARAMETERS
  uint16                      - ICMP error to report.
  struct ps_sock_extended_err - Socket extended error for ICMP type/code.
  struct connection *         - Pointer to the connection structure.
  struct ps_in6_addr          - Address of the source of ICMP error.
  dsm_item_type *             - Offending IP packet.

RETURN VALUE
  None.

SIDE EFFECTS
  Frees the DSM item.
===========================================================================*/
void udp_notify
(
  uint16                      ps_errno,
  struct ps_sock_extended_err *sock_err,
  struct connection           *conn,
  struct ps_in6_addr          v6_offender,
  dsm_item_type               *bpp
);

#endif /* PS_UDP_NEW_H */
