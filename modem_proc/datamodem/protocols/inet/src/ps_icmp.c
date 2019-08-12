/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                 I C M P . C

GENERAL DESCRIPTION
 Internet Control Message Protocol (ICMP)
  Copyright 1991 Phil Karn, KA9Q


EXTERNALIZED FUNCTIONS
  icmp_input()
   This function Process an incoming ICMP packet. The only ICMP messages
   that are processed are 1) Echo, 2) Info Request, and 3) Timestamp. Each
   of these messages will be echoed back to the ICMP peer in the base
   station. Other ICMP messages will be handled by dssicmp_ps_input() in
   dssicmp.c

  icmp_output()
    This function does the following:
      1. Converts from host to network all the elements of the ICMP header
         and inserts the ICMP header in the ICMP packet.
      2. Creates the pseudo-header and calculates ICMP checksum.
      3. Sends ICMP packet down to IP.

  icmp_get_stats()
    This function will copy ICMP stats (enumerated in icmp_stats_enum)
    into the given array.

  icmp_reset_stats()
    This function will reset those ICMP stats which can be reported
    with icmp_get_stats.


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_icmp.c_v   1.5   12 Feb 2003 20:35:30   omichael  $
 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_icmp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/10/14    ssh    IP Translation Engine 464XLAT 
11/22/13    ds     Adding support to read the ICMP reply rate parameters 
                   from NV item file.
12/12/12    ds     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
03/14/12    ss     Fix to allow zero payload ICMP packets.
12/23/10    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
08/12/10    pp     Use dsm_insert instead of manipulating data_ptr directly.
05/23/10    bq     Change to dsm_pushdown_packed.
01/19/10    ss     Lint fixes.
01/15/10    ss     Added echo reply case in icmp_hdr_parse().
09/30/09    ss     IP hdr init  before parsing in icmp_get_conn_struct().
05/22/09    kk     Q6 compiler warning fixes.
09/12/08    pp     Metainfo optimizations.
04/24/06    mct    Populating routing cache in dssicmp_write().
04/19/06    rt     Removed the field ip_sec_req from IP control block.
10/24/05    vp     Restricting the ICMP reply if iface state = CONFIGURING
10/07/05    vp     Changed pushdown_packed to pushdown in icmp_hdr_create().
08/24/05    vp     Removed incorrect update of offset in get_conn_struct().
08/15/05    vp     Change due to change in signature of dssicmp_ps_input().
06/20/05    kr     added code for ICMPV4 stats; removed feature DS_PSTATS
08/15/05    vp     Support for IP_RECVERR.
06/13/05    vp     Support for reporting ICMP errors to apps.
03/02/05    vp     New function icmp_hdr_create() in place of icmpi_hton().
                   Changes for ICMP error message generation.
01/17/05    ks     memsetting IP control block in icmp_input.
12/27/04    ifk    Replaced setting of tracer field with DSM_TOUCH_ITEM
10/13/04    vp     Removed the byte order conversion of address before call
                   to ip4_send as it is expected in network order.
09/07/04    sv     Fixed compiler warning in icmp_output.
07/16/04    vp     Declaration of icmpv4_token_bucket. Added function
                   icmpi_rate_limit_on_type() for ICMP rate limiting.
                   Rearranged and added code to icmp_input and icmp_output for
                   ICMP rate limiting.
06/11/04    vp     Removed inclusion of psglobal.h and netuser.h and added
                   inclusion of ps_in.h. Replaced use of ICMP_PTCL with
                   PS_IPPROTO_ICMP and changes due to representation of IP
                   address as struct ps_in_addr
04/30/04    vp     Added dsm_pullup for removal of ip header in icmp_input.
04/30/04    vp     Added code to see if meta_info_ptr is not null in
                   icmp_output and removed the unnecessary ipaddress lookup
03/30/04    vp     Merged changes from June04Dev_05.00.00 PS branch.
03/16/04    mct    Renamed acl_ipfilter_info_type to ip_pkt_info_type.
03/02/04    rc     Changed icmp_input() and icmp_output() to call ip4_send()
                   with new signature.
02/10/04    rc     Changed icmp_input() and icmp_output() to call ip4_send()
                   in accordance with new signature.
02/04/04    vp     Changed the signature of icmp_input function to take
                   ip_pkt_info_type structure, dsm item and offset.
11/11/03   aku/rc  Passed parameter 'offset' in calls to cksum().
11/12/03    sv     Removed the check on validity of scb_ptr in udp_output
                   as scb_ptr could have been freed by then.
09/11/03    sv     Modified check in icmpi_ntoh to return error if the entire
                   ICMP header is not pushed onto the dsm_item.
08/21/03    ss     Pass meta_info->routing_cache to ip_send
08/18/03    sv     Modified the code to use IPv6 addresses internally.
08/08/03    ss/sv  Restructured the socket code.
04/23/03    ss     Modified code to reflect new socket options handling
02/12/03    om     Use SCB's routing cache only.
11/25/02    usb    Added routing cache for icmp sockets.
09/10/02    aku    Removed global Ip_addr.
08/30/02    om     Updates to use incoming iface for replies, routing
                   for outgoing requests.
07/31/02    usb    Added include ps_iputil.h
07/31/02    usb    Renamed psmisc.h to ps_utils.h
05/02/02    mvl    File cleanup and changed calls to ip_send() to reflect new
                   prototype
