#ifndef PS_IFACEI_ADDR_MGMT_H
#define PS_IFACEI_ADDR_MGMT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ I F A C E _ A D D R _ M G M T . H

GENERAL DESCRIPTION
  Interface IP Address Management Layer

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c)2008-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_iface_addr_mgmt.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/07/12    ash    Initial version
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_in.h"
#include "ps_hashtable.h"

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/

/**
@brief Struct for Prefix & Prefix length. 
*/
typedef struct
{
  struct ps_in6_addr  prefix;
  uint8               prefix_len;
} ps_iface_addr_mgmt_prefix_type;

/*---------------------------------------------------------------------------
TYPEDEF IP6_PREFIX_MODE_TYPE

DESCRIPTION
  This is the definition of the IPv6 prefix mode.
---------------------------------------------------------------------------*/
typedef enum
{
  IP6_PREFIX_SHARING     = 0,
  IP6_PREFIX_DELEGATION  = 1,
  IP6_PREFIX_MAX_MODE
} ps_iface_ip6_prefix_mode_type;

/**
@brief Stores the PS interface Delegated Prefix & Prefixes 
       Generated from them.
*/
typedef struct 
{
  ps_iface_addr_mgmt_prefix_type  delegated_prefix;                                 
    /**< IPv6 delegated prefix.  */

  boolean                         exclude_prefix;
  ps_iface_addr_mgmt_prefix_type  prefix_to_exclude;                                 
    /**< IPv6 delegated prefix.  */

  ps_hashtable_type               prefix_ht;
    /**< Prefix Hash Table.  */

  uint64                          last_prefix;
  /**< Last generated prefix. */
} ps_ifacei_prefix_mgmt_type;

#endif /* PS_IFACEI_ADDR_MGMT_H */
