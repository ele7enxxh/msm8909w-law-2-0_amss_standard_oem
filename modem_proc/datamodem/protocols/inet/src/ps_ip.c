/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                 P S _ I P . C

GENERAL DESCRIPTION
 Upper half of IP, consisting of send/receive primitives,
  Not needed when running as a standalone gateway.
  Copyright 1991 Phil Karn, KA9Q
  Additional material Copyright 1992 William Allen Simpson
  fragment reassembly removed.


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ip.c_v   1.8   19 Feb 2003 14:33:04   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_ip.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/03/14    ss     Update routing cache in meta info for an incoming SYN 
                   packet
03/12/14    ss     Changes to generate IPSEC info in IP layer.
11/20/13    fn     Fix bug in iPSec info update for iPv4 ESP TCP packets
10/14/13    cx     Fix LLVM warnings 
06/21/13    ash    Support MTU upto 2000
04/10/13    fn     Fix pseudo hdr length for IPv6 ext headers
02/22/13    fn     Moving IP packet len check before setting pseudo hdr len
12/12/12    ds     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
12/10/12    ssh    NV provisioning for IPv6 MTU - fixed includes
12/10/12    ssh    NV provisioning for IPv6 MTU
05/24/12    ss     Klocwork fixes.
04/17/12   vpk     Handle invalid V6 packet length.
01/17/12   mct     Fix to ensure incoming IPSec TCP connections respond with
                   appropriate encryption
01/07/11   cp      Added ps_ip_calc_path_mtu() API which calcs the mtu
                   value used in embedded calls.
09/01/10    pp     IPSec related fixes.
06/17/10    ts     Move implementation of ps_ip_update_routing_cache_in_ipcb 
                   from ps_ip.c to ps_ip_new.c.
11/10/09    ss     IP Multicast feature addition.
09/25/09    ss     KW warning fixes.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
05/22/09    kk     Q6 compiler warning fixes.
09/30/08    pp     New API to get inner IP fields from DSM (packet) item.
09/12/08    pp     Metainfo optimizations.
06/26/07    rs     Added code to the function ip_setopt() to store the
                   multicast TTL value.
02/07/07    scb    Fixed Klocwork High errors
12/15/06    mct    Remove default IP ttl as the IP layer should select the
                   proper ttl/hop limit to use.
11/22/06    sv     Added support for IPSEC IPv6 support.
11/02/06    mct    Supporting IPv4/IPv6 on a single interface for multicast
                   ifaces.
10/25/05    msr    In ip_receive(), trimming IP pkt to get rid of padding
10/24/05    msr    Fixed invalid check for IP pkt length in ip_receive()
08/16/05    mct    Fixed missing v6 featurization.
08/16/05    vp     Cleanup for propagating sys sock info to ipcb.
08/15/05    vp     Storing sys sock info in ipcb.
08/12/05    kr     Removed existing stats code and added code for updating
                   the new statistics framework
08/15/05    vp     Support for IP_RECVERR.
05/11/05    sv      Lint changes.
03/01/05    sv     Added support to enforce security policy on incoming
                   traffic.
03/02/05    vp     Support for ICMP error generation.
02/02/05    ssh    Reverted earlier fix for setsockopt being allowed for TTL=0.
                   Instead, changed min value of TTL in dss_sock_opts_default
                   table to 1 (in dssocki.c)
01/20/05    ssh    Fixed setopt for TTL=0
01/05/05    msr    Added support for IPV6_TCLASS socket option.
12/07/04    msr    Added support for IP_TOS socket option.
11/03/04    mct    Set or clear the IP_RECVIF option in the ipcb options_mask
                   based on what user sets through setsockopt.
10/13/04    vp     Removed inclusion of ps_iphdr.h and included ps_ip4_hdr.h.
                   Removal of byte order conversions as addresses are kept
                   in network order.
06/11/04    vp     Removed inclusion of psglobal.h, internet.h. Included files
                   ps_pkt_info.h, ps_in.h and ps_stats.h. Changes for
                   representation of IP addresses as struct ps_in_addr or struct
                   ps_in6_addr. Replaced use of byte ordering macros with their PS
                   versions. Replaced use of ICMP6_PTCL etc with PS_IPPROTO_ICMP6
04/27/04    usb    Msg Medium to Low.
04/05/04    sv/rc  Featurized v6 specific code under FEATURE_DATA_PS_IPV6
03/29/04    mct    Fixed errors when compiling with V6 turned off.
03/16/04    mct    Renamed acl_ipfilter_info_type to ip_pkt_info_type.
03/02/04    rc     Removed tx interface cache entry from ip_send() signature.
                   Fixed ip_send() to always use routing cache from ip_cb.
02/11/04    rc     Moved functions common to ipv4 and ipv6 to this file.
                   Changed ip_send() so now it is the top level function
                   that in turn calls ip4_send() or ip6_send().
11/11/03    aku/rc Moved function ip_receive() from this file to ps_ip6.c.
01/31/04    usb    Misc routing fixes, removed if_cache from IP filter info,
                   passing NULL iface ptr to global inbound ACL, removed
                   policy info from meta info, inbound ACL support.
09/20/03    usb    Fixed some errors seen with ARM RVCT 2.0 compiler.
09/09/03    jz     Avoided Loopback Routing, and Released Packet if Route is
                   Unavailable
08/21/03    ss     Use the meta_info->routing cache for ICMP as well (as UDP)
08/08/03    sv     Restructured the socket code.
07/18/03    usb    Always use routing cache for all sockets, not only
                   system sockets - removed force route flag.
