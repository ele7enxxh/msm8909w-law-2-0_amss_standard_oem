/*===========================================================================

             D N S   H O S T E N T   A P I   M O D U L E

DESCRIPTION

  The Domain name services deprecated API module. Contains definitions of
  deprecated API functions like dss_getipnodebyname(), dss_getipnodebyaddr()
  and dss_freehostent(). All new applications should use the
  dss_dns_get_addrinfo and dss_dns_get_nameinfo API.

EXTERNALIZED FUNCTIONS

  dss_getipnodebyname()
    POSIX IPv6 compliant function returns the resolved IP address in
    dss_hostent structure given a host name.  Reference RFC 2553.

  dss_getipnodebyaddr()
    POSIX IPv6 compliant.  Returns resolved hostname in dss_hostent structure
    given a host IP address. Reference RFC 2553.

  dss_freehostent()
    Frees the dss_hostent structure returned by dss_getipnodebyname() and
    dss_getipnodebyaddr().  Reference RFC 2553.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2007-2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssvc/src/dss_dns_hostent.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/09   ts      Fix dss_getipnodebyname and dss_getipnodebyaddr to return 
                   correct errno in case dss_dns_get_nameinfo or
                   dss_dns_get_addrinfo returns invalid 
                   handle.
06/22/09   mt      Moved the code to resolve numerical addresses to happen
                   before cache checks. Fixed net policy settings.
06/07/09   ts      Fix dss_dnsi_construct_hostent_addr_results to allocate
                   correct size of memory for hostname. 
09/23/08   hm      Changed the hostent behavior to search for cache first
                   before creating a session.
02/15/08   dm      Fixed a crash that occurs when domain name validation 
                   fails
07/09/07   hm      Fixed a race condition where DNS API callback function was
                   being called before API function could return.
06/18/07   hm      Setting errno to DS_EHOSTHOTFOUND when num_records is 0
06/11/07   hm      Un-included dss_dns_config.h
03/27/07   hm      Changed the deprecated functions as wrappers around the
                   get_addrinfo and get_nameinfo APIs.
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"



#include <stringl/stringl.h>
#include "ps_dnsi_mem_mgr.h"
#include "dssdns.h"
#include "ps_dnsi_defs.h"
#include "ps_dnsi_msg_mgr.h"
#include "ps_iface_dns_cache.h"
#include "ps_iface.h"
#include "ps_mem.h"
#include "queue.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                         INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/* Forward declaration */
extern void dss_dnsi_construct_queries
(
  dss_dns_api_type_enum_type    api_type,
  uint16                        addr_family,
  uint32                        flags,
  const char                  * query_data_ptr,
  ps_dnsi_query_struct_type   * query_struct_ptr
);


/*===========================================================================
FUNCTION DSS_DNSI_HOSTENT_FREE()

DESCRIPTION
  Frees the hostent structure and associated memory

PARAMETERS
  hostent_ptr_ptr   - dss_hostent to be freed.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void dss_dnsi_hostent_free
(
  struct dss_hostent **hostent_ptr_ptr
)
{
  int index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == hostent_ptr_ptr || NULL == *hostent_ptr_ptr )
  {
    LOG_MSG_ERROR_0("Null argument passed" );
    return;
  }

  /*-------------------------------------------------------------------------
    Free the aliases list
  -------------------------------------------------------------------------*/
  if( NULL != (*hostent_ptr_ptr)->h_aliases )
  {
    index = 0;
    while( NULL != (*hostent_ptr_ptr)->h_aliases[index] )
    {
      PS_DNSI_MEM_FREE((*hostent_ptr_ptr)->h_aliases[index++]);
    } /* while */
    PS_DNSI_MEM_FREE((*hostent_ptr_ptr)->h_aliases);
  }

  /*-------------------------------------------------------------------------
    Free the address list
  -------------------------------------------------------------------------*/
  if( NULL != (*hostent_ptr_ptr)->h_addr_list )
  {
    index = 0;
    while( NULL != (*hostent_ptr_ptr)->h_addr_list[index] )
    {
      PS_DNSI_MEM_FREE((*hostent_ptr_ptr)->h_addr_list[index++]);
    } /* while */
    PS_DNSI_MEM_FREE((*hostent_ptr_ptr)->h_addr_list);
  }

  PS_DNSI_MEM_FREE((*hostent_ptr_ptr)->h_name);

  /*-------------------------------------------------------------------------
    Free the hostent
  -------------------------------------------------------------------------*/
  PS_DNSI_MEM_FREE(*hostent_ptr_ptr);

  return;
} /* dss_dnsi_hostent_free() */

