/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ I C M P 6 _ D E F S . H

GENERAL DESCRIPTION
 Internet Protocol Version 6 - Neighbor Discovery(IPv6 ND)

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c)2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_icmp6_defs.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/23/07    mct    Initial Version
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "datamodem_variation.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)
/*---------------------------------------------------------------------------
  NV Items to define... initialize from the IP6_INIT. define externs here
---------------------------------------------------------------------------*/
#define ICMP6_ND_MAX_MULTICAST_SOLICT    3
#define ICMP6_ND_MAX_RETRANSMITS         3
#define ICMP6_ND_DEFAULT_REACHABLE_TIME  30000
#define ICMP6_ND_DEFAULT_RETRANSMIT_TIME 1000
#define ICMP6_ND_DELAY_FIRST_PROBE_TIME  5000

typedef enum
{
  /* There is no entry                                                     */
  ICMP6_ND_STATE_NO_ENTRY      = 0,
  /* Address resolution is in progress and the link-layer address of the
     neighbor has not yet been determined.                                 */
  ICMP6_ND_STATE_INCOMPLETE    = 1,
  /* The neighbor is known to have been reachable recently.                */
  ICMP6_ND_STATE_REACHABLE     = 2,
  /* The neighbor is no longer known to be reachable but until traffic is sent
     to the neighbor, no attempt should be made to verify its reachability.*/
  ICMP6_ND_STATE_STALE         = 3,
  /* The neighbor is no longer known to be reachable, and traffic has recently
     been sent to the neighbor. Rather than probe the neighbor immediately,
     however, delay sending probes for a short while in order to give upper
     layer protocols a chance to provide reachability confirmation.        */
  ICMP6_ND_STATE_DELAY         = 4,
  /* The neighbor is no longer known to be reachable, and Neighbor Solicitation
     probes are being sent to verify reachability.                         */
  ICMP6_ND_STATE_PROBE         = 5
} ps_icmp6_nd_state_enum_type;

#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6 */