06/05/03    om     Fixed call to IPSec tx function for ESP packets.
05/27/03    om     Added IPSec ESP processing on the receive path.
04/23/03    ss     Modified ip_route() to use IS_SOCKOPT_SET() macro
04/17/03    aku    Free dsm packet in ip_receive() if mobile receives packet
                   with unknown protocol type.
02/19/03    usb    Setting IP version field of ip_fltr_info in ip_input()
02/13/03    usb    Moved msg macros in ip_receive to LOG_MSG_INFO3
02/12/03    om     Added UDP routing info use in ip_send().
01/31/03    usb    Fixed IP pkt forwarding, fixed IP routing for local
                   packets and loopback iface, added new arg to ntohip()
                   based on the new prototype.
10/03/02    rc     Commented out ps_inbound_acl.h.
09/25/01    om     Updates for forced routing.
08/30/02    om     Updates to use socket's routing cache.
08/01/02    om     Re-added temp. call to pppi_outgoing() to skip routing.
08/01/02    mvl    Removed ip_(de)reg_pkt_handler() and iface.h as they
                   weren't used.
07/31/02    om     Split fragmentation and utilities into separate files.
07/31/02    usb    Removed include psi.h
07/26/02    om     Update to include routing and fixes.
07/17/02    aku    Fixed bug in ip_free_reasm()
05/02/02    mvl    File cleanup and added meta_info to ip_send()
04/17/02    rc     Added support to call IP pkt handler function if registered
                   to process IP packets instead of the default ppp_outgoing()
03/18/02    usb    Added dsm_free_packet in ip_fragment_handler() to free the
                   incoming fragment if dsm_pushdown fails (CR21183).
03/01/02    vsk    removed dsm_free_packet() being called in case of
                   ppp_outgoing() returns an error
02/01/02    jay    2 bytes of offset is pushed before the dsm item is
                   enqueued.(CR19891)
12/21/01    dwp    Add or !FEATURE_DATA_PS to some DSM tracer settings. This
                   is temporary until 5200 syncs up to latest DSM.
12/18/01    rsl    Updated tracer field.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
07/23/01    pjb    Return type of dsm_pushdown changed.  Changed the
                   location/prefix of command names.
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth
                   parameter.Made changes to handle dsm_pushdown()
                   returning FALSE.
                   Handled the case when htonip() returns NULL.
06/08/00    rc     Removed ovld of link ptr in dsm items
05/20/00    snn    Added support for IP Reassembly.
05/04/00    mvl    Changed calls to ppp_outgoing() to reflect new interface.
08/17/99    smp    Added include files msg.h & err.h.
08/09/99    mvl    Added network model changes.
07/07/99    ak     In ip_route, check for ip fragments via the MF flag. If
                   a fragment, then dump ip packet (we do not support ip
                   fragmentation).
04/16/99    hcg    Fixed comment for ip_recv().  The function does not support
                   IP re-assembly.
02/24/99   na/hcg  Added DSSock sockets support.  Includes UDP support, so
                   UDP no longer exclusive to Browser interface.
01/09/99    jjw    changed to generic Browser interface
10/27/98    ldg    ARM porting changes.
01/27/98    ldg    Removed the DS_SLIM switch.
06/17/97    fkm    FEATURE_xxx Updates (via Automatic Scripts)
06/12/97    ldg    Added new FEATURE_DS_PSTATS
02/13/97    na     Removed the ps_ppp_tx_wm. CTIA show build.
11/14/96    jjw    Added changes to support Unwired Planet (UDP) operation
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "memory.h"

#include "dsm.h"
#include "ps_ppp.h"
#include "psi.h"

#include "ps_icmp.h"
#include "ps_tcp.h"
#include "ps_udp_io.h"

#include "ps_acl.h"
#include "ps_inbound_acl.h"
#include "ps_route.h"
#include "ps_iface.h"
#include "ps_ifacei.h"
#include "ps_pkt_info.h"

#include "ps_ip4.h"
#include "ps_in.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ip6.h"
#include "ps_ip6_addr.h"
#include "ps_icmp6.h"
#include "ps_stat_ipv6.h"
#endif
#include "ps_iputil.h"
#include "ps_ip4_hdr.h"
#include "ps_icmp_error.h"
#include "ps_stat_ipv4.h"
#include "ds_Sock_Def.h"

#include "ps_igmp.h"

#ifdef FEATURE_SEC_IPSEC
#include "secips.h"
#include "ps_iface_ipfltr.h"
#include "ps_metai_info.h"
#endif /* FEATURE_SEC_IPSEC */

#include "ds_Utils_DebugMsg.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/



