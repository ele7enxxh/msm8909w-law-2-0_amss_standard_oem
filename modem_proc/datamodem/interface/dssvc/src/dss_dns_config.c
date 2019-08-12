/*===========================================================================
                        D S S _ D N S _ C O N F I G . C

DESCRIPTION
  This file contains the access function to get the compile time parameters.

EXTERNALIZED FUNCTIONS
  dss_dns_getconst()
    This function gets the value of the constant parameter asked for.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2007,2009-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssvc/src/dss_dns_config.c#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/08/10    ea     The cache related constants will be handled in
                   ps_dns_cache_config.c.
02/12/09    ts     Moved DSS_DNS_CONST_QUERY_RETRIES and 
                   DSS_DNS_CONST_QUERY_TIMEOUT from dss_dns_const_enum_type
                   since they now can be configured during runtime.
06/05/07    hm     Initial development.

===========================================================================*/
/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"


#include "amssassert.h"
#include "dss_dns_config.h"
#include "msg.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSS_DNS_GETCONST

DESCRIPTION
  Function called by DNS library to acquire a configurable compile time
  constant.

DEPENDENCIES
  None.

RETURN VALUE
   0 on Success
   -1 on  Failure

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_getconst
(
  dss_dns_const_enum_type      const_param,
  void                       * const_val,
  uint32                       const_val_size
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3( "dss_dns_getconst():  called.  Args: param=%d, val=%p, size=%d",
                   const_param, const_val, const_val_size );

  switch( const_param )
  {
    case DSS_DNS_CONST_NET_TIMEOUT:
      if ( NULL == const_val || const_val_size < sizeof(uint32) )
      {
        LOG_MSG_ERROR_3("Insuff memory %d for const %d; const buffer = %p",
                        const_val_size, const_param, const_val );
        return -1;
      }
      *((uint32*)const_val) = DSS_DNS_NET_TIMEOUT_INTERVAL;
      break;

    case DSS_DNS_CONST_NET_RETRIES:
      if ( NULL == const_val || const_val_size < sizeof(uint32) )
      {
        LOG_MSG_ERROR_3("Insuff memory %d for const %d; const buffer = %p",
                        const_val_size, const_param, const_val );
        return -1;
      }
      *((uint32*)const_val) = DSS_DNS_MAX_NET_RETRIES;
      break;

    case DSS_DNS_CONST_DEFAULT_FIRST_QUERY_TIMEOUT:
      if ( NULL == const_val || const_val_size < sizeof(uint32) )
      {
        LOG_MSG_ERROR_3("Insuff memory %d for const %d; const buffer = %p",
                        const_val_size, const_param, const_val );
        return -1;
      }
      *((uint32*)const_val) = DSS_DNS_DEFAULT_FIRST_QUERY_TIMEOUT;
      break;

    case DSS_DNS_CONST_DEFAULT_MAX_QUERY_RETRIES:
      if ( NULL == const_val || const_val_size < sizeof(uint32) )
      {
        LOG_MSG_ERROR_3("Insuff memory %d for const %d; const buffer = %p",
                        const_val_size, const_param, const_val );
        return -1;
      }
      *((uint32*)const_val) = DSS_DNS_DEFAULT_MAX_QUERY_RETRIES;
      break;

    case DSS_DNS_CONST_DEFAULT_RETRIES_BACKOFF:
      if ( NULL == const_val || const_val_size < sizeof(uint32) )
      {
        LOG_MSG_ERROR_3("Insuff memory %d for const %d; const buffer = %p",
                        const_val_size, const_param, const_val );
        return -1;
      }
      *((uint32*)const_val) = DSS_DNS_DEFAULT_RETRIES_BACKOFF;
      break;

    default:
      LOG_MSG_ERROR_1( "Unknown parameter %d", const_param );
      return -1;
  }

  return 0;
} /* dss_dns_getconst() */


