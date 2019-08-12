#ifndef PS_IP_ADDR_H
#define PS_IP_ADDR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            P S _ I P _ A D D R . H

GENERAL DESCRIPTION
  This is the header file that defines all of the IP addressing types and
  macros. 

Copyright (c) 2002-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ip_addr.h_v   1.5   11 Oct 2002 09:15:46   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_ip_addr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/22/06    sv     Added support for AF_UNSPEC.
11/02/06    mct    Added support for RFC3041: IPv6 Privacy Extensions.
09/05/06    sv     Added UNSPEC IFACE address family.
08/29/06    ks     Added mask value for dial string param added earlier. 
08/15/06    asen   Added dial_string[] to network_params_info_type data structure
12/02/05    ks     Added common structures for network config params to be
                   passed in ps_iface_bring_up_cmd().
12/23/04    lyr    Added subnet mask in ip_v4_net_info_type & addr scope enum
11/01/04    rr/ks  Added primary_nbns and secondary_nbns structs in
                   ip_v4_net_info_type for WINS support.
07/30/04    vp     Consolidation of ip_v4_net_info_type and 
                   ip_v6_net_info_type in network_info_type structure.
06/14/04    mct    Added support in v6_net_info for V6 DNS addresses.
06/11/04    vp     Included file ps_in.h. Provided aliases for address family
                   types in ip_addr_enum_type. Changes for representation of 
                   IP addresses as struct ps_in_addr or struct ps_in6_addr.
04/23/04    usb    Added gateway iid field to v6 net info block.
12/07/03    aku/rc Added support for v6 net info
09/25/03    ifk    Removed PS_DATA_UNSUPPORTED around the v6 address in
                   ip_addr_type
10/01/02    mvl    Added gateway address to the v4_net_info.
09/17/02    mvl    renamed dns_prefix to dns_suffix.
09/02/02    mvl    file cleanup.
07/01/02    mvl    Simplified names.
03/22/02    mvl    created file
===========================================================================*/

/*===========================================================================

  OBSOLETE ... OBSOLETE ... OBSOLETE ... OBSOLETE ... OBSOLETE ... OBSOLETE

  THIS HEADER FILE SHOULD NOT BE USED GOING FORWARD, AS THIS JUST CONTAIN
  INCLUSION TO "ps.in.h".
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/

#include "ps_in.h"


#endif /* PS_IP_ADDR_H */