/*===========================================================================

                    PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION IP_SEND()

DESCRIPTION
  This function is used by the upper layers (UDP and TCP) to send
  an IP datagram.
  (Modeled after the example interface on p 32 of RFC 791.)

DEPENDENCIES
  None

RETURN VALUE
  0 for success and -1 for failure (ps_errno contains error code)

SIDE EFFECTS
  None
===========================================================================*/
int ip_send
(
  ip_cb_type           *ip_cb,       /* IP control block                  */
  struct ps_in6_addr    dest,        /* Destination address               */
  uint8                 protocol,    /* Protocol                          */
  dsm_item_type*        send_dg_ptr, /* Pointer to incoming datagram      */
  uint16                length,      /* Optional length of data portion   */
  ps_tx_meta_info_type* meta_info_ptr, /* Metainfo about packet           */
  int16*                ps_errno      /* Error code for failure case      */
)
{
  struct ps_in_addr v4_addr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (ip_cb == NULL)
  {
    return -1;
  }

  if ( ps_iface_addr_family_is_v4(ip_cb->routing_cache))
  {
    v4_addr.ps_s_addr = PS_IN6_GET_V4_FROM_V4_MAPPED_V6(&dest);
    (void) ip4_send( ip_cb,
                     v4_addr,
                     protocol,
                     send_dg_ptr,
                     length,
                     meta_info_ptr,
                     ps_errno);
  }
  else
  {
    #ifdef FEATURE_DATA_PS_IPV6
      (void) ip6_send( ip_cb,
                       dest,
                       protocol,
                       send_dg_ptr,
                       length,
                       meta_info_ptr,
                       ps_errno);
    #else
      LOG_MSG_ERROR_0("IPV6 not supported");
      return -1;
    #endif /* FEATURE_DATA_PS_IPV6 */
  }
  return 0;

} /* ip_send() */



/*===========================================================================

FUNCTION IP_RECEIVE()

DESCRIPTION
  This function generates the psuedo-header for the IP packet.
  It dispatches completed datagrams to the proper transport
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
  ps_rx_meta_info_type  * meta_info_ptr        /* RX meta info ptr */

)
{
  ip_pkt_info_type        *ip_pkt_info_ptr;           /* Pkt hdr and info */
#ifdef FEATURE_SEC_IPSEC
  ps_ipsec_info_type         ipsec_info;
  ps_ipsec_info_type       * meta_ipsec_info_ptr; /*ipsec info from meta info*/
  void                     * ipsec_handle;/* Handle to IPSEC client       */
  int                        i;
  int                        iface_index;
  ps_iface_name_enum_type       if_name;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == meta_info_ptr)
  {
    LOG_MSG_ERROR_0("ip_receive():meta_info_ptr is null");
    dsm_free_packet(&bp);
    return;
  }

  ip_pkt_info_ptr = &PS_RX_META_GET_PKT_INFO(meta_info_ptr);

  /*-------------------------------------------------------------------------
    Generate the pseudo header.
  -------------------------------------------------------------------------*/
  if (ip_pkt_info_ptr->ip_vsn == IP_V4)
  {
    /*-----------------------------------------------------------------------
      Compare IP header's "total length" field with DSM packet length;
      if it is greater, drop the packet and increment statistics. Actual
      packet length can be greater as lower layers might have added some
      padding
    -----------------------------------------------------------------------*/
    if (ip_pkt_info_ptr->ip_hdr.v4.length > dsm_length_packet(bp))
    {
      IPV4_INC_STATS(bad_len, 1);
      LOG_MSG_INFO1_0("Bad packet length: dropping packet ");
      PS_RX_META_INFO_FREE(&meta_info_ptr);
      dsm_free_packet(&bp);
      return;
    }
    else if (ip_pkt_info_ptr->ip_hdr.v4.length < dsm_length_packet(bp))
    {
      /*---------------------------------------------------------------------
        Since DSM item length is greater, it means that IP pkt is padded by
        lower layers. So, trim the padded bytes
      ---------------------------------------------------------------------*/
      dsm_trim_packet(&bp, (uint16)ip_pkt_info_ptr->ip_hdr.v4.length);
    }
    /*-----------------------------------------------------------------------
      Set the Pseudo header after we check the payload length in case we
      do trim the packet. Set the pseudo length equal to payload_len
    -----------------------------------------------------------------------*/
    PS_IN6_GET_V4_MAPPED_V6_FROM_V4(&ip_pkt_info_ptr->pseudo_hdr.source,
                                 ip_pkt_info_ptr->ip_hdr.v4.source.ps_s_addr);
    PS_IN6_GET_V4_MAPPED_V6_FROM_V4(&ip_pkt_info_ptr->pseudo_hdr.dest,
                                 ip_pkt_info_ptr->ip_hdr.v4.dest.ps_s_addr);
    ip_pkt_info_ptr->pseudo_hdr.length =
                                ((uint16)ip_pkt_info_ptr->ip_hdr.v4.length) - IPLEN -
                                ip_pkt_info_ptr->ip_hdr.v4.optlen;

    ip_pkt_info_ptr->pseudo_hdr.protocol =
                                 ip_pkt_info_ptr->ip_hdr.v4.protocol;
  }
  else
  {
#ifdef FEATURE_DATA_PS_IPV6
    uint16 packet_len = 0;
    /*-----------------------------------------------------------------------
      Compare IP header's "pay load length" field with DSM packet length - v6
      header length; if it is greater, drop the packet and increment statistics.
      Actual packet length can be greater as lower layers might have added some
      padding
    -----------------------------------------------------------------------*/
    packet_len  = (uint16)(dsm_length_packet(bp) - IP6LEN);
    if ( ip_pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.payload_len > packet_len )
    {
      IPV6_INC_STATS(bad_len, 1);
      LOG_MSG_INFO1_2("Bad payload length: %d, dropping packet, packet len %d ",
                     ip_pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.payload_len,
                      packet_len);
      PS_RX_META_INFO_FREE(&meta_info_ptr);
      dsm_free_packet(&bp);
      return;
    }
    else if (ip_pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.payload_len < packet_len )
    {
      /*---------------------------------------------------------------------
        Since DSM item length is greater, it means that IP pkt is padded by
        lower layers. So, trim the padded bytes
      ---------------------------------------------------------------------*/
      dsm_trim_packet(&bp,
            (uint16)(ip_pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.payload_len +
            IP6LEN));
    }

    /*-----------------------------------------------------------------------
      Set the Pseudo header after we check the payload length in case we
      do trim the packet. Set the pseudo length equal to payload_len. 
	  Need to also remove IPv6 extension headers from pseudo header length
    -----------------------------------------------------------------------*/
    ip_pkt_info_ptr->pseudo_hdr.source =
                    ip_pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.src_addr;

    ip_pkt_info_ptr->pseudo_hdr.dest =
                    ip_pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.dst_addr;

    ip_pkt_info_ptr->pseudo_hdr.length = 
                    ip_pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.payload_len 
                    	+ IP6LEN - offset ;

    ip_pkt_info_ptr->pseudo_hdr.protocol =
                               (uint8)ip_pkt_info_ptr->ip_hdr.v6.hdr_type;
#else
    LOG_MSG_ERROR_0("IPv6 not supported");
#endif
  }

