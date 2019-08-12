#ifndef PS_ACLI_RULES_H
#define PS_ACLI_RULES_H
/*===========================================================================

                    A C L   I N T E R N A L    R U L E S

GENERAL DESCRIPTION
  Access control lists are used to define IP packet filters. These
  filters can be used e.g. for firewall functionality, address
  translation for selected ranges, or to build policy based routing
  rules.
  ACLs are defined as integer functions, which take information
  about the IP packet filtered as parameters. The return value
  is either '0' for a deny (packet drop) or the highest priority
  rule class for which a match was found.
  This file defines a common ACL function prototype as well as some
  macros to specify ACLs more similar to the usual language based
  syntax on routers and hosts.
  The following is an example of an ACL:

  ACL_DEF( inbound_filter )
  ACL_START
    DENY( ACL_DEFAULT_CLASS, DST_ADDR_IS_IN( IPV4( 127,0,0,1 ), 0xffffffff ) )
    DENY( ACL_DEFAULT_CLASS, SRC_ADDR_IS_IN( LOCALHOST, 0xffffffff ) )
    PASS( ACL_DEFAULT_CLASS, DST_ADDR_IS_IN( LOCALHOST, 0xffffffff ) )
  ACL_END

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_acli_rules.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/07/09    pp     Common Modem Interface: Public/Private API split.
                   (Split from ps_acli_rules.h)
===========================================================================*/
#include "ps_aclrules.h"

/*===========================================================================

                     THIS FILE IS OBSOLETE
     There are entities outside CommonData still using this, once the external
     dependencies are sorted out, this file will be removed.

===========================================================================*/

#endif /* PS_ACLI_RULES_H */