02/12/02    dwp    Cleanup header inclusion(s) for non DS builds.
12/21/01    dwp    Add or !FEATURE_DATA_PS to some DSM tracer settings. This
                   is temporary until 5200 syncs up to latest DSM.
12/18/01    rsl    Updated tracer field.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
07/30/01    mvl    Modified a message to give info on unrecognized ICMP msgs
07/23/01    pjb    Return type of dsm_pushdown changed.
07/13/01    mvl    Fixed compile bug so will compile with sockets feature off
06/04/01  jd/mvl   resolved IP address inconsistency, temp. fix removed
05/10/01    jd     Added temporary fix to get correct destination IP addr in
                   icmp_output
05/10/01    jd     Removed extra call to ip_send() in icmp_output() by making
                   ttl parameter a variable.
03/27/01    js     Updated module to support arbitrary ICMP message types.
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth
                   parameter.Made changes to handle dsm_pushdown()
                   returning FALSE.
                   Handled the case when icmpi_hton()() returns NULL.
01/27/98    ldg    Removed the DS_SLIM switch.
11/06/97    ldg    Removed 'extern' keyword from function definition.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
06/12/97    ldg    Added new FEATURE_DS_PSTATS
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "ps_utils.h"
#include "dsm.h"
#include "ps_iface.h"
#include "ps_ip4.h"
#include "ps_in.h"
#include "ps_iputil.h"
#include "ps_tcp_hdr.h"
#include "ps_udp_io.h"
#include "ps_icmp.h"
#include "dssicmp.h"
#include "ps_udp.h"
#include "ps_byte.h"
#include "msg.h"
#include "err.h"
#include "dssocket.h"
#include "amssassert.h"
#include <memory.h>
#include "dserrno.h"
#include "ps_stat_icmpv4.h"
#include "token_bucket.h"
#include "ds_Utils_DebugMsg.h"



/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Token bucket to rate limit ICMPv4 messages.
---------------------------------------------------------------------------*/
static token_bucket_type icmpv4_token_bucket;

/*--------------------------------------------------------------------------
  Variables to store the value of NV item value for ICMP reply rate.
-------------------------------------------------------------------------*/
static uint16 icmp4_tokens_per_second;
static uint16 icmp4_max_tokens;

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION ICMP_GET_ERRNO()

DESCRIPTION
  This function gets the errno associated with the ICMP error received.

DEPENDENCIES
  None.

PARAMETERS
  struct icmp_hdr - ICMP common header containing type and code.

RETURN VALUE
  errno

SIDE EFFECTS
  None.
===========================================================================*/
static uint16 icmp_get_errno
(
  struct icmp_hdr _icmp_hdr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(_icmp_hdr.type)
  {
    case ICMP_DEST_UNREACH:
      switch(_icmp_hdr.code)
      {
        case ICMP_HOST_UNREACH:
        case ICMP_DEST_HOST_ADMIN_PROHIB:
        case ICMP_HOST_UNREACH_TOS:
        case ICMP_COMM_ADMIN_PROHIB:
        case ICMP_HOST_PREC_VIOLATION:
        case ICMP_PREC_CUTOFF:
          return DS_EHOSTUNREACH;

        case ICMP_NET_UNREACH:
        case ICMP_DEST_NET_UNKNOWN:
        case ICMP_DEST_NET_ADMIN_PROHIB:
        case ICMP_NET_UNREACH_TOS:
          return DS_ENETUNREACH;

        case ICMP_PROTO_UNREACH:
          return DS_ENOPROTOOPT;

        case ICMP_PORT_UNREACH:
          return DS_ECONNREFUSED;

        case ICMP_FRAG_NEEDED:
          return DS_EMSGSIZE;

        case ICMP_SRC_ROUTE_FAIL:
          return DS_EOPNOTSUPP;

        case ICMP_DEST_HOST_UNKNOWN:
          return DS_EHOSTDOWN;

        case ICMP_SRC_HOST_OSOLATED:
          return DS_ENONET;

        default:
          return DS_EHOSTUNREACH;

      }

    case ICMP_TIME_EXCEED:
      return DS_EHOSTUNREACH;

    case ICMP_PARAM_PROB:
      return DS_EPROTO;

    default:
      LOG_MSG_INFO1_0("Unknown ICMP error");
      return DS_EHOSTUNREACH;
  }
}/* icmp_get_errno() */
/*===========================================================================

FUNCTION ICMP_GET_CONN_STRUCT()

DESCRIPTION
  This function gets the connection structure and transport protocol of the
  ICMP error packet.

DEPENDENCIES
  None.

PARAMETERS
  dsm_item_type*  - DSM item for the packet.
  uint16 *        - offset in DSM item for the offending packet.
  uint8 *         - Transport protocol of the offending packet. (Return param)
  struct connection* - Connection structure. (Return Param)

RETURN VALUE
  TRUE  - On success.
  FALSE - On failure.

SIDE EFFECTS
  Offset gets updated to the end of IP header.
===========================================================================*/
static boolean icmp_get_conn_struct
(
  dsm_item_type     *bpp,
  uint16            *offset,
  uint8             *protocol,
  struct connection *conn
)
{
  struct ip iph;
  uint8  buf[4];
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(bpp == NULL || protocol == NULL || conn == NULL)
  {
    LOG_MSG_INFO1_0("Null parameter to icmp_get_conn_struct()");
    return FALSE;
  }
  /* Initialize ip header */
  memset((void *)&iph, 0, sizeof(iph));

  if(FALSE == ps_ip4_hdr_parse(bpp, offset, &iph))
  {
    LOG_MSG_INFO1_0("Error in IP header parse");
    return FALSE;
  }

  *protocol = iph.protocol;
  /**/
  PS_IN6_GET_V4_MAPPED_V6_FROM_V4(&conn->local.address, iph.source.ps_s_addr);
  PS_IN6_GET_V4_MAPPED_V6_FROM_V4(&conn->remote.address, iph.dest.ps_s_addr);
  /**/

  /*-------------------------------------------------------------------------
    Extract the port info.
  -------------------------------------------------------------------------*/
  if(dsm_extract(bpp, *offset, buf, 2*sizeof(uint16)) != (2*sizeof(uint16)))
  {
    LOG_MSG_INFO1_0("Error in dsm_extract()");
    return FALSE;
  }

  conn->local.port  = *((uint16*)&buf[0]);
  conn->remote.port = *((uint16*)&buf[2]);

  return TRUE;
}/* icmp_get_conn_struct() */