#ifdef FEATURE_SEC_IPSEC

  LOG_MSG_DATA_PATH_INFO_0("In IP_RECEIVE() executing IPSec input filters");
  ipsec_handle =
    (void *) ps_iface_ipfltr_execute( NULL,
                                      IP_FLTR_CLIENT_IPSEC_INPUT,
                                      PS_IFACE_IPFLTR_SUBSET_ID_DEFAULT,
                                      ip_pkt_info_ptr);
  if(ipsec_handle != NULL)
  {
    if(meta_info_ptr == NULL)
    {
      LOG_MSG_INFO1_0("Missing Required SAs: Dropping packet ");
      PS_RX_META_INFO_FREE(&meta_info_ptr);
      dsm_free_packet(&bp);
      return;
    }

    memset(&(ipsec_info), 0, sizeof(ipsec_info));
    (void)secips_generate_ipsec_info(ipsec_handle,
                                     ip_pkt_info_ptr,
                                     &(ipsec_info),
                                     0);

    LOG_MSG_DATA_PATH_INFO_2("Generate ipsec info for src port %d dest port: %d", 
                           ip_pkt_info_ptr->ptcl_info.tcp.src_port,
                           ip_pkt_info_ptr->ptcl_info.tcp.dst_port);

    meta_ipsec_info_ptr = &(PS_RX_META_GET_IPSEC_INFO(meta_info_ptr));

    for (i = 0; i < ipsec_info.iface_cnt; i++)
    {
      LOG_MSG_DATA_PATH_INFO_2("IPSec iface list index %d  iface name 0x%x ",
                              i, 
                               ((ps_iface_type*)ipsec_info.iface_list[i])->name);
    }
    
    for (i = 0; i < meta_ipsec_info_ptr->iface_cnt; i++)
    {
      LOG_MSG_DATA_PATH_INFO_2("Meta IPSec iface list index %d  iface name 0x%x ",
                              i, 
                               ((ps_iface_type*)meta_ipsec_info_ptr->iface_list[i])->name);
    }

    /* Check the sanity */
    if ( (ipsec_info.iface_cnt >= meta_ipsec_info_ptr->iface_cnt) ||
         (meta_ipsec_info_ptr->iface_cnt > SECIPS_MAX_IFACE_LIST) )
    {
      LOG_MSG_INFO1_2("SAs sanity mismatch: Dropping packet: EXP (%x) : RCV(%x) ",
                      ipsec_info.iface_cnt, meta_ipsec_info_ptr->iface_cnt);
      PS_RX_META_INFO_FREE(&meta_info_ptr);
      dsm_free_packet(&bp);
      return;
    }
    
    for (i = 0; i < ipsec_info.iface_cnt; i++)
    {
      /* The IFACE order is reversed in the INPUT direction. */
      iface_index         = meta_ipsec_info_ptr->iface_cnt - 1 - i;

      if (ipsec_info.iface_list[i] !=
            meta_ipsec_info_ptr->iface_list[iface_index])
      {
        LOG_MSG_INFO1_2("Missing Required SAs: Dropping packet: EXP (%x) : RCV(%x) ",
                 ipsec_info.iface_list[i],
                        meta_ipsec_info_ptr->iface_list[iface_index]);
        PS_RX_META_INFO_FREE(&meta_info_ptr);
        dsm_free_packet(&bp);
        return;
      }
    }

    if(ip_pkt_info_ptr->esp_hdr.esp_handle != ipsec_info.esp_sa_ptr)
    {
      LOG_MSG_INFO1_0("Missing Required SAs: Dropping packet ");
      PS_RX_META_INFO_FREE(&meta_info_ptr);
      dsm_free_packet(&bp);
      return;
    }

    if(ip_pkt_info_ptr->ah_hdr.ah_handle != ipsec_info.ah_sa_ptr)
    {
      LOG_MSG_INFO1_0("Missing Required SAs: Dropping packet ");
      PS_RX_META_INFO_FREE(&meta_info_ptr);
      dsm_free_packet(&bp);
      return;
    }

    /* IPSEC is validated. So set the incoming IFACE to physical IFACE */
    ip_pkt_info_ptr->if_ptr =  PS_RX_META_GET_IPSEC_INFO(meta_info_ptr).iface_list[0];
  }
  else {
    if_name = ((ps_iface_type *)(ip_pkt_info_ptr->if_ptr))->name;
    if (  if_name == IPSEC_IFACE ) {
      MSG_ERROR("Filtering failure on the packet which has come on IPsec Iface: Drop the Packet", 0,0,0);
      PS_RX_META_INFO_FREE(&meta_info_ptr);
      dsm_free_packet(&bp);
      return;
    }
  }
