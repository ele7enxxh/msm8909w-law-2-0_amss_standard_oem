#ifndef PS_IFACE_DNS_CACHE_H
#define PS_IFACE_DNS_CACHE_H

/*===========================================================================

           P S _ I F A C E _ D N S _ C A C H E . H

DESCRIPTION

  The Data Services DNS Subsystem Cache manager header file. Contains
  declarations of functions, variables, macros, structs and enums used by DNS
  Cache subsystem. This module is internal to the PS iface subsystem.

EXTERNALIZED FUNCTIONS

  ps_iface_dns_cache_init()
    Initializes the cache structures.

  ps_iface_dns_cache_add_rr_q()
    Adds a Q of RRs to the cache.

  ps_iface_dns_cache_find_rr_q()
    Finds a Q of RRs corresponding to a query.

  ps_iface_dns_cache_flush()
    Deletes all RR Queues from the cache of the specified iface 
    and frees memory associated with them.

  ps_dnsi_cache_mgr_flush_entry()
    Deletes an entry in the cache of the specified iface 
    and frees memory associated with it.

INTIALIZATIONS AND SEQUENCING REQUIREMENTS
  This module should be run only in PS task context.

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"


#include "pstimer.h"
#include "ps_iface_defs.h"
#include "ps_dnsi_query_defs.h"

/*===========================================================================

                                DATA STRUCTURES

===========================================================================*/
/*---------------------------------------------------------------------------
  Structure of a cache entry.
  query_data    - The query data for query made by client
  cname         - Canonical name of the URL so that cache lookup can be
                  done with query name as CNAME or Alias URL.
  query_type    - The query type for query made by client
  query_class   - The query class for query made by client
  min_ttl       - The minimal value among the TTLs of the RR nodes.
  pstimer       - Timer that expires after the minimum TTL in the RR
                  set has been reached.(All the RR entries in the RR set
                  should be cleaned whenever the RR with minimum TTL expires)
  rr_q          - A Q of generic resource record nodes.
  iface_ptr     - A pointer to the interface that the specific cache contains
                  this entry belongs to.
---------------------------------------------------------------------------*/
typedef struct ps_dnsi_cache_entry_s_type
{
  q_link_type             link;
  char                    query_data[PS_DNSI_MAX_DOMAIN_NAME_LEN];
  char                    cname[PS_DNSI_MAX_DOMAIN_NAME_LEN];
  uint16                  query_type;
  uint16                  query_class;
  uint32                  min_ttl;
  ps_timer_handle_type    pstimer;
  q_type                  rr_q;
  ps_iface_type         * iface_ptr;
} ps_dnsi_cache_entry_type;

/*===========================================================================

                             FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION  PS_IFACE_DNS_CACHE_INIT()

DESCRIPTION
  Initializes the cache Q during powerup.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_iface_dns_cache_init
(
  void
);

/*===========================================================================
FUNCTION  PS_IFACE_DNS_CACHE_ADD_RR_Q()

DESCRIPTION
  Adds a Q of RRs as a single cache entry to the cache. If the entire
  Q cannot be added, then truncated flag will be set to true in the
  cache entry.

PARAMETERS
  iface_ptr       - Pointer to the interface of the cache queue
  query_type      - Type of query
  query_class     - Class of query
  query_data_ptr  - Query data
  rr_q            - Q of RRs to be added to cache. All the nodes must
                    be of type ps_dnsi_generic_rr_type.
                    A NULL rr_q specifies that this is a negative cache.
  ps_errno        - Error number in case of error

RETURN VALUE
  Returns DSS_SUCCESS in case of success.
  Returns DSS_ERROR on error and sets errno to the error code.

  errno values
  ------------
  DS_EFAULT - Invalid arguments.
  DS_ENOMEM - If not enough memory to add into cache.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_iface_dns_cache_add_rr_q
(
  ps_iface_type                  * iface_ptr,
  ps_dnsi_query_type_enum_type     query_type,
  ps_dnsi_query_class_enum_type    query_class,
  char                           * query_data_ptr,
  q_type                         * rr_q,
  int16                          * ps_errno
);

/*===========================================================================
FUNCTION  PS_IFACE_DNS_CACHE_FIND_RR_Q()

DESCRIPTION
  Finds a Q of RRs corresponding to a query.

PARAMETERS
  iface_ptr       - Pointer to the interface of the cache queue
  query_type      - Type of query
  query_class     - Class of query
  query_data_ptr  - Query data
  rr_q            - Resultant RR Q.
  ps_errno        - Error code in case of error.

RETURN VALUE
  Returns the Q of RRs if found in cache. Each node of the Queue can be
  cast to ps_dnsi_generic_rr_type.
  Returns NULL if query results are not found in cache or its a
  negative cache hit.

  errno values
  ------------
  DS_ENOTFOUND    - Cache results are not found
  DS_EFAULT       - Invalid arguments
  DS_ENOMEM       - No memory for the RR q.
  DSS_SUCCESS     - On Negative or positive cache hit.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_iface_dns_cache_find_rr_q
(
  ps_iface_type                  * iface_ptr,
  ps_dnsi_query_type_enum_type     query_type,
  ps_dnsi_query_class_enum_type    query_class,
  char                           * query_data_ptr,
  q_type                         * rr_q,
  int16                          * ps_errno
);

/*===========================================================================
FUNCTION  PS_IFACE_DNS_CACHE_FLUSH()

DESCRIPTION
  Deletes all entries in the cache of the specified iface_ptr and frees memory 
  associated with them.
  This operation is done asynchronously by posting a command to the PS task.

PARAMETERS
  iface_ptr       - Pointer to the interface of the cache queue
  ps_errno        - Error number in case of error

RETURN VALUE
  Returns DSS_SUCCESS in case of success.
  Returns DSS_ERROR on error and sets errno to the error code.

  errno values
  ------------
  DS_EFAULT - Invalid arguments.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_iface_dns_cache_flush
(
  ps_iface_type                  * iface_ptr,
  int16                          * ps_errno
);

/*===========================================================================
FUNCTION  PS_IFACE_DNS_CACHE_FLUSH_ENTRY()

DESCRIPTION
  Deletes an entry in the cache of the specified iface_ptr.
  The entry is specified by host name.
  Entries with query type PS_DNSI_QUERY_TYPE_A or PS_DNSI_QUERY_TYPE_AAAA
  or PS_DNSI_QUERY_TYPE_CNAME with the specified host name are deleted (flushed)
  This operation is done asynchronously by posting a command to the PS task.

PARAMETERS
  iface_ptr       - Pointer to the interface of the cache queue
  hostname_ptr    - Name of the host for which entries should be flushed
  ps_errno        - Error number in case of error

RETURN VALUE
  Returns DSS_SUCCESS in case of success.
  Returns DSS_ERROR on error and sets errno to the error code.

  errno values
  ------------
  DS_EFAULT - Invalid arguments.
  DS_ENOMEM - Out of memory.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_iface_dns_cache_flush_entry
(
  ps_iface_type                  * iface_ptr,
  const char                     * hostname_ptr,
  int16                          * ps_errno
);

#endif  /* PS_IFACE_DNS_CACHE_H */
