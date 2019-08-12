#ifndef PS_IP_H
#define PS_IP_H
/*===========================================================================

                   I P    H E A D E R    F I L E

DESCRIPTION
This file contains IP functions that are common to both IPv4 and IPv6.

Copyright (c) 1995-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ip.h_v   1.1   30 Aug 2002 20:59:24   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_ip.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/12/14    ss     Changes to generate IPSEC info in IP layer.
01/07/11    cp     Added ps_ip_calc_path_mtu() API which calcs the mtu
                   value used in embedded calls.
11/10/09    ss     IP Multicast feature support.
06/15/09    ssh    New flag in ipcb for DAD Neighbor Solicitations
09/30/08    pp     New API to get inner IP fields from DSM (packet) item.
09/12/08    pp     Metainfo optimizations.
11/09/07    ssh    Added support for MIPv6
06/26/07    rs     Added new fields to the structure ip_cb_type to store the
                   multicast TTL value and router alert option.
11/02/06    mct    Supporting IPv4/IPv6 on a single interface for multicast
                   ifaces.
11/02/06    mct    Added support for local IPv6 address binding.
04/19/06    rt     Removed ip_sec_reqd from ip_cb_type.
08/15/05    kr     Removed old stats code.
04/18/05    vp     Added route_scope to ip_cb_type.
03/01/05    sv     Added support to pass metainfo to IP layer on rx_path
01/10/05    sv     Merged IPSEC changes.
01/05/05    msr    Added tclass field to ip_cb_type in order to support
                   IPV6_TCLASS socket option.
12/07/04    msr    Added tos field to ip_cb_type in order to support IP_TOS
                   socket option.
06/15/04    sv     Added flow label field to IP control block.
06/11/04    vp     Removed inclusion of ps_ip_defs.h. Decalration of ip_cb_type
                   Definition of macro IP_PRECEDENCE.
03/16/04    mct    Renamed acl_ipfilter_info_type to ip_pkt_info_type.
03/02/04    rc     Removed tx interface cache entry from ip_send() signature.
02/11/04    rc     Moved function prototypes common to ipv4 and ipv6 to this
                   file.
11/11/03   aku/rc  Added support for IPv6. Added function prototypes for
                   ip6_send() and ip6_input(). Added parameter 'offset' to
                   ip_receive().
01/31/04    usb    Explcitly passing tx iface cache to ip_route()
08/08/03    sv     Restructured the sockets code.
08/30/02    om     Updated ip_send to use routing cache info.
08/01/02    mvl    Removed ip_(de)reg_pkt_handler() and iface.h as they
                   weren't used.
05/03/02    mvl    File cleanup and added meta_info to ip_send() prototype
04/17/02    rc     Defined internal ip_pkt_fn_ptr_type to make it
                   build for all targets
05/20/00    snn    Added support for IP fragments Reassembly.
12/14/98    ldg    Commented out extern declaration for Raw_ip.
10/27/98    ldg    Merged from ARM port: Include comdef.h, iface.h.
06/12/97    ldg    Added new FEATURE_DS_PSTATS
11/14/96    jjw    Unwired Planet Upgrade (ie added UDP)
11/09/95    jjw    IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"
#include "dsm.h"
#include "ps_iface_defs.h"
#include "ps_route_scope.h"
#include "ps_tx_meta_info.h"
#include "ps_rx_meta_info.h"
#include "ps_in.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define IP_DEF_MCAST_TTL 1                        /* Default Multicast TTL */

typedef struct
{
  ps_iface_type        *routing_cache;
  ps_tx_meta_info_type *meta_info_ptr;
  struct ps_in6_addr    src_ip_addr;
  ps_route_scope_type   route_scope;
  uint32                flow_label;
  uint64                options_mask;
  uint32                qos;
  uint8                 ttl;
  uint8                 tos;
  uint8                 df;
  uint8                 tclass;
  uint8                 mcast_ttl;
  boolean               mcast_ttl_set;
  uint8                 mcast_loop;
  boolean               ip_cb_router_alert;
#ifdef FEATURE_DATA_PS_MIPV6
  struct ps_in6_addr    mip6_hoa;
#endif /* FEATURE_DATA_PS_MIPV6 */
#ifdef FEATURE_DATA_PS_IPV6
  boolean               icmp6_ns_is_dad;
#endif /* FEATURE_DATA_PS_IPV6 */
} ip_cb_type;

/*===========================================================================

                      PUBLIC MACRO DECLARATIONS

===========================================================================*/
/*===========================================================================
MACRO         IP_PRECEDENCE()

DESCRIPTION   This macro returns the IP precedence value of the packet.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define  IP_PRECEDENCE(x)     (((x)>>5) & 0x7)

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION IP_SEND()

DESCRIPTION
  This function will Send an IP datagram. Modeled after the example interface
  on p 32 of RFC 791

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int ip_send
(
  ip_cb_type         *ip_cb,          /* IP control block                  */
  struct ps_in6_addr dest,            /* Destination address               */
  uint8              protocol,        /* Protocol                          */
  dsm_item_type*     send_dg_ptr,     /* Pointer to incoming datagram      */
  uint16             length,          /* Optional length of data portion   */
  ps_tx_meta_info_type* meta_info_ptr, /* Metainfo about packet            */
  int16*             ps_errno         /* Error code for failure case       */
);

