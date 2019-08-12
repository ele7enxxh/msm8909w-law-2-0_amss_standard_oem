/*===========================================================================

                                  U D P . C

DESCRIPTION

  The Data Services UDP protocol interface file. This contains all the
  functions used by the data services to use the UDP protocol.

EXTERNALIZED FUNCTIONS
  udp_output()
    Sends UDP packet out to IP, performing pseudo-header checksum
    calculation.

  udp_input()
    Called from IP layer, and hands off UDP packet to UDP protocol layer.

Copyright (c) 1998-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_udp.c_v   1.6   12 Feb 2003 20:35:44   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_udp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/08/13    cx     Replace memcpy and memmove with memscpy and memsmove
12/03/12    ds     Fix to print the socket fd in F3 message when there is 
                   route scope mismatch.
09/21/12    ss     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
08/18/10    sa     Use dsm_insert instead of manipulating data_ptr directly.
07/20/10    sy     Removed critical section.
06/30/10    bq     Make udp_hdr_create public.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
05/22/09    kk     Q6 compiler warning fixes.
03/13/08    pp     Metainfo optimizations.
05/07/07    rt     Added critical section while traversing the UCB queue in
                   udp_input().
12/05/06    msr    Passing sockfd in ps_event_info_type
11/22/06    sv     Added support for IPSEC transport mode.
11/03/06    rt     Modified udp_input() to not use DUP memory pool for the
                   first copy of the data.
11/02/06    mct    Updates for supporting IPv6 local addr binding.
09/18/06    hm     Changed the usage of DSSOCKI_IFACE_GET_ID to PS_IFACE_GET_ID
07/31/06    rt     Changed udp_input() to allow multiple sockets to receive
                   incoming packets. (SO_REUSEADDR)
05/12/06    rt     Changed one of the debug messages from HIGH to LOW.
08/29/05    vp     Pushing port on the error packet dsm item going on errqueue.
08/15/05    mct    Changed pkt_info to look at psuedo_hdr as v6 is not avail.
08/15/05    vp     Check for whether its a sys_sock if iface is in configuring
                   state.
08/15/05    kr     Removed feature DS_PSTATS and added support for updating
                   new stats framework
08/15/05    vp     Support for IP_RECVERR.
08/12/05    sv     optimize metainfo generation.
06/13/05    vp     Function udp_notify().
05/12/05    rt     Fixed lint errors.
04/25/05    vp     Send port unreachable error when dropping packet due to
                   failed netpolicy.
04/18/05    vp     Changes for enforcing netpolicy on input.
03/02/05    vp     Rearrangement of code in udp_input for support for ICMP(v6)
                   error generation.
02/28/05    sv     Removed references to old IPSEC code.
01/27/05    ssh    Changed include file name dsbyte.h to ps_byte.h
12/27/04    ifk    Replaced setting of tracer field with DSM_TOUCH_ITEM
11/10/04    mct    Push the iface id on the packet for IP_RECVIF.
10/13/04    vp     Create udp_hdr_create() function and related changes.
06/11/04    vp     Removed the inclusion of psglobal.h and netuser.h. Included
                   ps_socket_defs.h. Changes for representation of IP addresses
                   as struct ps_in_addr or struct ps_in6_addr. Replaced use of
                   UDP_PTCL with PS_IPPROTO_UDP.
04/30/04    vp     Set ipcb ip_sec_reqd to true if ipsec_hdr_size > 0
03/16/04    mct    Renamed acl_ipfilter_info_type to ip_pkt_info_type.
03/02/04   rc      Modified call to ip_send() according to new signature.
02/10/04   rc      Changed parameters passed to ip_send() according to
                   new signature.
02/04/04    vp     Changed the signature of udp_input function to take
                   ip_pkt_info_type structure, dsm item and offset.
02/04/04    vp     Replaced use of PS_IFACE_IP_V4_ADDR with ps_iface_get_addr
11/11/03    rc/aku Passed additional parameter 'offset' in calls to cksum().
01/22/04    sv     Modified code to accept zero length udp packets.
11/12/03    sv     Removed the check on validity of scb_ptr in udp_output
                   as scb_ptr could have been freed by then.
08/21/03    ss     Convert the source address returned by the ps_iface into
                   network byte order while putting it in the pseudo header
08/18/03    sv     Modified the code to use IPv6 addresses internally.
08/01/03    ss/sv  Restructured the code for IPv6
05/28/03    ss     Modified udp_input() to pass payload length to
                   process_rx_pkt
04/23/03    ss     Modified code to reflect new socket options handling
04/15/03    sv     Changed udp_input to search for a valid socket before
                   calling dssudp_rx_packet
02/12/03    om     Removed routing dependency on ACB.
10/17/02    ss     Uncommented the code in udp_input() to drop malformed pkts
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
09/10/02    aku    Removed global Ip_addr.
08/30/02    om     Updates to use socket's routing cache.
07/31/02    usb    Added include ps_iputil.h
07/31/02    usb    Removed include psi.h
07/21/02    na     Fixed cut & paste extra code
07/16/02    aku    Include dssudp.h and dsbyte.h
07/15/02    na     Added support for FEATURE_DATA_MM.
05/02/02    mvl    File cleanup and added meta_info to output() and changed
                   calls to ip_send() to reflect new prototype
12/18/01    rsl    Updated tracer field.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
07/23/01    pjb    Return type of dsm_pushdown changed.
02/14/01    js     Modified udp_output() to support socket option
                   DSS_IP_TTL.
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth
                   parameter.Made changes to handle dsm_pushdown()
                   returning FALSE.
05/20/00    snn    Removed some code to support the IP Fragmentation.
08/05/99    rc     Added support for UDP debug statistics for Sockets.
07/07/99    ak     In udp_input, if udp length (as spec'd in hdr) does not
                   match ip pkt length (as spec'd in hdr), then dump pkt.
                   Should help guard against fragments (if it passes chksum
                   test).  Also, in udp_output, now allow fragmentation -
                   DF bit is not set.
04/19/99    hcg    Added check to ensure that payload length was same size
                   as the packet length.  Fixed memory leak caused by
                   mis-sized packets being sent from faulty IWF.
04/05/99    hcg    Incorporated code review changes and comments.  Moved
                   definition of maximum UDP payload length to udp.h.
12/09/98   na/hcg  Created module.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "amssassert.h"
#include "dsm.h"
#include "dssocket.h"
#include "ps_udp_io.h"
#include "ps_ip.h"
#include "ps_iputil.h"
#include "msg.h"
#include "err.h"
#include "ps_byte.h"
#include "ps_socket_defs.h"
#include "ps_socki_defs.h"
#include "ps_icmp_error.h"
#include "ps_route.h"
#include "ps_route_scope.h"
#include "ps_iface.h"
#include "ps_stat_udp.h"
#include "ps_socket_event_defs.h"
#include "PS_Sock_IO_IReader.h"
#include "PS_Sock_IO_IWriter.h"
#include "ds_Sock_Def.h"
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

                         LOCAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION UDP_HDR_CREATE()

DESCRIPTION
  This function pushes the UDP header on the DSM item and calculates the
  checksum and fills it in.

PARAMETERS
  dsm_item_type**     - DSM item.
  udp_hdr_type*       - Pointer to UDP header to be filled in.
  pseudo_header_type* - Pseudo header for checksum calcultations.

DEPENDENCIES
  The source and destination port numbers in the passed udp header should be
  in the network order. Length should be in host order.

RETURN VALUE
  boolean - TRUE on success and FALSE on failure.

SIDE EFFECTS
  None.
===========================================================================*/
boolean udp_hdr_create
(
  dsm_item_type      **item_ptr,
  udp_hdr_type       *udp_hdr_ptr,
  pseudo_header_type *ph
)
{
  uint16  checksum;
  uint16  ret;
  uint16  hdr_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(item_ptr == NULL || *item_ptr == NULL || (udp_hdr_ptr == NULL) || (ph == NULL) )
  {
    LOG_MSG_ERROR_0("NULL DSM or UPD or pseudo header: udp_hdr_create()");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Length in UDP header is passed in host order. Change it in network order
    before pushing the header on to the dsm item.
  -------------------------------------------------------------------------*/
  udp_hdr_ptr->length = ps_htons(udp_hdr_ptr->length);
  udp_hdr_ptr->chksum = 0;

  hdr_len = (uint16) sizeof(udp_hdr_type);
  ret = dsm_pushdown_packed(item_ptr,
                            (uint8 *) udp_hdr_ptr,
                            hdr_len,
                            DSM_DS_SMALL_ITEM_POOL);
  if (ret < hdr_len)
  {
    LOG_MSG_ERROR_2("Error in dsm_pushdown_packed() in udp_hdr_create() "
                      "pushed only %d bytes, expected %d bytes", ret, hdr_len);
    return FALSE;
  }

  /*------------------------------------------------------------------------
    All zeros and all ones is equivalent in one's complement arithmetic;
    the spec requires us to change zeros into ones to distinguish an
    all-zero checksum from no checksum at all.
  -------------------------------------------------------------------------*/
  if((checksum = cksum(ph,*item_ptr,ph->length,0)) == 0)
  {
    checksum = 0xffff;
  }
  /*-------------------------------------------------------------------------
    Put the checksum on the outgoing packet in network order.
    Use dsm_insert API to insert bytes into particular location in DSM packet
    chain. Manipulating data_ptr might result in DSM corruption {if the
    data_ptr doesnt have enough space to accomodate checksum}.
  -------------------------------------------------------------------------*/
  // (void)put16(((*item_ptr)->data_ptr + 6),checksum);
  checksum = ps_htons(checksum); /* Host to Network Byte Ordering */
  if (dsm_insert(item_ptr,
                 DSM_DS_SMALL_ITEM_POOL,
                 6,
                 (void*)&checksum, /* 2 byte checksum */
                 sizeof(uint16)) != sizeof(uint16))
  {
    LOG_MSG_ERROR_0("Error in inserting checksum in udp_hdr_create()");
    return FALSE;
  }
  return TRUE;
}/* udp_hdr_create() */


/*===========================================================================
FUNCTION DSSUDP_PROCESS_RX_PKT()

DESCRIPTION
  This function processes the passed UDP packet.

  For socket calls, this function stores the packet in the UDP receive queue
  for the socket, and informs the application if it is waiting for the READ
  event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_udp_process_rx_pkt
(
  struct udp_cb * ucb_ptr,                   /* UDP protocol control block */
  dsm_item_type ** item_ptr,            /* Contains payload and socket info */
  ps_pkt_read_info * read_info_ptr
)
{
   int16 ps_errno;
/*- - - - - - - - -  - - - - - - - - - - - - - - - - - -  - - - - - - - - -*/

  ASSERT(ucb_ptr != NULL);

  //TODO statistics

  (void) Write(ucb_ptr->writer,
               item_ptr,
               read_info_ptr->length,
               (char *) read_info_ptr,
               sizeof(ps_pkt_read_info),
               &ps_errno);

  return;
} /* ps_udp_process_rx_pkt() */