#endif /* FEATURE_SEC_IPSEC */

  LOG_MSG_DATA_PATH_INFO_1("In IP_RECEIVE() protocol : %d",
                           ip_pkt_info_ptr->pseudo_hdr.protocol);

  /*-------------------------------------------------------------------------
    Look at protocol field of Extracted IP header to determine the
    transport layer for processing and possible reply, either TCP or ICMP.
  -------------------------------------------------------------------------*/
  switch( ip_pkt_info_ptr->pseudo_hdr.protocol)
  {
#ifdef FEATURE_DATA_PS_IPV6
    case PS_IPPROTO_ICMP6:

      icmp6_input(ip_pkt_info_ptr,
                  &bp,
                  offset);
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    case PS_IPPROTO_TCP:
      tcp_input( ip_pkt_info_ptr,
                 bp,
                 offset);

      break;

    case PS_IPPROTO_UDP:
      udp_input( ip_pkt_info_ptr,
                 bp,
                 offset);
      break;


    case PS_IPPROTO_ICMP:
      icmp_input( ip_pkt_info_ptr,
                  bp,
                  offset);
      break;

    case PS_IPPROTO_IGMP:
	      ps_igmp_input( ip_pkt_info_ptr,
	                     bp,
	                     offset);
      break;

    default:
      LOG_MSG_ERROR_1( "Invalid Protocol Field: %x",
                       ip_pkt_info_ptr->pseudo_hdr.protocol );
      /*---------------------------------------------------------------------
        Send protocol unreachable ICMP error if IPv4.
      ---------------------------------------------------------------------*/
      if(ip_pkt_info_ptr->ip_vsn == IP_V4)
      {
        /*-------------------------------------------------------------------
          Increment ipv4 stats
        *-----------------------------------------------------------------*/
        IPV4_INC_STATS(no_proto,1) ;
        IPV4_INC_STATS(pkts_dropped_rx,1) ;
        icmpv4_error(&bp,
                     ICMP_DEST_UNREACH,
                     ICMP_PROTO_UNREACH,
                     ip_pkt_info_ptr);
      }
#ifdef FEATURE_DATA_PS_IPV6
      else
      {
        /*-------------------------------------------------------------------
          Increment ipv6 stats
        *-----------------------------------------------------------------*/
        IPV6_INC_STATS(no_proto,1) ;
        IPV6_INC_STATS(pkts_dropped_rx,1) ;
        dsm_free_packet(&bp);
      }
#endif /* FEATURE_DATA_PS_IPV6 */
      break;
  }
  
  PS_RX_META_INFO_FREE(&meta_info_ptr);
} /* ip_receive() */


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
)
{
  uint8 ip_ver=0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------
    Extract the IP version of packet.
  ---------------------------------------------------------------------*/
  if(sizeof(uint8) != dsm_extract(bp, 0, &ip_ver, sizeof(uint8)))
  {
    LOG_MSG_INFO2_0("unable to get IP version");
    PS_RX_META_INFO_FREE(&meta_info_ptr);
    dsm_free_packet(&bp);
    return -1;
  }

  /*---------------------------------------------------------------------
    Pass the packet to the appropriate IP input function based on the IP
    version.
  ---------------------------------------------------------------------*/
  if(IP_V4 == (ip_ver >> 4))
  {
    return (ip4_input(in_if_ptr,
                      bp,
                      source,
                      broadcast,
                      meta_info_ptr));
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if(IP_V6 == (ip_ver >> 4))
  {
    return (ip6_input(in_if_ptr,
                      bp,
                      source,
                      broadcast,
                      meta_info_ptr));
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  LOG_MSG_INFO2_1("Incorrect IP version %d", (ip_ver >> 4));
  PS_RX_META_INFO_FREE(&meta_info_ptr);
  dsm_free_packet(&bp);
  return -1;
} /* ip_any_input() */

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
void ip_setopt
(
  ip_cb_type *  ipcb,
  int           level,
  int           optname,
  void       *  optval,
  int           optlen
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DATA_PS_IPV6
  ASSERT(ds_Sock_OptLevel_QDS_LEVEL_IP == level || ds_Sock_OptLevel_QDS_LEVEL_IPV6 == level);
#else
  ASSERT(ds_Sock_OptLevel_QDS_LEVEL_IP == level);
#endif
  (void)optlen;

  switch(optname)
  {
#ifdef FEATURE_DATA_PS_IPV6
    case ds_Sock_OptName_QDS_IPV6_TCLASS:
    {
      ipcb->tclass = (uint8) (*(int *) optval);
      break;
    }
#endif /* FEATURE_DATA_PS_IPV6 */

    case ds_Sock_OptName_QDS_IP_TOS:
    {
      ipcb->tos = (uint8) (*(int *) optval);
      break;
    }

    case ds_Sock_OptName_QDS_IP_TTL:
    {
      ipcb->ttl = (uint8)(*(int *)optval);
      break;
    }

    case ds_Sock_OptName_QDS_IP_RECVIF:
    case ds_Sock_OptName_QDS_IP_RECVERR:
#ifdef FEATURE_DATA_PS_IPV6
    case ds_Sock_OptName_QDS_IPV6_RECVERR:
#endif /* FEATURE_DATA_PS_IPV6 */
    {
      if (*((int *) optval))
      {
        ipcb->options_mask |= ((uint64) 1 << optname);
      }
      else
      {
        ipcb->options_mask &= ~((uint64) 1 << optname);
      }

      break;
    }

    case ds_Sock_OptName_QDS_IP_MULTICAST_TTL:
    {      
      ipcb->mcast_ttl_set = TRUE;
      ipcb->mcast_ttl = (uint8)(*(int *)optval);
      break;
    }

    case ds_Sock_OptName_QDS_IP_MULTICAST_LOOP:
    {      
      ipcb->mcast_loop = (uint8)(*(int *)optval);
      break;
    }	

    default:
    {
      LOG_MSG_INFO1_0("IP_SETOPT: UNKNOWN SOCKET OPTION ");
      break;
    }
  } /* switch (optname) */

  return;
} /* ip_setopt() */

/*===========================================================================

FUNCTION IP_INIT_IPCB()

DESCRIPTION
  This function initializes the IP control block.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the IP control block
===========================================================================*/
void
ip_init_ipcb
(
  ip_cb_type * ipcb_ptr                               /* IP control block */
)
{
  (void)ipcb_ptr;
}

/*===========================================================================
  FUNCTION PS_IP_GET_INNER_IP_FIELDS_INTO_IPCB()

DESCRIPTION
  This function processes the DSM item having an IP packet, extracts
  the required inner IP header fields and copies into IPCB. These fields
  from IPCB are further copied into Outer IP header during header
  creation in IP layer.

  This utility function mainly used during Tunneling scenarios, where
  OuterIP header fields are copied from InnerIP header fields.

  Ref: RFC 1853 - IP in IP tunneling
       RFC 2003 - IP encapsulation within IP: section 3.1
       RFC 2473 - Generic packet tunneling in IPv6: sections 5,6

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
)
{
  ip_version_enum_type      ip_ver;
  uint32                    field[2]; /* for extracting 64 bits */

  /* Extract first 64bits - IP version, TOS/TCLASS, DF,FLOW LABEL (IPv6) */
  if (sizeof(field) != dsm_extract(pkt_ref_ptr, 0, &field[0], sizeof(field)))
  {
    LOG_MSG_ERROR_0("Unable to do DSM extract");
    return -1;
  }

  /* Convert first 32-bit into Host order */
  field[0] = ps_ntohl(field[0]);

  /* Get IP version */
  ip_ver = (ip_version_enum_type) (field[0] >> 28);

  /* Based on IP version type, copy specific bytes */
  if(IP_V4 == ip_ver)
  {
    /* IPv4 field: TOS/DSCP */
    ip_cb_ptr->tos = (uint8)((field[0] >> 16) & 0xFF);

    /* IPv4 field: DF */
    field[1] = ps_ntohl(field[1]);
    ip_cb_ptr->df = (uint8)(((field[1]) & 0x4000) >> 14);
  }
  else if(IP_V6 == ip_ver)
  {
    /* IPv6 field: TCLASS, FLOW LABEL */
    ip_cb_ptr->flow_label = field[0] & (0xFFFFF);
    ip_cb_ptr->tclass = (uint8)((field[0] >> 20) & (0xFF));
  }
  else
  {
    LOG_MSG_ERROR_0("Invalid IP version");
    return -1;
  }
  return 0;
}/* ps_ip_get_inner_ip_fields_into_ipcb */

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
void ps_ip_update_routing_cache_in_ipcb
(
 ip_cb_type    * ipcb_ptr,
 ps_iface_type * new_routing_cache
 )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
  Update IPCB routing cache
  -------------------------------------------------------------------------*/
  ipcb_ptr->routing_cache = new_routing_cache;

}/* ps_ip_update_routing_cache_in_ipcb() */