/*===========================================================================
FUNCTION DSS_DNSI_CONSTRUCT_HOSTENT_ADDR_RESULTS()

DESCRIPTION
  Constructs a hostent structure from the given generic RR Q

PARAMETERS
  ques_type   - Type of query initially made.
  addr_family - Address family for the query.
  rr_q        - A queue of generic resource records.

RETURN VALUE
  A hostent struct containing the results.
  NULL if rr_q is NULL or memory cannot be allocated for hostent or
  any field of hostent.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static dss_hostent  * dss_dnsi_construct_hostent_addr_results
(
  uint16                          addr_family,
  char                          * query_data_ptr,
  dss_dns_nameinfo              * results_buf,
  uint16                          num_records,
  int16                         * dss_errno
)
{
  struct dss_hostent            * hostent_ptr = NULL;
  struct dss_dns_nameinfo       * ni_ptr = results_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == dss_errno )
  {
    LOG_MSG_ERROR_0("NULL dss_errno" );
    return NULL;
  }

  if( NULL == results_buf )
  {
    LOG_MSG_ERROR_0("NULL results_buf" );
    *dss_errno = DS_EFAULT;
    return NULL;
  }

  if( NULL == query_data_ptr )
  {
    LOG_MSG_ERROR_0("NULL query_data_ptr" );
    *dss_errno = DS_EFAULT;
    return NULL;
  }

  if( DSS_AF_INET != addr_family && DSS_AF_INET6 != addr_family )
  {
    LOG_MSG_ERROR_1("Invalid addr_family: %d", addr_family );
    *dss_errno = DS_EAFNOSUPPORT;
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Allocate memory for the hostent, aliases and address lists.  In case of
    failure return with DSS_ERROR and the error code set to DS_ENOMEM.
    Memset aliases and address array to zero
  -------------------------------------------------------------------------*/

  if( NULL != (hostent_ptr = ps_dnsi_mem_alloc( sizeof(struct dss_hostent) )) )
  {
    hostent_ptr->h_aliases =
      ps_dnsi_mem_alloc( 2 * sizeof(char *));

    hostent_ptr->h_addr_list =
      ps_dnsi_mem_alloc( 2 * sizeof(char *));
  }

  if( NULL == hostent_ptr ||
      NULL == hostent_ptr->h_aliases ||
      NULL == hostent_ptr->h_addr_list )
  {
    dss_dnsi_hostent_free( &hostent_ptr );
    *dss_errno = DS_ENOMEM;
    return NULL;
  }

  hostent_ptr->h_addrtype  = addr_family;
  hostent_ptr->h_length    = ( DSS_AF_INET == addr_family ) ?
                              sizeof(struct ps_in_addr)    :
                              sizeof(struct ps_in6_addr);

  memset( hostent_ptr->h_aliases, 0, sizeof(char*) * (2) );
  memset( hostent_ptr->h_addr_list, 0, sizeof(char*) * (2) );
  hostent_ptr->h_name = NULL;

  hostent_ptr->h_addr_list[0] = ps_dnsi_mem_alloc( hostent_ptr->h_length );
  hostent_ptr->h_name = ps_dnsi_mem_alloc( strlen(ni_ptr->hostname) + 1);

  if( NULL == hostent_ptr->h_addr_list[0] ||
      NULL == hostent_ptr->h_name )
  {
    dss_dnsi_hostent_free( &hostent_ptr );
    *dss_errno = DS_ENOMEM;
    return NULL;
  }

  (void) strlcpy (hostent_ptr->h_name,
                      ni_ptr->hostname,
                      DSS_DNS_MAX_DOMAIN_NAME_LEN );

  memscpy( hostent_ptr->h_addr_list[0],
    hostent_ptr->h_length,
    query_data_ptr, hostent_ptr->h_length );

  return hostent_ptr;
} /* dss_dnsi_construct_hostent_addr_results() */