/*===========================================================================
FUNCTION DSSUDP_LOOKUP_CONN()

DESCRIPTION
  Given a connection structure, looks up in the ucb's to see if there is a
  socket expecting packet from that source address.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to UDP_CB if a socket is expecting the packet.
  If not, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
static struct udp_cb *dssudp_lookup_conn
(
  struct connection *conn                    /* connection to be looked up */
)
{
  struct udp_cb *ucb_ptr = NULL;             /* UDP protocol control block */
  struct udp_cb *next_ucb_ptr = NULL;        /* UDP protocol control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Look up the connection and look at IP address and port numbers for
    both source and destination.
  -------------------------------------------------------------------------*/
  ucb_ptr = (struct udp_cb *) q_check(&sock_config_cb.udp_cb_q);
  while (NULL != ucb_ptr)
  {
    ASSERT(SOCKAVAIL != ucb_ptr->sockfd);
    next_ucb_ptr = (struct udp_cb *) q_next(&sock_config_cb.udp_cb_q,
                                            (q_link_type *) ucb_ptr);

    if (NULL == ucb_ptr->scb_ptr)
    {
      ucb_ptr = next_ucb_ptr;
      continue;
    }

    if ((PS_IN6_ARE_ADDR_EQUAL(&ucb_ptr->conn.local.address,
                               &conn->local.address) ||
         PS_IN6_ARE_ADDR_EQUAL(&ucb_ptr->conn.local.address, &ps_in6addr_any)) &&
        ucb_ptr->conn.local.port    == conn->local.port &&
        (PS_IN6_ARE_ADDR_EQUAL(&ucb_ptr->conn.remote.address,
                               &conn->remote.address) ||
         PS_IN6_ARE_ADDR_EQUAL(&ucb_ptr->conn.remote.address, &ps_in6addr_any)) &&
        (ucb_ptr->conn.remote.port == conn->remote.port ||
         ucb_ptr->conn.remote.port == 0 ))
    {
      return( ucb_ptr);
    }

    ucb_ptr = next_ucb_ptr;
  } /* while */

  /*-------------------------------------------------------------------------
      Connection not found.
  -------------------------------------------------------------------------*/
  return( NULL);

} /* dssudp_lookup_conn() */