#ifdef FEATURE_SEC_IPSEC
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
)
{
  ps_ipsec_info_type   ipsecInfo;           /* IPSec info struct to update */
  uint32               ipsecHandle;    /* IPsec handle from matched filter */
  ip_pkt_info_type     temp_ip_pkt_info;   /* Pktinfo to use for filtering */
  uint8                temp_protocol;           /* Temporary protocol type */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* On listening sockets no data has been sent yet so filters
   * won't have been run. Run IPSec output filters to ensure metainfo 
   * is updated with proper ESP info prior to responding to syn    */

  /* Setup temp pkt info for outgoing pkt to filter on */
  memset( &temp_ip_pkt_info, 0, sizeof( temp_ip_pkt_info ) );
  temp_ip_pkt_info.ip_vsn = ip_pkt_info_ptr->ip_vsn;
  temp_ip_pkt_info.if_ptr = ip_pkt_info_ptr->if_ptr;

  if(temp_ip_pkt_info.ip_vsn == IP_V4)
  {
    temp_protocol                       = ip_pkt_info_ptr->ip_hdr.v4.protocol;
    temp_ip_pkt_info.ip_hdr.v4.protocol = ip_pkt_info_ptr->ip_hdr.v4.protocol;
    temp_ip_pkt_info.ip_hdr.v4.source   = ip_pkt_info_ptr->ip_hdr.v4.dest;
    temp_ip_pkt_info.ip_hdr.v4.dest     = ip_pkt_info_ptr->ip_hdr.v4.source;
  }
  else
  {
    temp_protocol = (uint8) ip_pkt_info_ptr->ip_hdr.v6.hdr_type;
    temp_ip_pkt_info.ip_hdr.v6.hdr_type = ip_pkt_info_ptr->ip_hdr.v6.hdr_type;
    temp_ip_pkt_info.ip_hdr.v6.hdr_body.base_hdr.next_hdr = 
      (byte) ip_pkt_info_ptr->ip_hdr.v6.hdr_type;
    temp_ip_pkt_info.ip_hdr.v6.hdr_body.base_hdr.dst_addr = 
      ip_pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.src_addr;
    temp_ip_pkt_info.ip_hdr.v6.hdr_body.base_hdr.src_addr = 
      ip_pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.dst_addr;
  }

  if(temp_protocol == (uint8) PS_IPPROTO_TCP)
  {
    temp_ip_pkt_info.ptcl_info.tcp.src_port = 
      ip_pkt_info_ptr->ptcl_info.tcp.dst_port;
    temp_ip_pkt_info.ptcl_info.tcp.dst_port = 
      ip_pkt_info_ptr->ptcl_info.tcp.src_port;
  }
  else if(temp_protocol == (uint8) PS_IPPROTO_UDP)
  {
    temp_ip_pkt_info.ptcl_info.udp.src_port = 
      ip_pkt_info_ptr->ptcl_info.udp.dst_port;
    temp_ip_pkt_info.ptcl_info.udp.dst_port = 
      ip_pkt_info_ptr->ptcl_info.udp.src_port;
  }
  else
  {
    LOG_MSG_ERROR_1("IPSec not support for protocol %d",temp_protocol);
  }
  
  LOG_MSG_DATA_PATH_INFO_2("Running ipsec filters src port %d dest port: %d", 
                         temp_ip_pkt_info.ptcl_info.tcp.src_port,
                         temp_ip_pkt_info.ptcl_info.tcp.dst_port);

  
  ipsecHandle = ps_iface_ipfltr_execute( NULL,
                                         IP_FLTR_CLIENT_IPSEC_OUTPUT,
                                         PS_IFACE_IPFLTR_SUBSET_ID_DEFAULT,
                                         &temp_ip_pkt_info);

  /*---------------------------------------------------------------
    Generate ipsecInfo if filters matched and update metainfo
  ---------------------------------------------------------------*/
  memset( &ipsecInfo, 0, sizeof( ipsecInfo));

  if (0 != ipsecHandle)
  {
    (void) secips_generate_ipsec_info
           (
             (void*) ipsecHandle,
             &temp_ip_pkt_info,
             &ipsecInfo,
             (uint32) ipcb_ptr
           );

    if ((ipcb_ptr->meta_info_ptr == NULL) ||
       (PS_TX_META_GET_RT_META_INFO_PTR(ipcb_ptr->meta_info_ptr) == NULL))
    {
      PS_TX_META_INFO_AND_RT_META_INFO_GET(ipcb_ptr->meta_info_ptr);

      if ((ipcb_ptr->meta_info_ptr != NULL ) &&
       (PS_TX_META_GET_RT_META_INFO_PTR(ipcb_ptr->meta_info_ptr) != NULL))
      {	  
        PS_TX_META_SET_ROUTING_CACHE(ipcb_ptr->meta_info_ptr,
                                     ipcb_ptr->routing_cache);
        LOG_MSG_INFO1_1("Routing cache iface=0x%x",ipcb_ptr->routing_cache);
      }
    }

    /* Verify if meta info received. */
    if( ipcb_ptr->meta_info_ptr == NULL )
    {
      LOG_MSG_ERROR_0("NULL meta info.");
      return;
    }

    PS_RT_META_SET_IPSEC_INFO( PS_TX_META_GET_RT_META_INFO_PTR(ipcb_ptr->meta_info_ptr),
                               ipsecInfo);
  }
} /* ps_ip_update_ipsec_info_in_ipcb() */