/*===========================================================================
FUNCTION DSS_DNSI_CONSTRUCT_HOSTENT_NAME_RESULTS()

DESCRIPTION
  Constructs a hostent structure from the given generic RR Q

PARAMETERS
  ques_type   - Type of query initially made.
  addr_family - Address family for the query.
  rr_q        - A queue of generic resource records.

RETURN VALUE
  A hostent struct containing the results.
  NULL if rr_q is NULL or memory cannot be allocated for hostent or
  any field of hostent.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static dss_hostent  * dss_dnsi_construct_hostent_name_results
(
  uint16                          addr_family,
  char                          * query_data_ptr,
  dss_dns_addrinfo              * results_buf,
  uint16                          num_records,
  int16                         * dss_errno
)
{
  struct dss_hostent            * hostent_ptr = NULL;
  struct dss_dns_addrinfo       * ai_ptr;
  uint16                          num_aliases = 0;
  uint16                          num_address = 0;
  struct ps_in_addr             * temp_inaddr_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == dss_errno )
  {
    LOG_MSG_ERROR_0("NULL dss_errno" );
    return NULL;
  }

  if( NULL == results_buf )
  {
    LOG_MSG_ERROR_0("NULL results_buf" );
    *dss_errno = DS_EFAULT;
    return NULL;
  }

  if( NULL == query_data_ptr )
  {
    LOG_MSG_ERROR_0("NULL query_data_ptr" );
    *dss_errno = DS_EFAULT;
    return NULL;
  }

  if( DSS_AF_INET != addr_family && DSS_AF_INET6 != addr_family )
  {
    LOG_MSG_ERROR_1("Invalid addr_family: %d", addr_family );
    *dss_errno = DS_EAFNOSUPPORT;
    return NULL;
  }

  if( 0 >= num_records )
  {
    LOG_MSG_ERROR_1("Invalid num_records: %d", num_records );
    *dss_errno = DS_EFAULT;
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Loop through the addrinfo records to find number of elements in
    the alias and address array
  -------------------------------------------------------------------------*/
  for( ai_ptr = results_buf; ai_ptr < results_buf + num_records; ai_ptr++ )
  {
    /*-------------------------------------------------------------------
      dss_hostent can't mix v4 and v6 addresses.  Skip address type not
      queried for.
    -------------------------------------------------------------------*/
    if( addr_family == ai_ptr->ai_family )
    {
      num_address++;
    }
    /*-------------------------------------------------------------------
      Can safely dereference ai_ptr->ai_canonname since 
      ai_canonname is a static array inside dss_dns_addrinfo
      and ai_ptr was initialized to result buf which was tested for 
      not being NULL.
     -------------------------------------------------------------------*/
    if ( 0 != *(ai_ptr->ai_canonname) )
    {
      num_aliases++;
    }
  }

  /*-------------------------------------------------------------------------
    Allocate memory for the hostent, aliases and address lists.  In case of
    failure return with DSS_ERROR and the error code set to DS_ENOMEM.
    Memset aliases and address array to zero
  -------------------------------------------------------------------------*/
  if( NULL != (hostent_ptr = ps_dnsi_mem_alloc( sizeof(struct dss_hostent) )) )
  {
    hostent_ptr->h_aliases =
      ps_dnsi_mem_alloc((num_aliases + 1) * sizeof(char *));

    hostent_ptr->h_addr_list =
      ps_dnsi_mem_alloc((num_address + 1) * sizeof(char *));
  }

  if( NULL == hostent_ptr ||
      NULL == hostent_ptr->h_aliases ||
      NULL == hostent_ptr->h_addr_list )
  {
    dss_dnsi_hostent_free( &hostent_ptr );
    *dss_errno = DS_ENOMEM;
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Initialize aliases and addr_list arrays to zero.  Set h_name to NULL,
    h_addrtype and h_length to values.
  -------------------------------------------------------------------------*/
  memset( hostent_ptr->h_aliases, 0, sizeof(char*) * (num_aliases + 1) );
  memset( hostent_ptr->h_addr_list, 0, sizeof(char*) * (num_address + 1) );
  num_aliases              = 0;
  num_address              = 0;
  hostent_ptr->h_name      = NULL;
  hostent_ptr->h_addrtype  = addr_family;
  hostent_ptr->h_length    = ( DSS_AF_INET == addr_family ) ?
                              sizeof(struct ps_in_addr)    :
                              sizeof(struct ps_in6_addr);

  /*-------------------------------------------------------------------------
    Traverse the queue and fill up the hostent.
  -------------------------------------------------------------------------*/
  for( ai_ptr = results_buf; ai_ptr < results_buf + num_records; ai_ptr++ )
  {
    if( DSS_AF_INET == addr_family )
    {
      hostent_ptr->h_addr_list[num_address] =
        ps_dnsi_mem_alloc( sizeof(struct ps_in_addr) );
      if( NULL == hostent_ptr->h_addr_list[num_address] )
      {
        dss_dnsi_hostent_free( &hostent_ptr );
        *dss_errno = DS_ENOMEM;
        return NULL;
      }

      temp_inaddr_ptr =
        &(((struct ps_sockaddr_in *)&(ai_ptr->ai_sockaddr))->ps_sin_addr);

      memscpy( hostent_ptr->h_addr_list[num_address],
        sizeof(struct ps_in_addr),
        temp_inaddr_ptr, sizeof(struct ps_in_addr) );
      num_address++;
    }

    else // DSS_AF_INET6 == addr_family
    {
      hostent_ptr->h_addr_list[num_address] =
        ps_dnsi_mem_alloc( sizeof(struct ps_in6_addr) );
      if( NULL == hostent_ptr->h_addr_list[num_address] )
      {
        dss_dnsi_hostent_free( &hostent_ptr );
        *dss_errno = DS_ENOMEM;
        return NULL;
      }

      memscpy( hostent_ptr->h_addr_list[num_address],
        sizeof(struct ps_in6_addr),
        &(((struct ps_sockaddr_in6 *)&(ai_ptr->ai_sockaddr))->ps_sin6_addr),
        sizeof(struct ps_in6_addr));
      num_address++;
    }

    if( 0 != *(ai_ptr->ai_canonname) )
    {
      if( NULL == hostent_ptr->h_name )
      {
        hostent_ptr->h_name =
          ps_dnsi_mem_alloc(strlen(ai_ptr->ai_canonname) + 1);
        if( NULL == hostent_ptr->h_name )
        {
          dss_dnsi_hostent_free( &hostent_ptr );
          *dss_errno = DS_ENOMEM;
          return NULL;
        }

        (void) strlcpy (hostent_ptr->h_name,
                            ai_ptr->ai_canonname,
                            DSS_DNS_MAX_DOMAIN_NAME_LEN);
      }
      else
      {
        hostent_ptr->h_aliases[num_aliases] =
          ps_dnsi_mem_alloc(strlen(ai_ptr->ai_canonname) + 1);
        if( NULL == hostent_ptr->h_aliases[num_aliases] )
        {
          dss_dnsi_hostent_free( &hostent_ptr );
          *dss_errno = DS_ENOMEM;
          return NULL;
        }

        (void) strlcpy (hostent_ptr->h_aliases[num_aliases],
                            ai_ptr->ai_canonname,
                            DSS_DNS_MAX_DOMAIN_NAME_LEN);
        num_aliases++;
      }
    } /* if-else 0 != *(ai_ptr->ai_canonname) */

  } /* for */

  /*-------------------------------------------------------------------------
    If h_name is still NULL, copy the query data into h_name
  -------------------------------------------------------------------------*/
  if( NULL == hostent_ptr->h_name )
  {
    hostent_ptr->h_name =
      ps_dnsi_mem_alloc(strlen(query_data_ptr) + 1);
    if( NULL == hostent_ptr->h_name )
    {
      dss_dnsi_hostent_free( &hostent_ptr );
      *dss_errno = DS_ENOMEM;
      return NULL;
    }

    (void) strlcpy (hostent_ptr->h_name,
                        query_data_ptr,
                        DSS_DNS_MAX_DOMAIN_NAME_LEN );
  }

  return hostent_ptr;
} /* dss_dnsi_construct_hostent_name_results() */

