#ifndef PS_IFACE_LOGGING_H
#define PS_IFACE_LOGGING_H

/*===========================================================================

                        P S _ I F A C E _ L O G G I N G . H

GENERAL DESCRIPTION
  This file defines external API used by mode handlers to enable logging and
  to populate description of various logging structures

EXTERNAL FUNCTIONS

  ps_iface_dpl_set_iface_desc()
    Set ps_iface's description

  ps_iface_dpl_set_flow_desc()
    Set a flow's(ps_phys_link) description

  ps_iface_dpl_set_phys_link_desc()
    Set a phys link's(for eg, RLP/RLC instance) description

  ps_iface_dpl_support_network_logging()
    Support network logging on the specified interface

  ps_iface_dpl_support_flow_logging()
    Support flow logging on the specified interface

  ps_iface_dpl_support_link_logging()
    Support link logging on the specified interface

Copyright (c) 2004-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/api/ps_iface_logging.h#1 $
 $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/08/09    pp     CMI Phase 4: SU Level API Effort.
12/25/06    msr    Fixed broken secondary link logging
08/15/05    msr    Using ps_flows instead of ps_phys_links to support flow
                   logging
01/12/05    msr    Added code review comments.
01/10/05    msr    Added support for flow and link logging.
10/31/04   ks/msr  Clean up and created ps_iface_logging.c to contain the
                   code for the functions defined here.
09/24/04   ks/msr  Created module.
=========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "ps_iface_defs.h"
#include "ps_logging_defs.h"



/*===========================================================================

                          EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION     PS_IFACE_DPL_SET_IFACE_DESC

DESCRIPTION
  Set ps_iface description in corresponding ps_iface structure

PARAMETERS
  ps_iface_ptr : pointer to ps_iface structure
  desc         : desc to be set

RETURN VALUE
  TRUE on success
  FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_dpl_set_iface_desc
(
  ps_iface_type  * ps_iface_ptr,
  char           * desc
);
/*===========================================================================
FUNCTION     PS_IFACE_DPL_SET_FLOW_DESC

DESCRIPTION
  Set flow description in corresponding ps_flow structure

PARAMETERS
  flow_ptr : pointer to ps_flow structure
  desc     : desc to be set

RETURN VALUE
  TRUE on success
  FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_dpl_set_flow_desc
(
  ps_flow_type  * flow_ptr,
  char          * desc
);
/*===========================================================================
FUNCTION     PS_IFACE_DPL_SET_PHYS_LINK_DESC

DESCRIPTION
  Set phys link description in corresponding phys link structure

PARAMETERS
  phys_link_ptr : pointer to phys link
  desc          : desc to be set

RETURN VALUE
  TRUE on success
  FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_dpl_set_phys_link_desc
(
  ps_phys_link_type  * phys_link_ptr,
  char               * desc
);

/*===========================================================================
FUNCTION    PS_IFACE_DPL_SUPPORT_NETWORK_LOGGING

DESCRIPTION
  Populate ifname in logging control block in ps_iface to indicate that
  logging is supported on this interface.

PARAMETERs
  ps_iface_ptr : ptr to iface
  ifname       : IID ifname to be set

RETURN VALUE
   0 on success
  -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_dpl_support_network_logging
(
  ps_iface_type            * ps_iface_ptr,
  dpl_iid_ifname_enum_type   ifname
);

/*===========================================================================
FUNCTION    PS_IFACE_DPL_SUPPORT_FLOW_LOGGING

DESCRIPTION
  Populate ifname in logging control block in default ps_flow of an ps_iface
  to indicate that flow logging is supported on this ps_iface

PARAMETERs
  ps_iface_ptr : ptr to iface
  ifname       : IID ifname to be set

RETURN VALUE
  0 on success
  -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_dpl_support_flow_logging
(
  ps_iface_type            * ps_iface_ptr,
  dpl_iid_ifname_enum_type   ifname
);

/*===========================================================================
FUNCTION    PS_IFACE_DPL_SUPPORT_LINK_LOGGING

DESCRIPTION
  Populate ifname in logging control block in link_cb_ptr to indicate that
  logging is supported on this interface.

PARAMETERs
  ps_iface_ptr : ptr to iface
  ifname       : IID ifname to be set

RETURN VALUE
  0 on success
  -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_dpl_support_link_logging
(
  ps_iface_type            * ps_iface_ptr,
  dpl_iid_ifname_enum_type   ifname
);
#endif /* PS_IFACE_LOGGING_H */