/*===========================================================================

FUNCTION ICMP_PROCESS_ERROR()

DESCRIPTION
  This function processes ICMP error messages to deliver them to applications
  or take any other appropriate steps.

DEPENDENCIES
  ICMP code and type info is present in the passed icmp structure.

PARAMETERS
  struct icmp*    - ICMP strucure with ICMP type and code information.
  struct ps_in_addr  - IP address of source of ICMP error.
  dsm_item_type** - DSM item for input packet.

RETURN VALUE
  None.

SIDE EFFECTS
  Frees the DSM item.
===========================================================================*/
static void icmp_process_error
(
  struct icmp     * icmp_ptr,
  struct ps_in_addr offender,
  dsm_item_type  ** bpp
)
{
  struct connection           conn;
  uint8                       protocol;
  uint16                      offset = 0;
  uint16                      icmp_errno = 0;
  struct ps_sock_extended_err sock_err;
  struct ps_in6_addr          v6_offender;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(icmp_ptr == NULL || bpp == NULL || *bpp == NULL)
  {
    LOG_MSG_INFO1_0("NULL parameter passed to icmp_process_error()");
    if(bpp && *bpp)
    {
      dsm_free_packet(bpp);
    }
    return;
  }

  LOG_MSG_INFO1_1("icmp_process_error: Code received %d", icmp_ptr->info.code);
  switch(icmp_ptr->info.type)
  {
    case ICMP_DEST_UNREACH :
      ICMPV4_INC_STATS(dest_unreach_rx, 1) ;
      if(icmp_ptr->info.code >= (uint8)ICMP_UNREACH_MAX)
      {
        ICMPV4_INC_STATS(bad_code_rx, 1);
        dsm_free_packet(bpp);
        return;
      }

      if(icmp_ptr->info.code == (uint8)ICMP_FRAG_NEEDED)
      {
        /*-------------------------------------------------------------------
          Extract the MTU of the path.
          -------------------------------------------------------------------*/
        if(dsm_extract(*bpp, sizeof(struct icmp_hdr), &icmp_ptr->arg.mtu,
                       sizeof(struct mtu_s)) != sizeof(struct mtu_s))
        {
          LOG_MSG_INFO1_0("Error in dsm_extract()");
          dsm_free_packet(bpp);
          return;
        }
        icmp_ptr->arg.mtu.mtu = ps_ntohs(icmp_ptr->arg.mtu.mtu);
      }
      break;

    case ICMP_PARAM_PROB :
      ICMPV4_INC_STATS(parm_probs_rx, 1) ;
      if(icmp_ptr->info.code >= (uint8)ICMP_PARAM_MAX)
      {
        ICMPV4_INC_STATS(bad_code_rx, 1);
        dsm_free_packet(bpp);
        return;
      }

      if(!dsm_peek_byte(*bpp,sizeof(struct icmp_hdr),&icmp_ptr->arg.pointer))
      {
        LOG_MSG_INFO1_0("Error in dsm_peek_byte()");
        dsm_free_packet(bpp);
        return;
      }
      break;

    case ICMP_TIME_EXCEED:
      ICMPV4_INC_STATS(time_exceeds_rx, 1) ;
      if(icmp_ptr->info.code >= (uint8)ICMP_EXCEED_MAX)
      {
        ICMPV4_INC_STATS(bad_code_rx, 1);
        dsm_free_packet(bpp);
        return;
      }
      break;

    case ICMP_QUENCH:
      ICMPV4_INC_STATS(src_quench_rx, 1) ;
      if(icmp_ptr->info.code != 0)
      {
        ICMPV4_INC_STATS(bad_code_rx, 1);
        dsm_free_packet(bpp);
        return;
      }
      break;

    default:
      dsm_free_packet(bpp);
      return;
  }

  (void) dsm_pullup(bpp, NULL,
                    (sizeof(struct icmp_hdr) + sizeof(union icmp_args)));

  if(icmp_get_conn_struct(*bpp, &offset, &protocol, &conn) == FALSE)
  {
    dsm_free_packet(bpp);
    return;
  }

  if(icmp_ptr->info.type != (uint8)ICMP_QUENCH)
  {
    icmp_errno = icmp_get_errno(icmp_ptr->info);
  }

  switch(protocol)
  {
    case PS_IPPROTO_TCP:

      dsm_free_packet(bpp);

      if(icmp_ptr->info.type == (uint8)ICMP_QUENCH)
      {
        /*-------------------------------------------------------------------
          Flow control. Slow down.
        -------------------------------------------------------------------*/
        tcp_quench(&conn);
      }
      else
      {
        /*-------------------------------------------------------------------
          Report the error to the application.
        -------------------------------------------------------------------*/
        tcp_notify(icmp_errno, &conn);
      }
      break;

    case PS_IPPROTO_UDP:

      memset(&sock_err, 0, sizeof(struct ps_sock_extended_err));
      sock_err.ee_errno  = icmp_errno;
      sock_err.ee_origin = PS_SO_EE_ORIGIN_ICMP;
      sock_err.ee_type   = icmp_ptr->info.type;
      sock_err.ee_code   = icmp_ptr->info.code;
      sock_err.ee_info   = icmp_ptr->arg.mtu.mtu;

      /*---------------------------------------------------------------------
        Remove the IP and UDP header before giving the dsm item to udp_notify
        offset contains the length of IP header.
      ---------------------------------------------------------------------*/
      (void) dsm_pullup(bpp, NULL, (offset + sizeof(udp_hdr_type)));

      /**/
      PS_IN6_GET_V4_MAPPED_V6_FROM_V4(&v6_offender, offender.ps_s_addr);
      /**/

      udp_notify(icmp_errno, &sock_err, &conn, v6_offender, *bpp);
      break;

    default:
      LOG_MSG_INFO1_0("Unknown transport protocol");
      dsm_free_packet(bpp);
      break;
  }

}/* icmp_process_error() */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      PUBLIC FUNCTION DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================