/*===========================================================================
FUNCTION DSS_DNSI_HOSTENT_CONVERT_GENERIC_RR_TO_HOSTENT()

DESCRIPTION
  This function converts a generic RR Q to a hostent structure.

PARAMETERS
  result_q - Queue of generic RRs.

RETURN VALUE
  A hostent struct containing the results.
  NULL if result Q is empty.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static struct dss_hostent* dss_dnsi_hostent_convert_generic_rr_to_hostent 
(
  dss_dns_api_type_enum_type    api_type,
  q_type                      * result_q,
  char                        * query_data_ptr,
  char                        * addr_ptr
)
{
  ps_dnsi_generic_rr_type        * rr_node_ptr = NULL;
  dss_dns_addrinfo               * temp_addrinfo_ptr = NULL;
  dss_dns_nameinfo               * temp_nameinfo_ptr = NULL;
  uint16                           num_recs;
  struct dss_hostent             * hostent_ptr = NULL;
  uint16                           index = 0;
  struct ps_sockaddr_in            temp_sockaddr_in;
  struct ps_sockaddr_in6           temp_sockaddr_in6;
  int16                            addr_family = DSS_AF_INET;
  int16                            local_errno;
  int                              len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == result_q)
  {
    LOG_MSG_ERROR_0("NULL result_q" );
    return NULL;
  }
  
  num_recs = q_cnt (result_q);
  if (0 == num_recs)
  {
    return NULL;
  }

  if (DSS_DNS_API_ADDRINFO == api_type)
  {
    temp_addrinfo_ptr = ps_dnsi_mem_alloc (num_recs * sizeof (dss_dns_addrinfo));
    if (NULL == temp_addrinfo_ptr)
    {
      return NULL;
    }
  }
  else if (DSS_DNS_API_NAMEINFO == api_type)
  {
    temp_nameinfo_ptr = ps_dnsi_mem_alloc (num_recs * sizeof (dss_dns_nameinfo));
    if (NULL == temp_nameinfo_ptr)
    {
      return NULL;
    }
  }

  for (index = 0; index < num_recs; index++)
  {
   
    rr_node_ptr  = q_get(result_q);
    if (NULL == rr_node_ptr)
    {
      break;
    }

    if (DSS_DNS_API_ADDRINFO == api_type)
    {

      temp_addrinfo_ptr[index].ai_flags    = DSS_DNS_AI_FLAGS_NONE;
      temp_addrinfo_ptr[index].ai_socktype = 0;
      temp_addrinfo_ptr[index].ai_protocol = 0;
      (void) strlcpy (temp_addrinfo_ptr[index].ai_canonname, 
                          rr_node_ptr->domain_name, 
                          DSS_DNS_MAX_DOMAIN_NAME_LEN);
  
      if (PS_DNSI_QUERY_TYPE_A == rr_node_ptr->rr_type)
      {
        temp_addrinfo_ptr[index].ai_family   = DSS_AF_INET;

        /* This is bad logic but we only support one type in hostent */
        addr_family = DSS_AF_INET;
      
        memset (&temp_sockaddr_in, 0, sizeof (struct ps_sockaddr_in));
        temp_sockaddr_in.ps_sin_family = DSS_AF_INET;
        temp_sockaddr_in.ps_sin_port   = 0;
        memscpy (&temp_sockaddr_in.ps_sin_addr,
          sizeof(struct ps_in_addr),
          rr_node_ptr->rdata,
          sizeof(struct ps_in_addr));
  
        memscpy (&temp_addrinfo_ptr[index].ai_sockaddr,
          sizeof (struct ps_sockaddr_in),
          &temp_sockaddr_in,
          sizeof (struct ps_sockaddr_in));
      }
      else
      {
        temp_addrinfo_ptr[index].ai_family   = DSS_AF_INET6;

        /* This is bad logic but we only support one type in hostent */
        addr_family = DSS_AF_INET6;

        memset (&temp_sockaddr_in6, 0, sizeof (struct ps_sockaddr_in6));
        temp_sockaddr_in6.ps_sin6_family = DSS_AF_INET6;
        temp_sockaddr_in6.ps_sin6_port   = 0;
        memscpy (&temp_sockaddr_in6.ps_sin6_addr,
          sizeof (struct ps_in6_addr),
          rr_node_ptr->rdata,
          sizeof (struct ps_in6_addr));
  
        memscpy (&temp_addrinfo_ptr[index].ai_sockaddr,
          sizeof (struct ps_sockaddr_in6),
          &temp_sockaddr_in6,
          sizeof (struct ps_sockaddr_in6));
      }
    } 
    
    else if (DSS_DNS_API_NAMEINFO == api_type)
    {
      if (PS_DNSI_QUERY_TYPE_PTR == rr_node_ptr->rr_type)
      {
        len = MIN( rr_node_ptr->rdata_len,
                   (int16)sizeof(temp_nameinfo_ptr[index].hostname) );
        (void) strlcpy (temp_nameinfo_ptr[index].hostname,
                            (char *) rr_node_ptr->rdata,
                            len);
      }
    }

    PS_DNSI_MEM_FREE(rr_node_ptr->rdata);
    PS_MEM_FREE(rr_node_ptr);
  } /* for */
  
  /*-----------------------------------------------------------------------
    Hostent structure is a complext structure and has a lot of a lot of 
    memory allocations etc. These function takes care of converting the 
    addrinfo structure to hostent structure.
  -----------------------------------------------------------------------*/

  if (DSS_DNS_API_ADDRINFO == api_type)
  {
    hostent_ptr = 
      dss_dnsi_construct_hostent_name_results (addr_family,
                                               query_data_ptr,
                                               temp_addrinfo_ptr,
                                               num_recs,
                                               &local_errno);

    PS_DNSI_MEM_FREE(temp_addrinfo_ptr);
  }
  else if (DSS_DNS_API_NAMEINFO == api_type)
  {
    //TODO: Currently DSS_AF_INET is the only one supported.
    hostent_ptr = 
      dss_dnsi_construct_hostent_addr_results (DSS_AF_INET,
                                               (char *)addr_ptr,
                                               temp_nameinfo_ptr,
                                               num_recs,
                                               &local_errno);
    PS_DNSI_MEM_FREE(temp_nameinfo_ptr);
  }

  return hostent_ptr;

} /* dss_dnsi_hostent_convert_generic_rr_to_hostent() */

/*===========================================================================
FUNCTION DSS_DNSI_HOSTENT_TRY_NUMERICAL_RESOLVE()

DESCRIPTION
  Resolve the hostent information if the query is a numerical address

PARAMETERS
  query_data_ptr - Query
  query_type     - Type of query (A/AAAA)

RETURN VALUE
  A hostent struct containing the results.
  NULL if query cannot be locally resolved.

DEPENDENCIES
  Query must be of A or AAAA type.

SIDE EFFECTS
  None.
===========================================================================*/
static struct dss_hostent* dss_dnsi_hostent_try_numerical_resolve
(
  char                        *query_data_ptr,
  ps_dnsi_query_type_enum_type query_type
)
{
   struct dss_hostent *hostent_ptr = NULL;
   int16 local_errno;
   int16 result;
   q_type result_q;

   if( PS_DNSI_QUERY_TYPE_A != query_type && 
       PS_DNSI_QUERY_TYPE_AAAA != query_type )
   {
      LOG_MSG_ERROR_1("Invalid query_type: %d", query_type );
      return NULL;
   }

   memset(&result_q, 0, sizeof(result_q));
   (void) q_init (&result_q);

   result = ps_dnsi_msg_mgr_resolve_numerical_query (query_type,
                                                     PS_DNSI_QUERY_CLASS_IN,
                                                     query_data_ptr,
                                                     &result_q,
                                                     &local_errno);
   if (DSS_SUCCESS == result)
   {
      hostent_ptr = dss_dnsi_hostent_convert_generic_rr_to_hostent (
                                                     DSS_DNS_API_ADDRINFO,
                                                     &result_q, 
                                                     query_data_ptr,
                                                     NULL);
   }
   q_destroy(&result_q);
   return hostent_ptr;
}