/*===========================================================================

                         GLOBAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION UDP_OUTPUT()

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
  struct ps_in6_addr    *dest_ip_ptr,               /* Destination IP addr */
  udp_hdr_type          *udp_hdr_ptr,               /* UDP header          */
  dsm_item_type         *item_ptr,                  /* UDP payload         */
  struct udp_cb         *udp_cb_ptr,                /* UDP control block   */
  ps_tx_meta_info_type  *meta_info_ptr              /* ptr to meta info    */
)
{
  pseudo_header_type    ph;                 /* pseudo header               */
  int16                 ps_errno;
  ps_ip_addr_type       if_v6addr; /*for getting address from the interface*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  UDP_INC_STATS(dgram_tx, 1) ;
  UDP_INC_INSTANCE_STATS(udp_cb_ptr, dgram_tx, 1);
  /*-------------------------------------------------------------------------
    Make up pseudo header for checksum calculation
  -------------------------------------------------------------------------*/
  ph.length   = udp_hdr_ptr->length;

  memset(&if_v6addr,0,sizeof(if_v6addr));

  if(PS_IN6_IS_ADDR_UNSPECIFIED(&udp_cb_ptr->ipcb.src_ip_addr))
  {
    if_v6addr.type = IPV6_ADDR;

    ps_iface_get_ipv6_addr(udp_cb_ptr->ipcb.routing_cache,
                           dest_ip_ptr,
                           &if_v6addr);
    memscpy(&ph.source,
            sizeof(ph.source),
            &if_v6addr.addr.v6,
            sizeof(struct ps_in6_addr));
  }
  else
  {
    memscpy(&ph.source,
           sizeof(ph.source),
           &udp_cb_ptr->ipcb.src_ip_addr,
           sizeof(struct ps_in6_addr));
  }

  ph.dest     = *dest_ip_ptr;
  ph.protocol = PS_IPPROTO_UDP;

  if(udp_hdr_create(&item_ptr,udp_hdr_ptr,&ph) == FALSE)
  {
    LOG_MSG_ERROR_0("Error in udp_hdr_create(), freeing packet.");
    UDP_INC_STATS(dgram_dropped_tx, 1) ;
    dsm_free_packet(&item_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return;
  }

  DSM_TOUCH_ITEM(item_ptr);

  UDP_INC_STATS(bytes_tx, (ph.length - sizeof(udp_hdr_type)) ) ;
  UDP_INC_INSTANCE_STATS(udp_cb_ptr,bytes_tx,
                         (ph.length-sizeof(udp_hdr_type)));
  (void)ip_send( &udp_cb_ptr->ipcb,
                 ph.dest,
                 PS_IPPROTO_UDP,
                 item_ptr,
                 ph.length,
                 meta_info_ptr,
                 &ps_errno);

}  /* udp_output */


/*===========================================================================
FUNCTION UDP_INPUT()

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
  ip_pkt_info_type*  ip_pkt_info_ptr,         /* IP Packet Info            */
  struct dsm_item_s        *data_ptr,         /* Data field, if any        */
  uint16                   offset             /* Offset of the UDP packet  */
)
{
  word               calc_checksum;                 /* Calculated checksum */
  udp_hdr_type       udp_hdr;                     /* The UDP header fields */
  word               udp_payload_len;   /* payload length, from udp header */
  uint32             dsm_payload_len;   /* payload length from the dsm pkt */
  struct udp_cb    * ucb_ptr;                /* UDP protocol control block */
  struct connection  conn;               /* temp connection variable       */
  uint32             iface_id=0;
  struct udp_cb    * next_ucb_ptr    = NULL;  /* UDP protocol control block */
  boolean            is_port_unreachable = TRUE;  /*TRUE  - Send ICMP Error.
                                               FALSE - Don't send ICMP Err*/
  dsm_item_type    * dup_data_ptr    = NULL;
  boolean            hdr_processed_flag = FALSE;/*TRUE - Processing Complete
                                                  FALSE - Not Complete.*/
  ps_pkt_read_info   read_info;
  uint32             iface_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  UDP_INC_STATS(dgram_rx,1);
  /*-------------------------------------------------------------------------
    Extract the udp header from the UDP packet. Check to see if the header
    sent was accurate.
  -------------------------------------------------------------------------*/
  if ( dsm_extract( data_ptr, offset, &udp_hdr, sizeof( udp_hdr_type))
       != sizeof( udp_hdr_type))
  {
      LOG_MSG_ERROR_0( "UDP packet has an invalid header");
      UDP_INC_STATS(dgram_dropped_rx,1 );
      dsm_free_packet( &data_ptr);
      return;
  }

  /*-------------------------------------------------------------------------
    Check if the checksum field is all zeros. If it is zeros, the sender did
    not calculate the checksum, so we do not verify it.
    Note, there is no need to convert the checksum to host format because
    we are checking it against symmetric values ( 0 and FFFF).
  -------------------------------------------------------------------------*/
  if ( udp_hdr.chksum != 0)
  {

    /*-----------------------------------------------------------------------
      Calculate UDP checksum. Pushdown calculated
      UDP checksum and destination IP address for use in next layer
      (DevRevPDU) then queue and tell UI a UP client UDP packet is
       available.
    -----------------------------------------------------------------------*/
    calc_checksum = cksum( &ip_pkt_info_ptr->pseudo_hdr,
                           data_ptr,
                           ip_pkt_info_ptr->pseudo_hdr.length,
                           offset);

    if ( calc_checksum != 0)
    {
      LOG_MSG_INFO1_0( "Found bad UDP checksum");
      UDP_INC_STATS(bad_chksum, 1);
      UDP_INC_STATS(dgram_dropped_rx,1 );
      dsm_free_packet( &data_ptr);
      return;
     }

  } /* udp_hdr cksum != 0 */

  offset += sizeof(udp_hdr_type);

  /*-------------------------------------------------------------------------
    Check the length field of the udp header. Check if the length is more
    than the max. payload length. Compare the length specified
    in the header against the actual length in the payload. If the header
    length is more than the actual length, dump the packet. Note, if the
    header length was less than the actual payload then it is OK to send
    the packet to the UP layer.
  -------------------------------------------------------------------------*/
  udp_payload_len = get16((byte *) &(udp_hdr.length)) - sizeof(udp_hdr_type);

  dsm_payload_len = dsm_length_packet( data_ptr ) - offset;

  /*-------------------------------------------------------------------------
    Drop the packet if it is malformed.
  -------------------------------------------------------------------------*/
  if ( udp_payload_len > dsm_payload_len )
  {
    LOG_MSG_ERROR_0(" Length specified in UDP hdr is invalid");
    UDP_INC_STATS(bad_len, 1) ;
    UDP_INC_STATS(dgram_dropped_rx, 1);
    dsm_free_packet( &data_ptr);
    return;
  }

  UDP_INC_STATS(bytes_rx, udp_payload_len);
  /*-------------------------------------------------------------------------
    Ensure that the actual payload length is the size of the packet.  If
    not, trim the packet to the correct length.  This prevents memory leaks
    caused by faulty IWF's that may incorrectly insert additional bytes
    due to their own memory leaks.
  -------------------------------------------------------------------------*/
  if ( udp_payload_len < dsm_payload_len )
  {
    dsm_trim_packet( &data_ptr, (udp_payload_len + offset));
  }

  conn.local.address  = ip_pkt_info_ptr->pseudo_hdr.dest;
  conn.local.port     = (udp_hdr.dest);
  conn.remote.address = ip_pkt_info_ptr->pseudo_hdr.source;
  conn.remote.port    = (udp_hdr.source);

  iface_id = PS_IFACE_GET_ID((ps_iface_type *)(ip_pkt_info_ptr->if_ptr));


  read_info.length = udp_payload_len;
  read_info.ifaceId = iface_id;
  read_info.v6RemoteSockAddr.ps_sin6_family = DSS_AF_INET6;
  read_info.v6RemoteSockAddr.ps_sin6_port = udp_hdr.source;
  memscpy (&(read_info.v6RemoteSockAddr.ps_sin6_addr),
          sizeof(read_info.v6RemoteSockAddr.ps_sin6_addr),
          &(conn.remote.address),
          sizeof(struct ps_in6_addr));

  /*-------------------------------------------------------------------------
    Traverse the UCB queue and provide a copy of the incoming packet to each
    matching connection. Multicast packets can be received by multiple
    connections bound to the multicast address.
  -------------------------------------------------------------------------*/  
  ucb_ptr = (struct udp_cb *) q_check(&sock_config_cb.udp_cb_q);
  while (NULL != ucb_ptr)
  {
    ASSERT(SOCKAVAIL != ucb_ptr->sockfd);
    next_ucb_ptr = (struct udp_cb *) q_next(&sock_config_cb.udp_cb_q,
                                            (q_link_type *) ucb_ptr);

    if (NULL == ucb_ptr->scb_ptr)
    {
      ucb_ptr = next_ucb_ptr;
      continue;
    }

    if ((PS_IN6_ARE_ADDR_EQUAL(&ucb_ptr->conn.local.address,
                            &conn.local.address) ||
         PS_IN6_ARE_ADDR_EQUAL(&ucb_ptr->conn.local.address, &ps_in6addr_any)) &&
        ucb_ptr->conn.local.port    == conn.local.port &&
        (PS_IN6_ARE_ADDR_EQUAL(&ucb_ptr->conn.remote.address,
                              &conn.remote.address) ||
         PS_IN6_ARE_ADDR_EQUAL(&ucb_ptr->conn.remote.address, &ps_in6addr_any)) &&
        (ucb_ptr->conn.remote.port == conn.remote.port ||
         ucb_ptr->conn.remote.port == 0 ))
    {
      /*---------------------------------------------------------------------
        Enforce netpolicy on input packet.
      ---------------------------------------------------------------------*/
      iface_index =
        PS_IFACE_GET_IFACE_INDEX((ps_iface_type *) ip_pkt_info_ptr->if_ptr);

      if( iface_index == PS_IFACE_INVALID_IFACE_INDEX ||
          !ps_route_scope_is_iface_idx_set( &ucb_ptr->ipcb.route_scope,
                                            iface_index )
        )
      {
        LOG_MSG_INFO1_4("udp_input - route scope mismatch: 0x%x 0x%x, iface_idx %d, sockfd %d",
                         ucb_ptr->ipcb.route_scope.if_map[0], 
                         ucb_ptr->ipcb.route_scope.if_map[1], 
                         iface_index,
                          ucb_ptr->sockfd);
        ucb_ptr = next_ucb_ptr;
        continue;
      }

      /*---------------------------------------------------------------------
        Connection Found. So there is no need to send icmp unreachable error
        for this incoming packet.
      ---------------------------------------------------------------------*/
      is_port_unreachable = FALSE;

      UDP_INC_INSTANCE_STATS(ucb_ptr,dgram_rx, 1);
      UDP_INC_INSTANCE_STATS(ucb_ptr,bytes_rx, udp_payload_len);

      /*---------------------------------------------------------------------
        hdr_processed_flag would identify the part of the code that needs to
        be processed only once for the received packet even though there are
        multiple sockets that are going to receive the packet copy.
      ---------------------------------------------------------------------*/
      if(hdr_processed_flag != TRUE)
      {
        /*-------------------------------------------------------------------
          Remove the ip header and udp_hdr from the data_ptr.
        -------------------------------------------------------------------*/
        if( dsm_pullup(&data_ptr, NULL, offset) != offset )
        {          
          LOG_MSG_INFO2_0( "Error in dsm_pullup() in udp_input()" );
          UDP_INC_STATS(dgram_dropped_rx, 1) ;
          dsm_free_packet( &data_ptr);
          return;
        }
        hdr_processed_flag = TRUE;
      }

      /*-------------------------------------------------------------------
        In the case of multicast there could be other UCBs so dup it and
        continue
      -------------------------------------------------------------------*/
      if (PS_IN6_IS_V4_MAPPED_V6_ADDR_MULTICAST(&conn.local.address) ||
          PS_IN6_IS_ADDR_MULTICAST(&conn.local.address))
      {
        /*-------------------------------------------------------------------
          Dup a copy of the packet that would be enqueued for this UCB.
        -------------------------------------------------------------------*/
        if (dsm_dup_packet(&dup_data_ptr, data_ptr,0,
                           (uint16)udp_payload_len) != udp_payload_len)
        {          
          LOG_MSG_ERROR_0("Not able to dup a packet");
          UDP_INC_STATS(dgram_dropped_rx, 1) ;
          dsm_free_packet( &data_ptr);
          return;
        }
        ps_udp_process_rx_pkt(ucb_ptr, &dup_data_ptr, &read_info);
      }
      else
      {
        ps_udp_process_rx_pkt(ucb_ptr, &data_ptr, &read_info);
        break;
      }
    }

    ucb_ptr = next_ucb_ptr;
  } /* while */  

  /*-------------------------------------------------------------------------
     If sen_icmp_error==TRUE that means that there was no connection found
     for the incoming packet. Send ICMP port unreachable error.
  -------------------------------------------------------------------------*/
  if(is_port_unreachable == TRUE)
  {
    LOG_MSG_INFO3_0("Dropping pkt. Found ucb_ptr as null, no connection");
    UDP_INC_STATS(no_port,1);
    UDP_INC_STATS(dgram_dropped_rx,1);
    /*-----------------------------------------------------------------------
      Send ICMP port unreachable error.
    -----------------------------------------------------------------------*/
    icmp_send_unreach_error (&data_ptr, ip_pkt_info_ptr);
    return;
  }

  if (data_ptr)
  {
    dsm_free_packet( &data_ptr);
  }
  return;

} /* udp_input() */


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
  struct ps_in6_addr          - V6 address of the source of ICMP error.
  dsm_item_type *             - Offending IP packet.