FUNCTION ICMP_RATE_LIMIT_ON_TYPE()

DESCRIPTION
  This function rate limits outgoing ICMP messages based on type of message.

DEPENDENCIES
  icmpv4_token_bucket is initialized.

RETURN VALUE
  0 - Rate limit the packet (Drop the packet - no token)
  1 - Message can be sent.

SIDE EFFECTS
  None
===========================================================================*/
boolean icmp_rate_limit_on_type
(
  ps_icmp_msg_enum_type type                          /* ICMP message type */
)
{
  switch(type)
  {
    case ICMP_ECHO_REPLY :
    case ICMP_DEST_UNREACH :
    case ICMP_QUENCH :
    case ICMP_REDIRECT :
    case ICMP_RTR_ADV :
    case ICMP_TIME_REPLY :
    case ICMP_INFO_REPLY :
    case ICMP_ADDR_MASK_REPLY :
    case ICMP_FQDN_REPLY :
    case ICMP_TIME_EXCEED :
    case ICMP_PARAM_PROB :
      /*---------------------------------------------------------------------
        Rate limit these messages. See in the in icmpv4_token_bucket whether
        we get any token to send the message.
      ---------------------------------------------------------------------*/
      return token_bucket_get_token(&icmpv4_token_bucket);

    case ICMP_ECHO_REQ :
    case ICMP_RTR_SOL :
    case ICMP_TIMESTAMP :
    case ICMP_INFO_RQST :
    case ICMP_ADDR_MASK :
    case ICMP_FQDN_REQUEST :
      /*---------------------------------------------------------------------
        No rate limiting on these messages.
      ---------------------------------------------------------------------*/
      return 1;

    default :
      LOG_MSG_INFO1_1("Invalid ICMP message type %d", type);
      return 0;
  }
}/* icmp_rate_limit_on_type */

/*===========================================================================

FUNCTION ICMP_HDR_PARSE()

DESCRIPTION
 This function will parse ICMP header information.

DEPENDENCIES
  None

RETURN VALUE
  0 if the header extraction was successful, else -1

SIDE EFFECTS
  None
===========================================================================*/

int icmp_hdr_parse
(
  dsm_item_type **bpp,
  uint16        *offset,
  struct icmp   *icmp_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Extract the generic icmp header.
  -------------------------------------------------------------------------*/
  if (dsm_extract(*bpp, 0, (byte *) &icmp_ptr->info,
                  sizeof( struct icmp_hdr)) != sizeof( struct icmp_hdr))
  {
    LOG_MSG_ERROR_0("Error in dsm_extract()");
    return -1;
  }

  *offset += sizeof(struct icmp_hdr);

  /*-------------------------------------------------------------------------
    Process arguments based on type.
  -------------------------------------------------------------------------*/
  switch( icmp_ptr->info.type)
  {
    case ICMP_ECHO_REQ:
    /*-------------------------------------------------------------------------
      We need to extract the ICMP header information even in case of echo 
      reply in order to deliver the packet to proper ping session when 
      multiple sessions are ongoing.
    -------------------------------------------------------------------------*/
    case ICMP_ECHO_REPLY:
    case ICMP_TIMESTAMP:
    case ICMP_INFO_RQST:

      if (dsm_extract(*bpp, *offset, (byte *) &icmp_ptr->arg.echo,
                      sizeof(icmp_ptr->arg.echo)) != sizeof(icmp_ptr->arg.echo))
      {
        LOG_MSG_ERROR_0("Error in dsm_extract()");
        return -1;
      }

      *offset += sizeof(icmp_ptr->arg.echo);

      if(icmp_ptr->info.type == (uint8)ICMP_TIMESTAMP)
      {
        if (dsm_extract(*bpp, *offset, (byte *) &icmp_ptr->t_stamp,
                        sizeof(struct icmp_timestamp)) !=
            sizeof(struct icmp_timestamp))
        {
          LOG_MSG_ERROR_0("Error in dsm_extract()");
          return -1;
        }

        *offset += sizeof(struct icmp_timestamp);

        icmp_ptr->t_stamp.rx_timestamp   = (msclock() | 0x80000000U);
        icmp_ptr->t_stamp.rx_timestamp   = ps_htonl(icmp_ptr->t_stamp.rx_timestamp);
      }
      break;

    default:
      LOG_MSG_INFO2_1( "Other ICMP msg type %d rx'd", icmp_ptr->info.type);
      break;
  }

  /*------------------------------------------------------------------------
     Everything was successful! Terminate normally.
  ------------------------------------------------------------------------*/
  return 0;
} /* icmp_hdr_parse() */

