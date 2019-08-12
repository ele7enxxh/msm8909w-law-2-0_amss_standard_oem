#ifndef PS_DNS_CACHE_CONFIG_H
#define PS_DNS_CACHE_CONFIG_H
/*===========================================================================

           P S _ D N S _ C A C H E _ C O N F I G . H

DESCRIPTION

 The Data Services DNS cache Configuration File. Contains declarations of
 macros that can be configured by clients.

EXTERNALIZED FUNCTIONS
  ps_dns_cache_getconst()
    This function gets the value of the constant parameter asked for.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"


/*===========================================================================

                            CONFIGURABLE PARAMETERS

===========================================================================*/
/*--------------------------------------------------------------------------
  DNS Cache parameters
---------------------------------------------------------------------------*/
#define PS_DNS_CACHE_MAX_ENTRIES     5   /* Entries in cache table        */
#define PS_DNS_CACHE_MIN_TTL         5   /* Minimum ttl in sec to cache   */
#define PS_DNS_CACHE_MAX_TTL         0x15180 /* Maximum ttl in sec        */
#define PS_DNS_CACHE_NEGATIVE_TTL    60 /* Negative cache timeout value   */

/*===========================================================================

                                DATA STRUCTURES

===========================================================================*/
/*---------------------------------------------------------------------------
  Enumeration of configurable parameters
---------------------------------------------------------------------------*/
typedef enum ps_dns_cache_const_enum_type
{
  PS_DNS_CACHE_CONST_MIN            = 0,
  PS_DNS_CACHE_CONST_MAX_ENTRIES    = PS_DNS_CACHE_CONST_MIN,
  PS_DNS_CACHE_CONST_MIN_TTL        = 1,
  PS_DNS_CACHE_CONST_MAX_TTL        = 2,
  PS_DNS_CACHE_CONST_NEGATIVE_TTL   = 3,
  PS_DNS_CACHE_CONST_MAX            = 4
} ps_dns_cache_const_enum_type;

/*===========================================================================

                             FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_DNS_CACHE_GETCONST

DESCRIPTION
  Function called by DNS library to acquire a configurable compile time
  constant.

DEPENDENCIES
  None

RETURN VALUE
  0 on Success
  -1 on Failure

SIDE EFFECTS
  None
===========================================================================*/
int16 ps_dns_cache_getconst
(
  ps_dns_cache_const_enum_type const_param,
  void                       * const_val,
  uint32                       const_val_size
);

#endif /* PS_DNS_CACHE_CONFIG_H */
