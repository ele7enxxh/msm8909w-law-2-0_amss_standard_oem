/*===========================================================================

                    P S _ S T A T _ C O M M O N I . C

GENERAL DESCRIPTION
  This program provides the statistic access functions that can be used by
  all layers

EXTERNALIZED FUNCTIONS
  ps_stat_init
    This function initializes the statistics module appropriately

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright (c) 2007-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ps_stat_commoni.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/10   rp      RAM optimization changes.
05/07/09   sp      Added support for updating the HC packed files.
08/19/08   pp      Included ps_mem.h to avoid compile warnings.
04/04/07   scb     Added featurization for IPHC, ROHC and RSVP
02/28/07   scb     Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "IxErrno.h"
#include "ps_stat_commoni.h"
#include "ps_mem.h"

#include "ps_stat_arp.h"
#include "ps_stat_llc.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_stat_icmpv6.h"
#include "ps_stat_ipv6.h"
#endif/* FEATURE_DATA_PS_IPV6 */

#include "ps_stat_flow.h"
#include "ps_stat_icmpv4.h"
#include "ps_stat_iface.h"
#include "ps_stat_ipv4.h"
#include "ps_stat_mem.h"
#include "ps_stat_phys_link.h"

#ifdef FEATURE_DATA_PS_PPP
#include "ps_stat_ppp.h"
#endif

#include "ps_stat_rsvp.h"
#include "ps_stat_tcp.h"
#include "ps_stat_udp.h"

#include "err.h"
#include <string.h>

/*===========================================================================
FUNCTION PS_STAT_INIT()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init
(
  void
)
{
  
  /*-------------------------------------------------------------------------
    For each of the statistics modules, initialize the offset structures
  -------------------------------------------------------------------------*/
  ps_stat_init_arp();
  ps_stat_init_llc();

#ifdef FEATURE_DATA_PS_IPV6
  ps_stat_init_ipv6();
  ps_stat_init_icmpv6();
#endif/* FEATURE_DATA_PS_IPV6 */

  ps_stat_init_flow();
  ps_stat_init_icmpv4();
  ps_stat_init_iface();
  ps_stat_init_ipv4();
  ps_stat_init_mem();
  ps_stat_init_phys_link();

#ifdef FEATURE_DATA_PS_PPP  
  ps_stat_init_ppp();
#endif

  ps_stat_init_rsvp();
  ps_stat_init_tcp();
  ps_stat_init_udp();
}