/*===========================================================================
FUNCTION TYPEDEF DSS_DNSI_HOSTENT_CBACK

DESCRIPTION
  Callback function registered with the session manager. This function gets
  called when the API layer has completed the query operations.

PARAMETERS
  session_handle -  Identifies the session
  query_handle   -  Identifies query instance
  api_type       -  Type of resource record
  num_records    -  Number of records
  user_data_ptr  -  Points to the hostent control block.
  dss_errno      -  Error code if any

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dss_dnsi_hostent_cback
(
  dss_dns_session_mgr_handle_type   session_handle,
  dss_dns_query_handle_type         query_handle,
  dss_dns_api_type_enum_type        api_type,
  uint16                            num_records,
  void                            * user_data_ptr,
  int16                             dss_errno
)
{
  dss_dnsi_hostent_cb_type        * hostent_cb_ptr = NULL;
  void                            * results_buf = NULL;
  struct dss_hostent              * ret_hostent_ptr = NULL;
  int16                             local_errno = DSS_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_3("DNS API Cback recvd for query %d, num recs %d, errno %d ",
                  query_handle, num_records, dss_errno );

  if( NULL == user_data_ptr )
  {
    DATA_ERR_FATAL("NULL user data PTR");
    return;
  }

  if( !ps_mem_is_valid( user_data_ptr, PS_MEM_DNSI_HOSTENT_CB_TYPE ) )
  {
    LOG_MSG_ERROR_1("Invalid Hostent control block 0x%p", user_data_ptr);
    return;
  }

  hostent_cb_ptr = (dss_dnsi_hostent_cb_type *) user_data_ptr;
  if( session_handle != hostent_cb_ptr->session_handle ||
      api_type       != hostent_cb_ptr->api_type )
  {
    LOG_MSG_ERROR_2("Invalid session %d or API type %d in cback",
                    session_handle, api_type);
    return;
  }

  /*-------------------------------------------------------------------------
    The do-while loop is simply meant for jumping to end in case of error.
  -------------------------------------------------------------------------*/
  do
  {
    if( 0 == num_records )
    {
      local_errno = DS_EHOSTNOTFOUND;
      break;
    }

    if( DSS_DNS_API_ADDRINFO == api_type )
    {
      /*-------------------------------------------------------------------------
        Allocate a buffer for storing addrinfo structs.
      -------------------------------------------------------------------------*/
      results_buf = ps_dnsi_mem_alloc( num_records * sizeof(struct dss_dns_addrinfo) );
      if( NULL == results_buf )
      {
        LOG_MSG_ERROR_0("Cannot allocate enough memory for reading results");
        local_errno = DS_ENOMEM;
        break;
      }

      if( DSS_ERROR == dss_dns_read_addrinfo( session_handle,
                                              query_handle,
                                              (struct dss_dns_addrinfo *)results_buf,
                                              num_records,
                                              &local_errno ) )
      {
        LOG_MSG_ERROR_0("Read addrinfo failed.");
        break;
      }

      ret_hostent_ptr =
        dss_dnsi_construct_hostent_name_results( hostent_cb_ptr->addr_family,
                                                 hostent_cb_ptr->query_data_ptr,
                                                (struct dss_dns_addrinfo *) results_buf,
                                                num_records,
                                                &local_errno );
      if( NULL == ret_hostent_ptr )
      {
        LOG_MSG_ERROR_2("Construct hostent from results buf 0x%p failed error %d",
                        results_buf, local_errno);
        break;
      }

    }
    else if ( DSS_DNS_API_NAMEINFO == api_type )
    {
      /*-------------------------------------------------------------------------
        Allocate a buffer for storing nameinfo structs.
      -------------------------------------------------------------------------*/
      results_buf = ps_dnsi_mem_alloc( num_records * sizeof(struct dss_dns_nameinfo) );
      if( NULL == results_buf )
      {
        LOG_MSG_ERROR_0("Cannot allocate enough memory for reading results");
        local_errno = DS_ENOMEM;
        break;
      }

      if( DSS_ERROR == dss_dns_read_nameinfo( session_handle,
                                              query_handle,
                                              (struct dss_dns_nameinfo *)results_buf,
                                              num_records,
                                              &local_errno ) )
      {
        LOG_MSG_ERROR_0("Read addrinfo failed.");
        break;
      }

      ret_hostent_ptr =
        dss_dnsi_construct_hostent_addr_results( hostent_cb_ptr->addr_family,
                                                 hostent_cb_ptr->query_data_ptr,
                                                 (struct dss_dns_nameinfo *) results_buf,
                                                 num_records,
                                                 &local_errno );
      if( NULL == ret_hostent_ptr )
      {
        LOG_MSG_ERROR_2("Construct hostent from results buf 0x%p failed error %d",
                        results_buf, local_errno);
        break;
      }
    }
    else
    {
      LOG_MSG_ERROR_1("Invalid API type %d", api_type);
      ASSERT(0);
      break;
    }
  } while (0);

  PS_DNSI_MEM_FREE(results_buf);

  if( query_handle == hostent_cb_ptr->query_handle )
  {
    /*-----------------------------------------------------------------------
      This is the normal case, where the API function returned query handle
      to the application and then the callback was called.
    -----------------------------------------------------------------------*/

    if (NULL != hostent_cb_ptr->cback_f_ptr)
    {
      /*---------------------------------------------------------------------
        Legacy APIs allow NULL callbacks.
        In such a case, no callback would be called. Results get stored
        in the cache. 
      ---------------------------------------------------------------------*/
      hostent_cb_ptr->cback_f_ptr ( ret_hostent_ptr,
                                    hostent_cb_ptr->user_data_ptr,
                                    local_errno );
    }
    else
    {
      /* If a callback is NULL, must take care of ret_hostent_ptr */
      dss_dnsi_hostent_free ( &ret_hostent_ptr );
    }

    memset( hostent_cb_ptr, 0, sizeof(dss_dnsi_hostent_cb_type) );
    PS_MEM_FREE(hostent_cb_ptr);
    if( DSS_ERROR == dss_dns_delete_session( session_handle, &local_errno ))
    {
      LOG_MSG_ERROR_2("Can't delete session %d err %d",
                      session_handle, local_errno);
      return;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      This is a race condition case, where the callback was called before
      the API function could return query handle to the application.
      Do not call the callback function here.
    -----------------------------------------------------------------------*/
    hostent_cb_ptr->ret_hostent_ptr = ret_hostent_ptr;
  }
} /* dss_dnsi_hostent_cback */

