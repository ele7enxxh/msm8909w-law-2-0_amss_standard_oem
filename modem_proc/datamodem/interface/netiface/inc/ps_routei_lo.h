#ifndef PS_ROUTEI_LO_H
#define PS_ROUTEI_LO_H
/*===========================================================================

                   P R O T O C O L   S E R V I C E S

              I N T E R N A L   L O O P B A C K   R O U T I N G

                          D E F I N I T I O N S

GENERAL DESCRIPTION
  This file defines the structure and management functions for
  the kernel's routing table. The table is a fixed size array
  for lookup efficiency.

  Functions and data structures are specific to the IP version
  and named accordingly.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_routei_lo.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/08/12   su      Changes for VToLTE feature 
02/22/11   ss      Compiler warning fixes.
12/23/10   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
12/14/08   pp      Created module as part of Common Modem Interface:
                   Public/Private API split.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "target.h"
#include "ps_route.h"
#include "ps_iface.h"
#include "ps_socket_defs.h"

/* <EJECT> */ 
/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

/* Loopback routing stuctures and arrays. */
typedef struct
{
  ps_iface_type*    iface_ptr;
  struct ps_in_addr ip_addr;
} ps_lo_v4_route_type;

#ifdef __cplusplus
extern "C" {
#endif
extern ps_lo_v4_route_type global_v4_lo_route_array[MAX_IPV4_LO_IFACES];
#ifdef __cplusplus
}
#endif

#ifdef FEATURE_DATA_PS_IPV6
typedef struct
{
  ps_iface_type*  iface_ptr;
} ps_lo_v6_route_type;

#ifdef __cplusplus
extern "C" {
#endif
extern ps_lo_v6_route_type global_v6_lo_route_array[MAX_IPV6_LO_IFACES];
#ifdef __cplusplus
}
#endif
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
FUNCTION PS_GET_V4_LO_RSCOPE_IDX()

DESCRIPTION
  This function returns the V4 loopback route scope index.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The V4 loopback route scope index.

SIDE EFFECTS
  None
===========================================================================*/
uint32 ps_get_v4_lo_rscope_idx(void);

/*===========================================================================
FUNCTION PS_GET_V6_LO_RSCOPE_IDX()

DESCRIPTION
  This function returns the V6 loopback route scope index.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The V6 loopback route scope index.

SIDE EFFECTS
  None
===========================================================================*/
uint32 ps_get_v6_lo_rscope_idx(void);
#ifdef __cplusplus
}
#endif


/*===========================================================================
FUNCTION PS_ROUTE_LO_IPV4()

DESCRIPTION
  This function determines whether the packet is destined to a local
  IPv4 loopback interface.

DEPENDENCIES
  None.

PARAMETERS
  ip_addr - Destination IPv4 address of the packet.

RETURN VALUE
  ps_iface_ptr of loopback iface
  NULL if the packet is not destined to a local IPv4 iface.

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
INLINE ps_iface_type *ps_route_lo_ipv4
(
  struct ps_in_addr *ip_addr
)
{
  int             index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for(index=0; global_v4_lo_route_array[index].iface_ptr != NULL; index++)
  {
    if( (global_v4_lo_route_array[index].ip_addr.ps_s_addr ==
          ip_addr->ps_s_addr) &&
        (ps_iface_state(global_v4_lo_route_array[index].iface_ptr) !=
          IFACE_ROUTEABLE) )
    {
      return global_v4_lo_route_array[index].iface_ptr;
    }
  }
  return NULL;
} /* ps_route_lo_ipv4() */
#ifdef __cplusplus
}
#endif


#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION PS_ROUTE_LO_IPV6()

DESCRIPTION
  This function determines whether the packet is destined to a local
  IPv6 loopback interface.

DEPENDENCIES
  None.

PARAMETERS
  ip_addr - Destination IPv6 address of the packet.

RETURN VALUE
  ps_iface_ptr of loopback iface
  NULL if the packet is not destined to a local IPv4 iface.

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
INLINE ps_iface_type *ps_route_lo_ipv6
(
  struct ps_in6_addr *ip_addr
)
{
  int                      index;
  int              addr_index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for(index = 0; global_v6_lo_route_array[index].iface_ptr != NULL; index++)
  {
    if (MODEM_LINK_LOCAL_IFACE == 
          global_v6_lo_route_array[index].iface_ptr->name)
    {      
      continue;
    }

    for(addr_index=0; addr_index < MAX_IPV6_ADDRS; addr_index++)
    {
      /*--------------------------------------------------------------------
        If IPv6 prefix is 0 we're at the end. Stop address search loop.
      --------------------------------------------------------------------*/
      if(global_v6_lo_route_array[index].iface_ptr->iface_private.
           ipv6_addrs[addr_index] != NULL)
      {
        if( (global_v6_lo_route_array[index].iface_ptr->iface_private.
              ipv6_addrs[addr_index]->prefix == ip_addr->ps_s6_addr64[0]) &&
            (global_v6_lo_route_array[index].iface_ptr->iface_private.
              ipv6_addrs[addr_index]->iid == ip_addr->ps_s6_addr64[1]) )
        {
          if(ps_iface_state(global_v6_lo_route_array[index].iface_ptr) !=
             IFACE_ROUTEABLE)
          {
            /*---------------------------------------------------------------
              Addresses match and iface isn't routable return loopback iface
            ---------------------------------------------------------------*/
            return global_v6_lo_route_array[index].iface_ptr;
          }
        }
      }/* end if addr is NULL */
      else
      {
        break;
      }
    } /* end addr for loop */
  } /* end iface loop */

  /*-------------------------------------------------------------------------
    Packet not destined to any IPv6 loopback iface
  --------------------------------------------------------------------------*/
  return NULL;
} /* ps_route_lo_ipv6() */
#ifdef __cplusplus
}
#endif
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION PS_ROUTE_LO_UPDATE()

DESCRIPTION
  This function is used to update the loopback routing table. It is
  called whenever and address changes on an interface, either when bringing
  it up initially or during an address change on handoff.

DEPENDENCIES
  Currently we support only one IPv4 address per IPv4 interface. For IPv6 we
  support only one prefix and IID per IPv6 interface. When support for
  RFC3041 is added multiple IID support will need to be added.


PARAMETERS
  this_iface_ptr      - Pointer to the interface
  ip_addr_ptr         - Pointer to the IP address

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_route_lo_update
(
  ps_iface_type*       this_iface_ptr,
  ps_ip_addr_type*     ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_ROUTE_LO_DELETE()

DESCRIPTION
  This function is used to clear out an interface's loopback entry when it
  is torn down.

DEPENDENCIES

PARAMETERS
  this_iface_ptr      - Pointer to the interface

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_route_lo_delete
(
  ps_iface_type*       this_iface_ptr
);

#endif    /* ROUTEI_H */
