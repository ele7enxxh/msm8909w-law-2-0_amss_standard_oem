#ifndef DSS_DNS_CONFIG_H
#define DSS_DNS_CONFIG_H
/*===========================================================================

       D A T A   S E R V I C E S   D N S   A P I   C O N F I G   F I L E

DESCRIPTION

 The Data Services DNS Configuration File. Contains declarations of macros
 that can be configured by clients.

EXTERNALIZED FUNCTIONS
  dss_dns_getconst()
    This function gets the value of the constant parameter asked for.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2007,2009-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
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
  Timeout intervals (in milliseconds) and retry attempts
---------------------------------------------------------------------------*/
#define DSS_DNS_NET_TIMEOUT_INTERVAL   7000
#define DSS_DNS_MAX_NET_RETRIES        3
/*---------------------------------------------------------------------------
  Query timeout intervals (in milliseconds) and retry attempts
  The timeout calculated for a retry is 
  FIRST_QUERY_TIMEOUT+(RETRIES_BACKOFF*retry_index)
  Where the retry_index specify which retry is that.(0 for first, 
  1 for second etc.)
  These constants (FIRST_QUERY_TIMEOUT_INTERVAL, MAX_QUERY_RETRIES
  and RETRIES_BACKOFF) are default values and can be changed during runtime
  per session.
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Default timeout for the first Query (in milliseconds).
---------------------------------------------------------------------------*/
#define DSS_DNS_DEFAULT_FIRST_QUERY_TIMEOUT 3000

/*---------------------------------------------------------------------------
  Default maximum query retries.
---------------------------------------------------------------------------*/
#define DSS_DNS_DEFAULT_MAX_QUERY_RETRIES      7

/*---------------------------------------------------------------------------
  Default query retries backoff (in milliseconds).
---------------------------------------------------------------------------*/
#define DSS_DNS_DEFAULT_RETRIES_BACKOFF      1000

/*===========================================================================

                                DATA STRUCTURES

===========================================================================*/
/*---------------------------------------------------------------------------
  Enumeration of configurable parameters
---------------------------------------------------------------------------*/
typedef enum dss_dns_const_enum_type
{
  DSS_DNS_CONST_MIN                           = 0,
  DSS_DNS_CONST_NET_TIMEOUT                   = DSS_DNS_CONST_MIN,
  DSS_DNS_CONST_NET_RETRIES                   = 1,
  DSS_DNS_CONST_DEFAULT_FIRST_QUERY_TIMEOUT   = 2,
  DSS_DNS_CONST_DEFAULT_MAX_QUERY_RETRIES     = 3,
  DSS_DNS_CONST_DEFAULT_RETRIES_BACKOFF       = 4,
  DSS_DNS_CONST_MAX                           = 5
} dss_dns_const_enum_type;

/*===========================================================================

                             FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSS_DNS_GETCONST

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
int16 dss_dns_getconst
(
  dss_dns_const_enum_type      const_param,
  void                       * const_val,
  uint32                       const_val_size
);

#endif /* DSS_DNS_CONFIG_H */