/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSS_GETIPNODEBYNAME()

DESCRIPTION
  This function performs a DNS resolution on the passed domain name.  It
  returns the resolved mapping in a dss_hostent structure.  In case the
  domain name is a dotted decimal ipv4 or dotted decimal / colon seperated
  ipv6 address it is copied to the dss_hostent structure and returned.  If
  a mapping is not found the return value is NULL and the appropriate error
  code is set in dss_errno. The resolver would query the DNS servers and 
  return the mapping by invoking the callback function.

  The returned dss_hostent structure is freed by calling the
  dss_freehostent() function.

PARAMETERS
  hostname_ptr      string with name of an ipv4 or ipv6 host or dotted
                    decimal ipv4 or colon seperated/dotted decimal ipv6 host
  addr_family       address family of name.  DSS_AF_INET or DSS_AF_INET6
  flags             options - none currently supported.
  cback_f_ptr       callback function pointer
  user_data_ptr     voidptr passed in by the application.  This is returned
                    back to the application in the callback.
  dss_errno             error code returned

DEPENDENCIES
  None.

RETURN VALUE
  Address of a dss_hostent structure if successful.
  If no mapping found then returns a NULL and the *dss_errno is set to one of
  the following values.

  dss_errno Values
  ----------------
  DS_EHOSTNOTFOUND  -  domain name asked is not known.
  DS_ETRYAGAIN      -  temporary and transient error e.g., resolver out of
                       resources, server temporarily unreachable.
  DS_ENORECOVERY    -  irrecoverable server error occurred
  DS_EFAULT         -  invalid parameters passed to function
  DS_EAFNOSUPPORT   -  invalid value for the address family parameter
  DS_EOPNOTSUPP     -  invalid value for the flags parameter
  DS_ENOMEM         -  out of memory in the DNS subsystem
  DS_NAMEERR        -  domain name is malformed
  DS_EWOULDBLOCK    -  DNS servers being queried would invoke the callback
                       with the answer

SIDE EFFECTS
  None.
===========================================================================*/
struct dss_hostent *dss_getipnodebyname
(
  char                            * hostname_ptr,
  int32                             addr_family,
  int32                             flags,
  dss_dns_hostent_cback_f_type      cback_f_ptr,
  void                            * user_data_ptr,
  int16                           * dss_errno
)
{
  dss_dns_session_mgr_handle_type   session_handle;
  static dss_dns_addrinfo           hints_struct;
  dss_dnsi_hostent_cb_type        * hostent_cb_ptr = NULL;
  struct dss_hostent              * ret_hostent_ptr = NULL;
  struct dss_hostent              * cache_hostent_ptr = NULL;
  ps_dnsi_query_type_enum_type      query_type;
  int16                             local_errno;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_getipnodebyname(): "
    "caller 0x%p", caller_ptr);

  /*-------------------------------------------------------------------------
    Verify the function arguments
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_3("dss_getipnodebyname(): "
    "dss_getipnodebyname called with addr 0x%p, family %d, cback 0x%p",
    hostname_ptr, addr_family, cback_f_ptr);

  if( NULL ==  dss_errno )
  {
    LOG_MSG_ERROR_0("NULL dss_errno");
    return NULL;
  }

  if( NULL == hostname_ptr || 0 == *hostname_ptr )
  {
    LOG_MSG_ERROR_0("NULL hostname" );
    *dss_errno = DS_EFAULT;
    return NULL;
  }

  if( DSS_AF_INET != addr_family && DSS_AF_INET6 != addr_family )
  {
    LOG_MSG_ERROR_0("Invalid address family" );
    *dss_errno = DS_EAFNOSUPPORT;
    return NULL;
  }

  if( flags != 0 )
  {
    LOG_MSG_ERROR_1("Invalid flags 0x%x", flags );
    *dss_errno = DS_EOPNOTSUPP;
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Try to resolve the DNS query if the domain name is a numeric 
    representation of IPv4 or IPv6 addr.
  -------------------------------------------------------------------------*/
  query_type = (DSS_AF_INET == addr_family) ? 
                PS_DNSI_QUERY_TYPE_A:
                PS_DNSI_QUERY_TYPE_AAAA;

  cache_hostent_ptr = dss_dnsi_hostent_try_numerical_resolve (hostname_ptr, query_type);
  if (NULL != cache_hostent_ptr)
  {
     *dss_errno = DSS_SUCCESS;
     return cache_hostent_ptr;
  }

  /*-------------------------------------------------------------------------
    Create a hostent control block to store the query id and application
    callback information.
  -------------------------------------------------------------------------*/
  hostent_cb_ptr =
    (dss_dnsi_hostent_cb_type *) ps_mem_get_buf(PS_MEM_DNSI_HOSTENT_CB_TYPE);
  if( NULL == hostent_cb_ptr )
  {
    *dss_errno = DS_ENOMEM;
    return NULL;
  }
  memset( hostent_cb_ptr, 0, sizeof(dss_dnsi_hostent_cb_type) );
  hostent_cb_ptr->cback_f_ptr = cback_f_ptr;
  hostent_cb_ptr->user_data_ptr = user_data_ptr;
  hostent_cb_ptr->api_type = DSS_DNS_API_ADDRINFO;
  hostent_cb_ptr->addr_family = (uint16)addr_family;
  (void) strlcpy (hostent_cb_ptr->query_data_ptr,
                      hostname_ptr,
                      DSS_DNS_MAX_DOMAIN_NAME_LEN);
  hostent_cb_ptr->ret_hostent_ptr = NULL;

  /*-------------------------------------------------------------------------
    Create a session corresponding to the query.
  -------------------------------------------------------------------------*/
  session_handle = dss_dns_create_session( dss_dnsi_hostent_cback,
                                           (void *) hostent_cb_ptr,
                                           dss_errno );
  if( DSS_DNS_SESSION_MGR_INVALID_HANDLE == session_handle )
  {
    LOG_MSG_ERROR_1("Cannot create sesssion CB error %d", *dss_errno );
    PS_MEM_FREE(hostent_cb_ptr);
    return NULL;
  }
  hostent_cb_ptr->session_handle = session_handle;

  /*-------------------------------------------------------------------------
    Call get_addrinfo to start the query.
  -------------------------------------------------------------------------*/
  memset( &hints_struct, 0, sizeof(dss_dns_addrinfo) );
  hints_struct.ai_family = (uint16)addr_family;

  hostent_cb_ptr->query_handle = dss_dns_get_addrinfo( session_handle,
                                                       hostname_ptr,
                                                       NULL,
                                                       &hints_struct,
                                                       dss_errno );

  if( DSS_DNS_QUERY_INVALID_HANDLE == hostent_cb_ptr->query_handle )
  {
    LOG_MSG_ERROR_1("Error %d in call to get_addrinfo", *dss_errno);
    PS_MEM_FREE(hostent_cb_ptr);
    /*-------------------------------------------------------------------
      Use local_errno instead dss_errno 
      overriding the real failure cause stored on dss_errno
    -------------------------------------------------------------------*/
    if( DSS_ERROR == dss_dns_delete_session( session_handle, &local_errno ) )
    {
        LOG_MSG_ERROR_2("Cant delete session %d err %d",
                        session_handle, local_errno);
    }
    return NULL;
  }

  if( hostent_cb_ptr->ret_hostent_ptr != NULL )
  {
    /*-----------------------------------------------------------------------
      This is the case where a context switch occurred and
      dss_dnsi_hostent_cback function was called before get_addrinfo return.
    -----------------------------------------------------------------------*/
    ret_hostent_ptr = hostent_cb_ptr->ret_hostent_ptr;

    PS_MEM_FREE(hostent_cb_ptr);
    if( DSS_ERROR == dss_dns_delete_session( session_handle, dss_errno ) )
    {
      LOG_MSG_ERROR_2("Cant delete session %d err %d",
                      session_handle, *dss_errno);
    }

    return ret_hostent_ptr;
  }
  else
  {
    *dss_errno = DS_EWOULDBLOCK;
    return NULL;
  }
} /* dss_getipnodebyname() */