/*===========================================================================

FUNCTION ICMP_HDR_CREATE()

DESCRIPTION
  This function will generate an ICMP header in network byte order,link data,
  and compute the checksum.
  The ICMP messages have the following general format:

       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |     Type      |     Code      |          Checksum             |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                                                               |
       +                         Message Body                          +
       |                                                               |

PARAMETERS
  struct icmp *           - ICMP header for the message to be sent.
  struct dsm_item_type ** - Data portion of the ICMP message.
  pseudo_header_type *    - Pseudo header (to get the length of ICMP packet)

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Success
  FALSE - Failure

SIDE EFFECTS
  None
===========================================================================*/
boolean icmp_hdr_create
(
  struct icmp        *icmp_ptr,
  dsm_item_type     **bp,
  pseudo_header_type *ph
)
{
  uint16 checksum;
  uint16 hdr_len   = sizeof(struct icmp_hdr);
  uint32 error_msg_flag = 0;   /* Outgoing msg is error msg (1) or not (0) */
  uint16 hdr_cksum = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /**/
  ICMPV4_INC_STATS(msgs_tx, 1);

  /**/
  /*-------------------------------------------------------------------------
    Sanity checks
  -------------------------------------------------------------------------*/
  if( NULL == bp || NULL == icmp_ptr || NULL == ph)
  {
    LOG_MSG_ERROR_0("NULL DSM item or icmp header or pseudo header in icmp_hdr_create()");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Save and clear checksum.
  -------------------------------------------------------------------------*/
  checksum          = icmp_ptr->info.chksum;
  icmp_ptr->info.chksum = 0;

  /*-------------------------------------------------------------------------
    Depending on the type of the ICMP packet we are dealing with, fill in
    the argument fields into the temporary argument structure accordingly.
    Then pushdown the arguments.
  -------------------------------------------------------------------------*/
  switch( icmp_ptr->info.type )
  {
    case ICMP_PARAM_PROB:
      ICMPV4_INC_STATS(parm_probs_tx, 1);
      icmp_ptr->arg.unused &= 0xFF000000U;
      error_msg_flag    = 1;
      hdr_len          += sizeof(uint32);
      break;

    case ICMP_REDIRECT:
      ICMPV4_INC_STATS(redirects_tx, 1);
      error_msg_flag  = 1;
      hdr_len        += sizeof(struct ps_in_addr);
      break;

    case ICMP_QUENCH:
      ICMPV4_INC_STATS(src_quench_tx, 1);
      icmp_ptr->arg.unused = 0;
      error_msg_flag = 1;
      hdr_len       += sizeof(uint32);
      break;
    case ICMP_TIME_EXCEED:
      ICMPV4_INC_STATS(time_exceeds_tx, 1);
      icmp_ptr->arg.unused = 0;
      error_msg_flag = 1;
      hdr_len       += sizeof(uint32);
      break;
    case ICMP_DEST_UNREACH:
      ICMPV4_INC_STATS(dest_unreach_tx, 1);
      if(icmp_ptr->info.code == (uint8)ICMP_FRAG_NEEDED)
      {
        icmp_ptr->arg.mtu.unused = 0;
        icmp_ptr->arg.mtu.mtu    = ps_htons(icmp_ptr->arg.mtu.mtu);
      }
      else
      {
        icmp_ptr->arg.unused = 0;
      }

      error_msg_flag = 1;
      hdr_len       += sizeof(uint32);
      break;

    case ICMP_TIME_REPLY:
      ICMPV4_INC_STATS(tstamp_reply_tx, 1);
      /*---------------------------------------------------------------------
        Update the timestamps and hdr_len
      ---------------------------------------------------------------------*/
      icmp_ptr->t_stamp.tx_timestamp   = (msclock() | 0x80000000U);
      icmp_ptr->t_stamp.tx_timestamp   = ps_htonl(icmp_ptr->t_stamp.tx_timestamp);

      hdr_len += sizeof(struct icmp_timestamp);
      icmp_ptr->arg.echo.id  = icmp_ptr->arg.echo.id;
      icmp_ptr->arg.echo.seq = icmp_ptr->arg.echo.seq;

      hdr_len += sizeof(struct echo_s);
      break;

    case ICMP_ECHO_REPLY:
      ICMPV4_INC_STATS(echo_reply_tx, 1);
      icmp_ptr->arg.echo.id  = icmp_ptr->arg.echo.id;
      icmp_ptr->arg.echo.seq = icmp_ptr->arg.echo.seq;

      hdr_len += sizeof(struct echo_s);
      break;

    case ICMP_INFO_REPLY:
      icmp_ptr->arg.echo.id  = icmp_ptr->arg.echo.id;
      icmp_ptr->arg.echo.seq = icmp_ptr->arg.echo.seq;

      hdr_len += sizeof(struct echo_s);
      break;

    case ICMP_ECHO_REQ:
      ICMPV4_INC_STATS(echo_tx, 1);
      icmp_ptr->arg.echo.id  = ps_htons(icmp_ptr->arg.echo.id);
      icmp_ptr->arg.echo.seq = ps_htons(icmp_ptr->arg.echo.seq);

      hdr_len += sizeof(struct echo_s);
      break;

    default:
      break;
  }/* switch */

  /*-------------------------------------------------------------------------
    Calculate the length of data going out in the ICMP message.
    If this is an error message the length of the complete ICMP datagram
    should not exceed 576 bytes (ICMP_MAX_LEN). Trim the data otherwise.
  -------------------------------------------------------------------------*/
  ph->length = (uint16)dsm_length_packet( *bp);
  if((error_msg_flag == 1) && ((ph->length + hdr_len + IPLEN) > ICMP_MAX_LEN))
  {
    ph->length = ICMP_MAX_LEN - hdr_len - IPLEN;
    dsm_trim_packet(bp, ph->length);
  }

  /*-------------------------------------------------------------------------
    Push the ICMP header on top of the data in dsm pointer.
  -------------------------------------------------------------------------*/
  if (dsm_pushdown_packed(bp,
                   (uint8 *)icmp_ptr,
                   hdr_len,
                   DSM_DS_SMALL_ITEM_POOL)
      < hdr_len)
  {
    LOG_MSG_ERROR_0("Out of Memory during dsm_pushdown_packed in icmp_hdr_create()");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Length of the data going out in the ICMP message for checksum.
  -------------------------------------------------------------------------*/
  ph->length += hdr_len;

  /*-------------------------------------------------------------------------
    Calculate the checksum and place it in the header.
    Use dsm_insert API to insert bytes into particular location in DSM packet
    chain. Manipulating data_ptr might result in DSM corruption {if the
    data_ptr doesnt have enough space to accomodate checksum}.
  -------------------------------------------------------------------------*/
  // (void)put16((*bp)->data_ptr + 2, cksum(NULL, *bp, ph->length, 0));
  hdr_cksum = cksum(NULL, *bp, ph->length, 0);
  hdr_cksum = ps_htons(hdr_cksum); /* Host to Network Byte Ordering */
  if (dsm_insert(bp, 
                 DSM_DS_SMALL_ITEM_POOL,
                 2, /* ICMP Header offset: type + code*/
                 (void*)&hdr_cksum, /* 2 byte checksum */
                 sizeof(uint16)) != sizeof(uint16))
  {
    LOG_MSG_ERROR_0("Error in inserting checksum in icmp_hdr_create()");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Restore the values that were changed in passed icmp hdr.
  -------------------------------------------------------------------------*/
  icmp_ptr->info.chksum = checksum;
  if(icmp_ptr->info.type == (uint8)ICMP_DEST_UNREACH &&
     icmp_ptr->info.code == (uint8)ICMP_FRAG_NEEDED)
  {
    icmp_ptr->arg.mtu.mtu = ps_ntohs(icmp_ptr->arg.mtu.mtu);
  }

  return TRUE;
} /* icmp_hdr_create() */

/*===========================================================================

FUNCTION ICMP_INPUT()

DESCRIPTION
 This function Process an incoming ICMP packet. The only ICMP messages that
 are processed are 1) Echo, 2) Info Request, and 3) Timestamp. Each of these
 messages will be echoed back to the ICMP peer in the base station. Other
 ICMP messages will be handled by dssicmp_ps_input() in dssicmp.c

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void icmp_input
(
  ip_pkt_info_type*        ip_pkt_info_ptr,    /* IP Packet Info           */
  struct dsm_item_s        *item_ptr,          /* Pointer to ICMP message  */
  uint16                   offset             /* Offset of the ICMP packet */
)
{
  struct icmp     icmp_hdr;  /* ICMP header                                */
  uint16          type;  /* Type of ICMP message                           */
  uint16          length;
  ip_cb_type      ip_cb;
  int16           ps_errno;
  pseudo_header_type ph;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ICMPV4_INC_STATS(msgs_rx, 1) ;

  /*------------------------------------------------------------------------
    Remove the ip header from the data_ptr.
  ------------------------------------------------------------------------*/
  if(dsm_pullup(&item_ptr, NULL, offset) != offset )
  {
    ICMPV4_INC_STATS(msgs_dropped_rx, 1) ;
    LOG_MSG_INFO2_0( "Truncated packet received" );
    dsm_free_packet( &item_ptr );
    return;
  }

  offset = 0;

  /*-------------------------------------------------------------------------
    Check if broadcast ICMP packets are to be ignored.
  -------------------------------------------------------------------------*/
  if( ip_pkt_info_ptr->is_brdcst )
  {
    LOG_MSG_INFO3_0( "ICMP Broadcast Rx'd, Ignored");
    dsm_free_packet( &item_ptr );
    return;
  }

  /*-------------------------------------------------------------------------
    Checksum verification. If not match, discard item_ptr.
  -------------------------------------------------------------------------*/
  length = (uint16) ip_pkt_info_ptr->ip_hdr.v4.length - IPLEN -
                    ip_pkt_info_ptr->ip_hdr.v4.optlen;

  if( cksum( NULL,item_ptr,length,0) != 0)
  {
    ICMPV4_INC_STATS(bad_chksum_rx, 1) ;
    ICMPV4_INC_STATS(msgs_dropped_rx, 1) ;
    LOG_MSG_INFO1_0("Invalid ICMP Checksum, Discard");
    dsm_free_packet( &item_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Get icmp header from the DSM pool.
  -------------------------------------------------------------------------*/
  if ( icmp_hdr_parse( &item_ptr, &offset, &icmp_hdr) == -1)
  {
    ICMPV4_INC_STATS(msgs_dropped_rx, 1) ;
    LOG_MSG_INFO1_0("icmp_hdr_parse conversion err, discard");
    dsm_free_packet( &item_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Get type of the icmp msg.
  -------------------------------------------------------------------------*/
  type = icmp_hdr.info.type;

  /*-------------------------------------------------------------------------
    Process the only valid ICMP messages: Echo Request (ping), Timestamp, and
    Info request.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO1_1( "ICMP message type %d", type);
  switch( type )
  {
    case ICMP_ECHO_REQ:
      ICMPV4_INC_STATS(echo_rx, 1);
      /*-------------------------------------------------------------------
        Change type to ECHO_REPLY, recompute checksum, and return datagram.
      -------------------------------------------------------------------*/
 
      /*---------------------------------------------------------------------
        Remove the icmp common header + echo header.
      ---------------------------------------------------------------------*/
      (void)dsm_pullup(&item_ptr, NULL, offset);

      icmp_hdr.info.type = (uint8)ICMP_ECHO_REPLY;
      break;

    case ICMP_TIMESTAMP:
      /*-------------------------------------------------------------------
        Change type to TIME_REPLY, recompute checksum, and return datagram.
        Note the source/dest addresses will be swapped by IP processing.
      -------------------------------------------------------------------*/
      ICMPV4_INC_STATS(tstamp_rx, 1);
      /*---------------------------------------------------------------------
        Remove the icmp common header + echo header + timestamp header.
        Note that in this case the offset would be updated to include
        timestamp header too.
      ---------------------------------------------------------------------*/
      (void)dsm_pullup(&item_ptr, NULL, offset);
      icmp_hdr.info.type = (uint8)ICMP_TIME_REPLY;
      break;

    case ICMP_INFO_RQST:
      /*-------------------------------------------------------------------
        Change type to INFO_REPLY, recompute checksum, and return datagram.
        Note the source/dest addresses will be swapped by IP processing.
      -------------------------------------------------------------------*/
      /*---------------------------------------------------------------------
        Remove the icmp common header + echo header.
      ---------------------------------------------------------------------*/
      (void)dsm_pullup(&item_ptr, NULL, offset);

      icmp_hdr.info.type = (uint8)ICMP_INFO_REPLY;
      break;

    case ICMP_DEST_UNREACH :
    case ICMP_QUENCH :
    case ICMP_TIME_EXCEED :
    case ICMP_PARAM_PROB :
      /*---------------------------------------------------------------------
        Appropriate error processing needs to be done.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO1_2("ICMP error msg rx'd,type = %d, code = %d",
                      	 icmp_hdr.info.type, icmp_hdr.info.code);
      icmp_process_error(&icmp_hdr, ip_pkt_info_ptr->ip_hdr.v4.source ,&item_ptr);
    return;


  default:
      /*---------------------------------------------------------------------
        If the ICMP packet we are dealing with is not one of the types
        defined above, handle it sperately within the PS context.
      ---------------------------------------------------------------------*/
    if (dssicmp_ps_input( &icmp_hdr, item_ptr, ip_pkt_info_ptr ) == DSS_ERROR)
    {
      ICMPV4_INC_STATS(unsupported_type_rx, 1);
      ICMPV4_INC_STATS(msgs_dropped_rx, 1);
      LOG_MSG_INFO1_1( "Unsupported ICMP message %d, Discard", type);
      dsm_free_packet( &item_ptr);
    }
    return;
  } /* end switch */

  /*-------------------------------------------------------------------------
    In case the request is received while the iface is being configured we
    need to drop the request.
  -------------------------------------------------------------------------*/
  if(IFACE_CONFIGURING == ps_iface_state((ps_iface_type *)ip_pkt_info_ptr->if_ptr))
  {
    ICMPV4_INC_STATS(msgs_dropped_tx, 1);
    LOG_MSG_INFO1_0("ICMP reply dropped, iface in configuring state");
    dsm_free_packet(&item_ptr);
    return;
  }
  /*-------------------------------------------------------------------------
    If the incoming message is some request (ECHO, TIMESTAMP or INFO_RQST)
    We need to generate a reply. We first see whether the reply needs to be
    rate limited according to the icmpv4_token_bucket.
  -------------------------------------------------------------------------*/
  ICMPV4_INC_STATS(msgs_tx,1);
  if(icmp_rate_limit_on_type((ps_icmp_msg_enum_type)icmp_hdr.info.type) == FALSE)
  {
    ICMPV4_INC_STATS(rate_limit_tx, 1);
    ICMPV4_INC_STATS(msgs_dropped_tx, 1);
    LOG_MSG_INFO1_0("ICMP reply is rate limited, cannot send the reply.");
    dsm_free_packet(&item_ptr);
    return;
  }

  if ( icmp_hdr_create( &icmp_hdr, &item_ptr, &ph) == FALSE)
  {
    ICMPV4_INC_STATS(msgs_dropped_tx, 1);
    LOG_MSG_INFO1_0("icmp_hdr_create() conversion err, discard");
    dsm_free_packet( &item_ptr);
    return;
  }

  if (item_ptr != NULL)
  {
    DSM_TOUCH_ITEM(item_ptr);
    memset(&ip_cb, 0, sizeof(ip_cb));
    ip_cb.routing_cache = (ps_iface_type*)ip_pkt_info_ptr->if_ptr;
    ip_cb.tos = (uint8)ip_pkt_info_ptr->ip_hdr.v4.tos;
    ip_cb.ttl = 0;
    ip_cb.df  = 0;
    ip_cb.qos  = 0;

    (void)
    ip4_send( &ip_cb,
              ip_pkt_info_ptr->ip_hdr.v4.source,
              (uint8)PS_IPPROTO_ICMP,
              item_ptr,
              length,
              NULL,
              &ps_errno);

  }
} /* icmp_input() */

/*===========================================================================

FUNCTION ICMP_OUTPUT()

DESCRIPTION
  This function does the following:
  1. Converts from host to network all the elements of the ICMP header
     and inserts the ICMP header in the ICMP packet.
  2. Creates the pseudo-header and calculates ICMP checksum.
  3. Sends ICMP packet down to IP.

DEPENDENCIES
  The source IP address should be correct.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void icmp_output
(
  struct ps_in6_addr *dest_ip_ptr,       /* destination of the delivery */
  struct icmp        *icmp_hdr_ptr,                  /* ptr to ICMP header */
  struct dsm_item_s  *item_ptr,                       /* item to be output */
  struct icmp_cb     *icmp_cb_ptr,                  /* Ptr to ICMP ctl blk */
  uint16             length,            /* Optional length of data portion */
  ps_tx_meta_info_type  *meta_info_ptr                    /* meta info ptr */
)
{
  int16              ps_errno;
  struct ps_in_addr  v4_addr;
  pseudo_header_type ph;

  (void)length;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ICMPV4_INC_STATS(msgs_tx,1);
  /*-------------------------------------------------------------------------
    We first see whether this message needs to be rate limited according
    to type and the icmpv4_token_bucket.
  -------------------------------------------------------------------------*/
  if(icmp_rate_limit_on_type((ps_icmp_msg_enum_type)icmp_hdr_ptr->info.type) == FALSE)
  {
    ICMPV4_INC_STATS(rate_limit_tx, 1);
    ICMPV4_INC_STATS(msgs_dropped_tx, 1);
    LOG_MSG_INFO1_0("ICMP message is rate limited, cannot send the reply.");
    dsm_free_packet(&item_ptr);
    return;
  }

  if(icmp_cb_ptr == NULL)
  {
    ICMPV4_INC_STATS(msgs_dropped_tx, 1);
    DATA_ERR_FATAL("NULL icmp cb passed");
    dsm_free_packet(&item_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Call to icmp_hdr_create() does the following:
      1. Allocate ICMP hdr
      2. Fill in ICMP hdr in network byte order
      3. Compute checksum
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_0(" ICMP_OUTPUT: calling icmp_hdr_create() ");
  if ( icmp_hdr_create( icmp_hdr_ptr, &item_ptr, &ph) == FALSE)
  {
    ICMPV4_INC_STATS(msgs_dropped_tx,1);
    LOG_MSG_INFO1_0("icmp_hdr_create() conversion err, discard");
    dsm_free_packet( &item_ptr);
    return;
  }

  DSM_TOUCH_ITEM(item_ptr);

  /**/
  v4_addr.ps_s_addr = PS_IN6_GET_V4_FROM_V4_MAPPED_V6(dest_ip_ptr);
  /**/

  (void) ip4_send( &icmp_cb_ptr->ipcb,
                   v4_addr,
                   (uint8)PS_IPPROTO_ICMP,
                   item_ptr,
                   ph.length,
                   meta_info_ptr,
                   &ps_errno
                 );

} /* icmp_output () */

/*===========================================================================

FUNCTION ICMP_SET_V4_TOKENS()

DESCRIPTION
  This function sets the values of ICMP v4 tokens per second and max tokens 
  used for calculating the reply rate.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void icmp_set_v4_tokens
(
  uint16 tokens_per_sec,
  uint16 max_tokens
)
{
  /*-------------------------------------------------------
    Set the value of the global variables here.
   ------------------------------------------------------*/
  icmp4_tokens_per_second = tokens_per_sec;
  icmp4_max_tokens = max_tokens;
  
  LOG_MSG_INFO2_2("ICMP4 tokens/sec = %d and max tokens = %d", 
                icmp4_tokens_per_second,
                icmp4_max_tokens);
}

/*===========================================================================

FUNCTION ICMP_INIT_TOKEN_BUCKET()

DESCRIPTION
  This function initializes the icmpv4_token_bucket.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void icmp_init_token_bucket
(
  void
)
{
  token_bucket_init(&icmpv4_token_bucket,
                    icmp4_tokens_per_second,
                    icmp4_max_tokens,
                    TRUE
                   );
} /* icmp_init_token_bucket() */