/*===========================================================================
FUNCTION     PS_IP_UDPATE_IPSEC_INFO_IN_METAINFO()

DESCRIPTION  This function is used to update the ipsec info within the meta 
             info provided.

DEPENDENCIES temp_ip_pkt_info_ptr should be populated with IP hdr info.

PARAMETERS   meta_info_ptr - Meta info to be updated.
             temp_ip_pkt_info_ptr - IP Pkt info required to execute IPSEC 
                                    filters. 
                                    Partially populated with IP HDR info.
                                    Rest would be inferred in this API from
                                    the datagram.
             transport_dg_ptr - DSM item containing IP pkt payload, i.e. 
                                Transport layer header onwards.

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
void ps_ip_udpate_ipsec_info_in_metainfo
(
  ps_tx_meta_info_type *meta_info_ptr,
  dsm_item_type        *transport_dg_ptr,
  ip_pkt_info_type     *temp_ip_pkt_info_ptr,
  uint8                 protocol
)
{
  ps_ipsec_info_type ipsecInfo;        /* IPSec info struct to update */
  uint32             ipsecHandle;      /* IPsec handle from matched filter */

  #define TEMP_BUF_SIZE 4
  uint8 temp_buf[TEMP_BUF_SIZE] = {0}; /* To infer port values */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Validation checks
  -------------------------------------------------------------------------*/
  if( meta_info_ptr == NULL || transport_dg_ptr == NULL || 
      temp_ip_pkt_info_ptr == NULL )
  {
    LOG_MSG_ERROR_3( "ps_ip_udpate_ipsec_info_in_metainfo(): "
                     "NULL args : 0x%x 0x%x 0x%x", meta_info_ptr, 
                     transport_dg_ptr, temp_ip_pkt_info_ptr );
    return;
  }
  if( temp_ip_pkt_info_ptr->ip_vsn != IP_V4 && 
      temp_ip_pkt_info_ptr->ip_vsn != IP_V6 )
  {
    LOG_MSG_ERROR_1( "ps_ip_udpate_ipsec_info_in_metainfo():"
                     "Temp IP Pkt info doesnt have IP hdr info set, vsn %d",
                     temp_ip_pkt_info_ptr->ip_vsn );
    return;
  }

  /*-------------------------------------------------------------------------
    Setup temp pkt info for outgoing pkt to filter on
  -------------------------------------------------------------------------*/
  /* Update Temp IP Pkt Info with Transport layer values */
  /* Extract src and dst port values */
  (void)dsm_extract( transport_dg_ptr, 0, temp_buf, 
                     TEMP_BUF_SIZE*sizeof(uint8) );

  if( protocol == (uint8) PS_IPPROTO_TCP )
  {
    temp_ip_pkt_info_ptr->ptcl_info.tcp.src_port = *((uint16*)&temp_buf[0]);
    temp_ip_pkt_info_ptr->ptcl_info.tcp.dst_port = *((uint16*)&temp_buf[2]);

    if( temp_ip_pkt_info_ptr->ip_vsn == IP_V6 )
    {
      temp_ip_pkt_info_ptr->ip_hdr.v6.hdr_type = PS_IPPROTO_TCP;
      temp_ip_pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.next_hdr = 
        (byte) PS_IPPROTO_TCP;
    }
    else
    {
      temp_ip_pkt_info_ptr->ip_hdr.v4.protocol = PS_IPPROTO_TCP;
    }
  }
  else if( protocol == (uint8) PS_IPPROTO_UDP )
  {
    temp_ip_pkt_info_ptr->ptcl_info.udp.src_port = *((uint16*)&temp_buf[0]);
    temp_ip_pkt_info_ptr->ptcl_info.udp.dst_port = *((uint16*)&temp_buf[2]);

    if( temp_ip_pkt_info_ptr->ip_vsn == IP_V6 )
    {
      temp_ip_pkt_info_ptr->ip_hdr.v6.hdr_type = PS_IPPROTO_UDP;
      temp_ip_pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.next_hdr = 
        (byte) PS_IPPROTO_UDP;
    }
    else
    {
      temp_ip_pkt_info_ptr->ip_hdr.v4.protocol = PS_IPPROTO_UDP;
    }
  }
  else
  {
    LOG_MSG_ERROR_1( "ps_ip_udpate_ipsec_info_in_metainfo(): "
                     "IPSec not supported for protocol %d", protocol );
    return;
  }
  
  LOG_MSG_DATA_PATH_INFO("ps_ip_udpate_ipsec_info_in_metainfo(): "
                         "src port %d dst port: %d", 
                         temp_ip_pkt_info_ptr->ptcl_info.tcp.src_port,
                         temp_ip_pkt_info_ptr->ptcl_info.tcp.dst_port,0);

  /*-------------------------------------------------------------------------
    temp_ip_pkt_info populated, not execute IPSEC filters
  -------------------------------------------------------------------------*/
  ipsecHandle = ps_iface_ipfltr_execute( NULL,
                                         IP_FLTR_CLIENT_IPSEC_OUTPUT,
                                         PS_IFACE_IPFLTR_SUBSET_ID_DEFAULT,
                                         temp_ip_pkt_info_ptr );

  /*-------------------------------------------------------------------------
    Generate ipsecInfo if filters matched and update metainfo
  -------------------------------------------------------------------------*/
  memset( &ipsecInfo, 0, sizeof( ipsecInfo));

  if (0 != ipsecHandle)
  {
    (void) secips_generate_ipsec_info
           (
             (void*) ipsecHandle,
             temp_ip_pkt_info_ptr,
             &ipsecInfo,
             (uint32)NULL
           );

    PS_RT_META_SET_IPSEC_INFO( 
      PS_TX_META_GET_RT_META_INFO_PTR(meta_info_ptr), ipsecInfo);
  }
  else
  {
    LOG_MSG_INFO2_0("ps_ip_udpate_ipsec_info_in_metainfo(): "
                    "NULL IPSEC handle" );
  }
  
}
#endif /* FEATURE_SEC_IPSEC */