/*===========================================================================
FUNCTION DSS_GETIPNODEBYADDR()

DESCRIPTION
  This function performs reverse DNS resolutions on the passed host address
  returning the resolved mapping in a dss_hostent structure.  If a mapping
  is not found the return value is NULL and the appropriate error code is
  set in dss_errno. The resolver would query the DNS servers and
  return the mapping by invoking the callback function.

  The returned dss_hostent structure is freed by calling the
  dss_freehostent() function.

  At present support for IPv6 is not added.

PARAMETERS
  addr_ptr          based on the value of 'af' this will be either a
                    'struct ps_in_addr' (for af == DSS_AF_INET)  or 'struct
                    ps_in6_addr' (for af == DSS_AF_INET6)
  addr_len          length of the addr field
  addr_family       address family of addr DSS_AF_INET or DSS_AF_INET6
  cback_f_ptr       callback function pointer
  user_data_ptr     voidptr passed in by the application.  This is returned
                    back to the application in the callback.
  dss_errno             error code returned

DEPENDENCIES
  None.

RETURN VALUE
  Address of a hostent structure if successful.
  If no mapping found then returns a NULL and the *dss_errno is set to one of
  the following values.

  dss_errno Values
  ----------------
  DS_EHOSTNOTFOUND   - the specified host is unknown
  DS_ETRYAGAIN       - temporary and transient error e.g., resolver out of
                       resources, server temporarily unreachable.
  DS_EFAULT          - invalid parameters passed to function
  DS_EAFNOSUPPORT    - invalid value for the address family parameter
  DS_ENORECOVERY     - irrecoverable server error occurred
  DS_ENOMEM          - out of memory in the DNS subsystem
  DS_NAMEERR         - query is malformed
  DS_EWOULDBLOCK     - query being processed would invoke the callback
                       with the answer

SIDE EFFECTS
  None.
===========================================================================*/
struct dss_hostent *dss_getipnodebyaddr
(
  void                            * addr_ptr,
  int32                             addr_len,
  int32                             addr_family,
  dss_dns_hostent_cback_f_type      cback_f_ptr,
  void                            * user_data_ptr,
  int16                           * dss_errno
)
{
  dss_dns_session_mgr_handle_type   session_handle;
  struct ps_sockaddr_storage        query_sockaddr;
  struct ps_sockaddr_in           * temp_sockaddr_in_ptr;
  struct ps_sockaddr_in6          * temp_sockaddr_in6_ptr;
  int16                             query_len = 0;
  dss_dnsi_hostent_cb_type        * hostent_cb_ptr;
  static dss_dns_addrinfo           hints_struct;
  struct dss_hostent              * ret_hostent_ptr;
  int16                             local_errno;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_getipnodebyaddr(): "
    "caller 0x%p", caller_ptr);

  LOG_MSG_INFO2_3("dss_getipnodebyaddr(): "
    "dss_getipnodebyaddr called with addr 0x%p, family %d, cback 0x%p",
    addr_ptr, addr_family, cback_f_ptr);

  if( NULL ==  dss_errno )
  {
    LOG_MSG_ERROR_0("NULL dss_errno");
    return NULL;
  }

  if( NULL == addr_ptr || 0 >= addr_len )
  {
    LOG_MSG_ERROR_0("NULL address passed" );
    *dss_errno = DS_EFAULT;
    return NULL;
  }

  if( DSS_AF_INET  == addr_family && sizeof(struct ps_in_addr)  == addr_len )
  {
    memset( &query_sockaddr, 0, sizeof(struct ps_sockaddr_storage) );
    temp_sockaddr_in_ptr = (struct ps_sockaddr_in *) &query_sockaddr ;
    temp_sockaddr_in_ptr->ps_sin_family = DSS_AF_INET;
    temp_sockaddr_in_ptr->ps_sin_port = 0;
    memscpy( &(temp_sockaddr_in_ptr->ps_sin_addr), addr_len, addr_ptr, addr_len );
    query_len = sizeof(struct ps_sockaddr_in);
  }
  else if( DSS_AF_INET6 == addr_family && 
           sizeof(struct ps_in6_addr) == addr_len )
  {
    memset( &query_sockaddr, 0, sizeof(struct ps_sockaddr_storage) );
    temp_sockaddr_in6_ptr = (struct ps_sockaddr_in6 *) &query_sockaddr ;
    temp_sockaddr_in6_ptr->ps_sin6_family = DSS_AF_INET6;
    temp_sockaddr_in6_ptr->ps_sin6_port = 0;
    memscpy( &(temp_sockaddr_in6_ptr->ps_sin6_addr), addr_len, addr_ptr, addr_len );
    query_len = sizeof(struct ps_sockaddr_in6);
  }
  else
  {
    LOG_MSG_ERROR_2("Invalid address family %d or len %d",
                    addr_family, addr_len );
    *dss_errno = DS_EAFNOSUPPORT;
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Create a hostent control block to store the query id and application
    callback information.
  -------------------------------------------------------------------------*/
  hostent_cb_ptr =
    (dss_dnsi_hostent_cb_type *) ps_mem_get_buf(PS_MEM_DNSI_HOSTENT_CB_TYPE);
  if( NULL == hostent_cb_ptr )
  {
    *dss_errno = DS_ENOMEM;
    return NULL;
  }
  memset( hostent_cb_ptr, 0, sizeof(dss_dnsi_hostent_cb_type) );
  hostent_cb_ptr->cback_f_ptr = cback_f_ptr;
  hostent_cb_ptr->user_data_ptr = user_data_ptr;
  hostent_cb_ptr->api_type = DSS_DNS_API_NAMEINFO;
  hostent_cb_ptr->addr_family = (uint16) addr_family;
  memscpy(hostent_cb_ptr->query_data_ptr, addr_len, addr_ptr, addr_len);
  hostent_cb_ptr->ret_hostent_ptr = NULL;

  session_handle = dss_dns_create_session( dss_dnsi_hostent_cback,
                                           (void *) hostent_cb_ptr,
                                           dss_errno );
  if( DSS_DNS_SESSION_MGR_INVALID_HANDLE == session_handle )
  {
    LOG_MSG_ERROR_1("Cannot create sesssion CB error %d", *dss_errno );
    PS_MEM_FREE(hostent_cb_ptr);
    return NULL;
  }

  hostent_cb_ptr->session_handle = session_handle;

  /*-------------------------------------------------------------------------
    Call get_nameinfo to start the query.
  -------------------------------------------------------------------------*/
  memset( &hints_struct, 0, sizeof(dss_dns_addrinfo) );
  hints_struct.ai_family = (uint16) addr_family;

  hostent_cb_ptr->query_handle =
    dss_dns_get_nameinfo( session_handle,
                          (struct ps_sockaddr *) &query_sockaddr,
                          query_len,
                          0,
                          dss_errno );

  if( DSS_DNS_QUERY_INVALID_HANDLE == hostent_cb_ptr->query_handle )
  {
    LOG_MSG_ERROR_1("Error %d in call to get_nameinfo", *dss_errno);
    PS_MEM_FREE(hostent_cb_ptr);
    /*-------------------------------------------------------------------
      Use local_errno instead dss_errno 
      overriding the real failure cause stored on dss_errno
    -------------------------------------------------------------------*/
    if( DSS_ERROR == dss_dns_delete_session( session_handle, &local_errno ) )
    {
        LOG_MSG_ERROR_2("Cant delete session %d err %d",
                        session_handle, local_errno);
    }
    return NULL;
  }

  if( hostent_cb_ptr->ret_hostent_ptr != NULL )
  {
    /*-----------------------------------------------------------------------
      This is the case where a context switch occurred and
      dss_dnsi_hostent_cback function was called before get_nameinfo return.
    -----------------------------------------------------------------------*/
    ret_hostent_ptr = hostent_cb_ptr->ret_hostent_ptr;

    PS_MEM_FREE(hostent_cb_ptr);
    if( DSS_ERROR == dss_dns_delete_session( session_handle, dss_errno ) )
    {
      LOG_MSG_ERROR_2("Cant delete session %d err %d",
                      session_handle, *dss_errno);
    }

    return ret_hostent_ptr;
  }
  else
  {
    *dss_errno = DS_EWOULDBLOCK;
    return NULL;
  }
} /* dss_getipnodebyaddr() */

/*===========================================================================
FUNCTION DSS_FREEHOSTENT()

DESCRIPTION
  This function is called to free the dss_hostent structure returned by
  dss_getipnodebyname() or dss_getipnodebyaddr() functions.

PARAMETERS
  hostent_ptr    pointer to dss_hostent structure to be freed

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Frees all the memory associated with the passed dss_hostent structure.
===========================================================================*/
void dss_freehostent
(
  struct dss_hostent *hostent_ptr
)
{
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
  LOG_MSG_FUNCTION_ENTRY_1("dss_freehostent(): "
    "caller 0x%p", caller_ptr);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1("dss_freehostent called with hostent ptr 0x%p", hostent_ptr);
  /*-------------------------------------------------------------------------
    Free memory associated with the hostent.
  -------------------------------------------------------------------------*/
  if( NULL != hostent_ptr )
  {
    dss_dnsi_hostent_free( &hostent_ptr );
  }

  return;
} /* dss_freehostent() */

