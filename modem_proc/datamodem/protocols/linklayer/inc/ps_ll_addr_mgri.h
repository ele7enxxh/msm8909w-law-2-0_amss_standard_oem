#ifndef PS_LL_ADDR_MGRI_H
#define PS_LL_ADDR_MGRI_H
/*===========================================================================

                  P S _ L L _ A D D R _ M G R I . H


DESCRIPTION
This is the header file for the PS Link Local Address manager [internal].

This entity is responsible for dynamic configuration of IPv4 link local
addresses per RFC 3927

This is particularly useful on a ilnk where connectivity is restricted to
`nodes' on the same link. e.g. in Ad Hoc networks

It is important to note that the 169.254/16 addresses registered with IANA
for this purpose are not routable addresses.

NOTE: Use of this module for IP address configuration may cause some
      applications to fail!!


EXTERNALIZED FUNCTIONS

  ll_addr_mgr_init()
    Called to initialize the link local address mgr (at powerup)

Copyright (c) 2008 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_ll_addr_mgri.h#1 $
  $Author: mplcsds1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/17/08    pp     Created module as part of Common Modem Interface:
                   Public / Private split.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"   /* Customer Specific Features                      */


/*===========================================================================

                          FORWARD DECLARATIONS

===========================================================================*/


/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION LL_ADDR_MGR_INIT()

DESCRIPTION
  This function is responsible for (power-up) initialization of the Link
  Local address manager

DEPENDENCIES
  Must be called in PS context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void ll_addr_mgr_init
(
  void
);


#endif /* PS_LL_ADDR_MGR_H */