/*===========================================================================
FUNCTION IP_RECEIVE()

DESCRIPTION
  This function dispatches completed datagrams to the proper transport
  module.  Note that this function DOES NOT perform IP re-assembly, and
  thus, receipt of any IP fragments will be dispatched to the transport
  layer as a full IP datagram.  The transport layer will discard each
  fragment.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ip_receive
(
  dsm_item_type           *bp,                 /* Data portion */
  uint16                  offset,              /* offset where the transport
                                                  protocol begins   */
  ps_rx_meta_info_type    *meta_info_ptr       /* RX meta info */

);

/*===========================================================================

FUNCTION IP_ANY_INPUT()

DESCRIPTION
  This function accepts both received IPv4 or IPv6 packets and routes them to
  the appropriate IP inputs.

DEPENDENCIES
  None

RETURN VALUE
  0 for success, -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int ip_any_input
(
  ps_iface_type *in_if_ptr,           /* Input interface                   */
  dsm_item_type *bp,                  /* Input packet                      */
  uint8         *source,              /* Media source address, if any      */
  int            broadcast,           /* Packet had link broadcast address */
  ps_rx_meta_info_type *meta_info_ptr /* RX meta info pointer                 */
);

/*===========================================================================

FUNCTION IP_SETOPT()

DESCRIPTION
  Sets the IP options to the user specified value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void
ip_setopt
(
  ip_cb_type* ipcb,
  int level,
  int optname,
  void* optval,
  int   optlen
);

/*===========================================================================

FUNCTION IP_INIT_IPCB()

DESCRIPTION
  This function initializes the IP control block.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void
ip_init_ipcb
(
  ip_cb_type * ipcb_ptr                               /* IP control block */
);

/*===========================================================================
  FUNCTION PS_IP_GET_INNER_IP_FIELDS_INTO_IPCB()

DESCRIPTION
  This function processes the DSM item having an IP packet, extracts
  the required inner IP header fields and copies into IPCB. These fields
  from IPCB are further copied into Outer IP header during header
  creation in IP layer.

  This utility function mainly used during Tunneling scenarios, where
  OuterIP header fields are copied from InnerIP header fields.

DEPENDENCIES
  A valid DSM item pointer, valid IPCB pointer should be provided.

PARAMETERS
  pkt_ref_ptr - Ptr to DSM item to process
  ip_cb_ptr   - IPCB pointer

RETURN VALUE
  0 for successful processing of the pkt, -1 in case of error

SIDE EFFECTS
  None.

===========================================================================*/
int ps_ip_get_inner_ip_fields_into_ipcb
(
  dsm_item_type                    *pkt_ref_ptr,
  ip_cb_type                       *ip_cb_ptr
);

/*===========================================================================

  FUNCTION PS_IP_UPDATE_ROUTING_CACHE_IN_IPCB()

DESCRIPTION
  This function updates routing cache in ipcb.

DEPENDENCIES
  None

PARAMETERS
  pkt_ref_ptr - Ptr to DSM item to process
  ip_cb_ptr   - IPCB pointer

RETURN VALUE
  None

SIDE EFFECTS
  Decrements socket reference count of old cache,
  increments socket reference count of new cache
===========================================================================*/
void
ps_ip_update_routing_cache_in_ipcb
(
  ip_cb_type    * ipcb_ptr,
  ps_iface_type * new_routing_cache
);

/*===========================================================================
FUNCTION PS_IP_UPDATE_IPSEC_INFO_IN_IPCB()

DESCRIPTION
  This function runs the outbound ipsec filters and updates the ipsec
  info in the ipcb/meta_info.

DEPENDENCIES
  None

PARAMETERS
  ip_cb_ptr   - IPCB pointer
  ip_pkt_info_ptr - Ptr to the packet information

RETURN VALUE
  None

SIDE EFFECTS
  Update's the ipcb's meta_info_ptr with ipsec information if any filters
  are matched.
===========================================================================*/
void ps_ip_update_ipsec_info_in_ipcb
(
  ip_cb_type       * ipcb_ptr,
  ip_pkt_info_type * ip_pkt_info_ptr
);

/*===========================================================================

FUNCTION     PS_IP_UDPATE_IPSEC_INFO_IN_METAINFO()

DESCRIPTION  This function is used to update the ipsec info within the meta 
             info provided.

DEPENDENCIES None

PARAMETERS   meta_info_ptr,     - Meta info to be updated.

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
void ps_ip_udpate_ipsec_info_in_metainfo
(
  ps_tx_meta_info_type *meta_info_ptr,
  dsm_item_type        *transport_dg_ptr,
  ip_pkt_info_type     *temp_ip_pkt_info_ptr,
  uint8                 protocol
);

/*===========================================================================
FUNCTION PS_IP_CALC_PATH_MTU()

DESCRIPTION
  This function calculates the mtu value used in embedded ip stack calls.

DEPENDENCIES
  None

PARAMETERS
  ip_addr_type - type of the call
  if_ptr       - Interface on which the packet will be routed.

RETURN VALUE
  mtu value used for embedded stack calls in case of success,
  -1 in case of failure.

SIDE EFFECTS
  None
===========================================================================*/
int16
ps_ip_calc_path_mtu
(
  ip_addr_enum_type    ip_addr_type,
  ps_iface_type      * if_ptr
);

#endif /* PS_IP_H */
