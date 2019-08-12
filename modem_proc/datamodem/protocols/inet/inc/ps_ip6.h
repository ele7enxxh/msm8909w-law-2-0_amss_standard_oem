#ifndef PS_IP6_H
#define PS_IP6_H
/*===========================================================================

                                P S _ I P 6 . H

DESCRIPTION
 This file contains definitions and function prototypes specific to IPv6.

Copyright (c) 1995-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ip.h_v   1.1   30 Aug 2002 20:59:24   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_ip6.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/12/08    pp     Metainfo optimizations.
04/18/05    mct    Added ip6_route() to support v6 loopback.
03/01/05    sv     Added support to pass metainfo to IP layer on rx_path
06/11/04    vp     Removed inclusion of ps_ip_defs.h and included ps_ip.h
04/05/04    sv     Featurized the header file.
03/02/04    rc     Removed tx interface cache entry from ip6_send() signature.
01/09/04    rc     Initial Version

===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)



#include "ps_ip.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION IP6_SEND()

DESCRIPTION
  This function is used by the upper layers (TCP/UDP/ICMP) to send
  an IPv6 datagram. 

DEPENDENCIES
  None

RETURN VALUE
  0 for success and -1 for failure (ps_errno contains error code)

SIDE EFFECTS
  None
===========================================================================*/
int ip6_send
(
  ip_cb_type         *ip_cb,          /* IP control block                  */
  struct ps_in6_addr dest,            /* Destination address               */
  uint8              protocol,        /* Protocol                          */
  dsm_item_type*     send_dg_ptr,     /* Pointer to incoming datagram      */
  uint16             length,          /* Optional length of data portion   */
  ps_tx_meta_info_type* meta_info_ptr, /* Metainfo about packet             */
  int16*             ps_errno         /* Error code for failure case       */
);

/*===========================================================================

FUNCTION IP6_INPUT()

DESCRIPTION
  This function accepts received IPv6 packet(-fragment)s from network
  interfaces and applies ipfilter for the DMSS inbound ACL.
 
DEPENDENCIES
  None

RETURN VALUE
  0 for success, -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int ip6_input
(
  ps_iface_type *in_if_ptr,           /* Input interface                   */
  dsm_item_type *bp,                  /* Input packet                      */
  uint8         *source,              /* Media source address, if any      */
  int            broadcast,           /* Packet had link broadcast address */
  ps_rx_meta_info_type * meta_info_ptr /* Meta info pointer                */
);

/*===========================================================================

FUNCTION IP6_ROUTE()

DESCRIPTION
  This function finds the appropriate interface to send a given IPv6 packet 
  on and forwards it appropriately.

DEPENDENCIES
  If this function calls another function, say X, then X is responsible for
  freeing up memory in ALL cases.

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code)

   ps_errno can contain:
   E_NOTALLOWED - Pkt forwarding is disabled
   E_NOROUTE    - No route to destination could be found
   E_NONET      - Route to destination is cuurently not accessible

SIDE EFFECTS
  None
===========================================================================*/
int ip6_route
(
  ip6_hdr_type*            ip6_hdr,           /* Pkt and policy info       */
  ps_tx_meta_info_type*    meta_info_ptr,     /* Meta info                */
  dsm_item_type**          ip_pkt_ptr,        /* Input packet              */
  ps_iface_type*           tx_if_cache,       /* cached Tx route           */
  int16*                   ps_errno           /* Error code for faillure   */
);



#endif /* FEATURE_DATA_PS || FEATURE_DATA_PS_IPV6 */
#endif /* PS_IP6_H */