RETURN VALUE
  None.

SIDE EFFECTS
  Frees the DSM item.
===========================================================================*/
void udp_notify
(
  uint16                         ps_errno,
  struct ps_sock_extended_err  * sock_err,
  struct connection            * conn,
  struct ps_in6_addr             v6_offender,
  dsm_item_type                * bpp
)
{
  struct udp_cb              * ucb_ptr;
  ps_icmp_errq_meta_info       icmp_errq_meta_info;
  ps_socket_event_info_type    info;
  int16                        tmp_ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ucb_ptr = dssudp_lookup_conn(conn);

  if (ucb_ptr != NULL)
  {
    if (0 != (ucb_ptr->ipcb.options_mask &
               ((uint64) 1 << ds_Sock_OptName_QDS_IP_RECVERR)) ||
        0 != (ucb_ptr->ipcb.options_mask &
               ((uint64)1 << ds_Sock_OptName_QDS_IPV6_RECVERR)))
    {
      icmp_errq_meta_info.v6_offender  = v6_offender;
      icmp_errq_meta_info.extended_err = *sock_err;

      icmp_errq_meta_info.original_dgram_dest_addr.ps_sin6_family = DSS_AF_INET6;
      icmp_errq_meta_info.original_dgram_dest_addr.ps_sin6_port =
        conn->remote.port;
      memscpy(&(icmp_errq_meta_info.original_dgram_dest_addr.ps_sin6_addr),
             sizeof(icmp_errq_meta_info.original_dgram_dest_addr.ps_sin6_addr),
             &(conn->remote.address),
             sizeof(struct ps_in6_addr));

      /*---------------------------------------------------------------------
        Enqueue the item on to the errq. If will exceed the count of max err
        pkts allowed then dequeue the oldest err pkt out.
      ---------------------------------------------------------------------*/
#if 0
      if (GetPktCnt(ucb_ptr->errq_ptr) >= UDP_MAX_ERRQ_PKTS)
      {
        (void) Read(ucb_ptr->errq_ptr, &item_ptr, 0, 0, &tmp_ps_errno);
        dsm_free_packet(&item_ptr);
      }
#endif

      (void) Write(ucb_ptr->errq_ptr,
                   &bpp,
                   dsm_length_packet(bpp),
                   (char *) &icmp_errq_meta_info,
                   sizeof(ps_icmp_errq_meta_info),
                   &tmp_ps_errno);
    }
    else
    {
      dsm_free_packet(&bpp);
    }

    info.scb_ptr         = ucb_ptr->scb_ptr;
    info.data.icmp_error = ps_errno;

    socket_platform_event_cback_f_ptr(PS_SOCKET_ICMP_ERROR_EV, &info);
  }
  else
  {
    dsm_free_packet(&bpp);
  }

  return;
} /* udp_notify() */