/*===========================================================================
FUNCTION PS_IP_CALC_PATH_MTU()

DESCRIPTION
  This function calculates the mtu value used for embedded ip stack calls.

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
)
{

  uint16 mtu = 0;
  uint16 default_ip6_mtu = 0;

  mtu = PS_IFACE_GET_MTU(if_ptr);

  /*
   * Path MTU Discovery is right now not implemented.
   * Right now supported values for embedded V4 call:- mtu value <= 1500
   * For embedded V6 call :- mtu value <= 1280
   */

  switch ( ip_addr_type )
  {

    case IFACE_IPV4_ADDR_FAMILY:
      /* Restrict the Max MTU to SYSTEM LIMIT */
      mtu = MIN(mtu, MAX_SYSTEM_MTU);
      break;

    case IFACE_IPV6_ADDR_FAMILY:
      default_ip6_mtu = ps_ip_addr_get_ip6_default_mtu();

      /* 
       * Restrict the NV MTU between MIN MTU & SYSTEM LIMIT 
       * Note: MTU returned by PS_IFACE_GET_MTU() cannot be less than 
       * MIN allowed MTU.
       */
      mtu = MIN(mtu, MIN(default_ip6_mtu, MAX_SYSTEM_MTU));
      break;

    default:
      LOG_MSG_ERROR_1("ps_ip_calc_path_mtu(): INVALID Address Family %d",
                      ip_addr_type);
      ASSERT(0);
      return -1;

  }

  return mtu;

} /* ps_ip_calc_path_mtu */
