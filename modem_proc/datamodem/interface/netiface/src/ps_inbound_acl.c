/*===========================================================================

                   P R O T O C O L   S E R V I C E S

       A C C E S S   C O N T R O L   L I S T   D E F I N I T I O N S

            F O R   T H E   I N B O U N D   I P   F I L T E R

GENERAL DESCRIPTION
  The PS inbound filter is used to protect the mobile device
  from malicious traffic.
  This filter is application specific and should be re-evaluated
  for each model.
  All firewall type filter rules should use the ACL_DEFAULT_CLASS.
  Any specific rules to identify packets for this host (e.g.
  destination address matches interface address) should use the
  ACL_IPADDR_CLASS, which is used to tag local traffic.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_inbound_acl.c_v   1.6   19 Feb 2003 14:32:04   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_inbound_acl.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/05   sv      Lint changes.
10/13/04   vp      Representation of netmask as an IP addresses instead of
                   hex number.
04/30/04   usb     Loopback support through all ifaces
03/15/04   mct     Added check to deny v6 and pass v4 for new routing updates
03/01/04   mct     Updated ACL check for V6
01/31/04    usb    Renamed THIS_IFACE_NAME_IS to INCOMING_IFACE_TYPE_IS
                   to correctly reflect the operation. Added ACL_START_GLOBAL
                   for global ACLs.
05/13/03    usb    Updated rule names to reflect operation on this iface
02/19/03    usb    Allowing all addresses to go up the stack for now.
02/12/03    om     Fixed loopback if rule.
01/31/03    usb    Added new rule to check IP version and loopback iface,
                   moved DENY rules before local check rule
11/25/02    usb    Updated DENY rules as per the new format.
09/13/02    om     Update to includes.
03/08/02    om     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ps_iface.h"
#include "ps_inbound_acl.h"

/* <EJECT> */ 
/*===========================================================================

                 ACCESS CONTROL LIST NAME DEFINITIONS

===========================================================================*/
ACL_DEF( ps_inbound_acl )
ACL_START_GLOBAL
  int i;
  ps_iface_type *if_ptr;

  DENY( IP_POL_FAMILY_IS( IFACE_IPV6_ADDR_FAMILY ) )
  DENY( NOT(IS_PKT_INFO_NULL() OR IP_PKT_FAMILY_IS( IFACE_IPV4_ADDR_FAMILY)))
  PASS_QUICK(ACL_LOOPBACK_CLASS, INCOMING_IFACE_TYPE_IS( LO_IFACE ) )
  for (i=0; i<MAX_SYSTEM_IFACES; i++)
  {
    if((if_ptr = global_iface_ptr_array[i]) == NULL)
      break;
    PASS_QUICK(ACL_LOOPBACK_CLASS,
               ((ps_iface_state(if_ptr) != IFACE_ROUTEABLE) &&
               (SRC_ADDR_IS_IN(PS_IFACE_IP_V4_ADDR(if_ptr), IPV4(255,255,255,255)))));
  }
  DENY( DST_ADDR_IS_IN( IPV4( 127,0,0,1 ), IPV4(255,255,255,255) ) )
  DENY( SRC_ADDR_IS_IN( LOCALHOST, IPV4(255,255,255,255) ) )
  /*-------------------------------------------------------------------------
    For now accept all other src/dst addresses. This will pass all non
    loopback pkts up the stack including broadcast and multicast pkts.
    It needs to be fixed later to handle broadcast/multicast more
    appropriately and also ifaces with null addresses.
  -------------------------------------------------------------------------*/
  PASS_DEFAULT( ACL_IPADDR_CLASS )
//  PASS( ACL_IPADDR_CLASS, DST_ADDR_IS_IN( LOCALHOST, 0xffffffff ) )
ACL_END
