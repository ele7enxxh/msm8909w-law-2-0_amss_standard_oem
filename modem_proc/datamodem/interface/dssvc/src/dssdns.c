/*===========================================================================

             D O M A I N   N A M E   S E R V I C E S   M O D U L E

DESCRIPTION

 The Domain Name Services mobile API File. Contains basic API functions for
 DNS Lookup.

EXTERNALIZED FUNCTIONS

  dss_dns_init()
    Initializes the DNS subsystem during powerup.

  dss_dns_get_addrinfo()
    Queries the resolver asyncrhonously for a list of addrinfo structs.
    Reference RFC 3493.

  dss_dns_get_nameinfo()
    Queries the resolver asyncrhonously for a list of nameinfo structs.
    Reference RFC 3493.

  dss_dns_read_addrinfo()
    Function to read the the addrinfo structs. The memory for the addrinfo
    structs should be provided by the application.

  dss_dns_read_nameinfo()
    Function to read the nameinfo structs. The memory for the name
    structs should be provided by the application.

  dss_dns_clear_cache()
    Clears the cache of the specified iface.

  dss_dns_clear_cache_entry()
    Clears the specified host name from the cache 
    of the specified iface.

Copyright (c) 2007-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssvc/src/dssdns.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/28/11   hs      Protect from accessing api_cb from application task
                   while it is freed by PS task.
11/09/11   ea      Protect from performing two queries in parallel with the
                   same session
06/15/11   ea      Remove the critical section from
                   ps_dnsi_api_resolver_event_notify().
07/09/09   mt      Added additional rules to validate_hostname_query to stop
                   numerical queries early that will not succeed.
05/14/09   pp      Moved Local Heap allocations to Modem Heap.
08/29/08   am      Corrected Low WM counts for Low-tier builds.
02/15/08   dm      Fixed a crash that occurs when domain name validation
                   fails
06/18/07   hm      Deleting API CB after notifying error condition;
                   Returning EBADF instead of EFAULT for invalid handles.
06/11/07   hm      Fixed CNAME related bug in ai_construct_results.
05/18/07   hm      Moved deprecated hostent API to another file
03/27/07   hm      Initial development.

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>
#include "ps_handle_mgr.h"
#include "ps_mem.h"
#include "ps_dnsi.h"
#include "ps_dnsi_mem_mgr.h"
#include "dss_dns_config.h"
#include "dssdns.h"
#include "dss_dns_priv.h"
#include "ps_dnsi_defs.h"
#include "ps_dnsi_resolver.h"
#include "ps_iface.h"
#include "ps_iface_dns_cache.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                            LOCAL DATA DECLARATIONS

===========================================================================*/
#define API_CLIENT    (PS_HANDLE_MGR_CLIENT_DNS_API)
#define PS_DNSI_MAX_PORTNUM 65535

/*---------------------------------------------------------------------------
  Tuning the number of ps dnsi api callback buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_DNSI_API_CB_BUF_SIZE  ((sizeof(ps_dnsi_api_cb_type) + 3) & ~3)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

#define PS_DNSI_API_CB_BUF_NUM        DSS_DNS_MAX_QUERIES
#define PS_DNSI_API_CB_BUF_HIGH_WM    ((PS_DNSI_API_CB_BUF_NUM) - 1)
#define PS_DNSI_API_CB_BUF_LOW_WM     1

#else

#define PS_DNSI_API_CB_BUF_NUM        DSS_DNS_MAX_QUERIES
#define PS_DNSI_API_CB_BUF_HIGH_WM    ((PS_DNSI_API_CB_BUF_NUM) - 1)
#define PS_DNSI_API_CB_BUF_LOW_WM     1
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_dnsi_api_cb along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_dnsi_api_cb_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                  (
                                    PS_DNSI_API_CB_BUF_NUM,
                                    PS_DNSI_API_CB_BUF_SIZE
                                  )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ps_dnsi_api_cb_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type * ps_dnsi_api_cb_buf_hdr[PS_DNSI_API_CB_BUF_NUM];
static ps_dnsi_api_cb_type * ps_dnsi_api_cb_buf_ptr[PS_DNSI_API_CB_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of ps dnsi session manager callback buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_DNSI_SESSION_MGR_CB_BUF_SIZE  ((sizeof(ps_dnsi_session_mgr_cb_type) + 3) & ~3)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

#define PS_DNSI_SESSION_MGR_CB_BUF_NUM        DSS_DNS_MAX_SESSIONS
#define PS_DNSI_SESSION_MGR_CB_BUF_HIGH_WM    ((PS_DNSI_SESSION_MGR_CB_BUF_NUM) - 1)
#define PS_DNSI_SESSION_MGR_CB_BUF_LOW_WM     1

#else

#define PS_DNSI_SESSION_MGR_CB_BUF_NUM        DSS_DNS_MAX_SESSIONS
#define PS_DNSI_SESSION_MGR_CB_BUF_HIGH_WM    ((PS_DNSI_SESSION_MGR_CB_BUF_NUM) - 1)
#define PS_DNSI_SESSION_MGR_CB_BUF_LOW_WM     1
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_dnsi_session_mgr_cb along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_dnsi_session_mgr_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                       (
                                         PS_DNSI_SESSION_MGR_CB_BUF_NUM,
                                         PS_DNSI_SESSION_MGR_CB_BUF_SIZE
                                       )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ps_dnsi_session_mgr_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type         * ps_dnsi_session_mgr_cb_buf_hdr[PS_DNSI_SESSION_MGR_CB_BUF_NUM];
static ps_dnsi_session_mgr_cb_type * ps_dnsi_session_mgr_cb_buf_ptr[PS_DNSI_SESSION_MGR_CB_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of DNS servers(addr) buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_DNSI_SESSION_DNS_SERVERS_BUF_SIZE  ((sizeof(ps_dnsi_addr_node_type) + 3) & ~3)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET
/*---------------------------------------------------------------------------
  Define number of DNS servers per session. 
---------------------------------------------------------------------------*/
#define PS_DNSI_SESSION_DNS_SERVERS_PER_SESSION 3
/*---------------------------------------------------------------------------
  Calculate the total the number of required buffers based on
  the servers per session and max sessions(PS_DNSI_SESSION_MGR_CB_BUF_NUM).
---------------------------------------------------------------------------*/
#define PS_DNSI_SESSION_DNS_SERVERS_BUF_NUM   \
    (PS_DNSI_SESSION_DNS_SERVERS_PER_SESSION*PS_DNSI_SESSION_MGR_CB_BUF_NUM)

#define PS_DNSI_SESSION_DNS_SERVERS_BUF_HIGH_WM    2
#define PS_DNSI_SESSION_DNS_SERVERS_BUF_LOW_WM     1

#else

/* Define number of DNS servers per session */
#define PS_DNSI_SESSION_DNS_SERVERS_PER_SESSION 3
/*---------------------------------------------------------------------------
Calculate the total the number of required buffers based on
the servers per session and max sessions(PS_DNSI_SESSION_MGR_CB_BUF_NUM).
---------------------------------------------------------------------------*/
#define PS_DNSI_SESSION_DNS_SERVERS_BUF_NUM   \
  (PS_DNSI_SESSION_DNS_SERVERS_PER_SESSION*PS_DNSI_SESSION_MGR_CB_BUF_NUM)

#define PS_DNSI_SESSION_DNS_SERVERS_BUF_HIGH_WM    2
#define PS_DNSI_SESSION_DNS_SERVERS_BUF_LOW_WM     1

#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
Allocate memory to hold DNS server address along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_dnsi_session_dns_servers_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
(
 PS_DNSI_SESSION_DNS_SERVERS_BUF_NUM,
 PS_DNSI_SESSION_DNS_SERVERS_BUF_SIZE
 )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
Array of pointers used to facilitate easy debugging.  The first one points
to the ps_mem header and the latter points to ps_dnsi_addr_node_type
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type    * ps_dnsi_session_dns_servers_buf_hdr[PS_DNSI_SESSION_DNS_SERVERS_BUF_NUM];
static ps_dnsi_addr_node_type * ps_dnsi_session_dns_servers_buf_ptr[PS_DNSI_SESSION_DNS_SERVERS_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of ps dnsi hostent callback buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_DNSI_HOSTENT_CB_BUF_SIZE  ((sizeof(dss_dnsi_hostent_cb_type) + 3) & ~3)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

#define PS_DNSI_HOSTENT_CB_BUF_NUM        DSS_DNS_MAX_QUERIES
#define PS_DNSI_HOSTENT_CB_BUF_HIGH_WM    ((PS_DNSI_HOSTENT_CB_BUF_NUM) - 1)
#define PS_DNSI_HOSTENT_CB_BUF_LOW_WM     2

#else

#define PS_DNSI_HOSTENT_CB_BUF_NUM        DSS_DNS_MAX_QUERIES
#define PS_DNSI_HOSTENT_CB_BUF_HIGH_WM    ((PS_DNSI_HOSTENT_CB_BUF_NUM) - 1)
#define PS_DNSI_HOSTENT_CB_BUF_LOW_WM     2
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_dnsi_hostent_cb along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_dnsi_hostent_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                   (
                                     PS_DNSI_HOSTENT_CB_BUF_NUM,
                                     PS_DNSI_HOSTENT_CB_BUF_SIZE
                                   )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ps_dnsi_hostent_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type       * ps_dnsi_hostent_cb_buf_hdr[PS_DNSI_HOSTENT_CB_BUF_NUM];
static dss_dnsi_hostent_cb_type  * ps_dnsi_hostent_cb_buf_ptr[PS_DNSI_HOSTENT_CB_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */


/*===========================================================================

                         INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Forward declarations for internal session management functions
---------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION  DSS_DNS_SESSION_MGR_INIT

DESCRIPTION
  Initializes the session manager module.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void dss_dns_session_mgr_init
(
  void
);

/*===========================================================================
FUNCTION DSS_DNS_SESSION_MGR_IS_HANDLE_VALID

DESCRIPTION
  Checks if the specified session handle is valid or not.

PARAMETERS
  session_handle  - Session manager control block handle

RETURN VALUE
  TRUE if the handle is valid
  FALSE if the handle is not valid.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean dss_dns_session_mgr_is_handle_valid
(
  dss_dns_session_mgr_handle_type     session_handle
);

/*===========================================================================
FUNCTION DSS_DNS_SESSION_MGR_GET_CBACK_INFO

DESCRIPTION
  Gets the callback information from the session manager.

PARAMETERS
  session_handle    - Session manager control block handle
  cback_f_ptr_ptr   - Placeholder for callback function
  user_data_ptr_ptr - Placeholder for application user data

RETURN VALUE
  DSS_SUCCESS - if the session is valid.
  DSS_ERROR   - if the session handle specified is invalid.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern int16 dss_dns_session_mgr_get_cback_info
(
  dss_dns_session_mgr_handle_type      session_handle,
  dss_dns_cback_f_type               * cback_f_ptr_ptr,
  void                              ** user_data_ptr_ptr
);

/*===========================================================================
FUNCTION DSS_DNS_SESSION_MGRI_GET_CB_PTR

DESCRIPTION
  Checks if the specified handle is valid and if it is valid gets the
  control block corresponding to the handle.

PARAMETERS
  session_handle  - Session manager control block handle

RETURN VALUE
  Pointer to the control block if the specified handle is valid.
  NULL if the specified handle is not valid.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern ps_dnsi_session_mgr_cb_type  * dss_dns_session_mgri_get_cb_ptr
(
  dss_dns_session_mgr_handle_type     session_handle
);


/*===========================================================================
FUNCTION DSS_DNSI_ATOI

DESCRIPTION
  Unsigned, radix-10 custom implementation of atoi. If there was an atoi
  in AEE_Stdlib we would not need this.

PARAMETERS
  s - string to convert

RETURN VALUE
  True if conversion is successful.
  Value is set to conversion's value.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean dss_dnsi_atoi
(  
   const char *s,
   uint32 *value
)
{
   char c;
   uint32 val = 0;

   if ((s == NULL) || (value == NULL))
   {
      return FALSE;
   }

   while ( (c = *s++) != '\0')
   {
      if ((c < '0') || (c > '9')) // not a digit
      {
         return FALSE;
      }

      if ((val*10 + c - '0') > PS_DNSI_MAX_PORTNUM) // number is becoming too large
      {
         return FALSE;
      }

      if ((val == 0) && (c == '0')) // do not allow leading 0
      {
         return FALSE;
      }

      val = val*10 + c-'0';
   }

   *value = val;
   return TRUE;
}


/*===========================================================================
FUNCTION DSS_DNSI_PARSE_SERVICE_STRING()

DESCRIPTION
  Validate and parse a service string.

  According to RFC 2553 and to POSIX spec of getaddrinfo(), "service" is a 
  string of either a port number or a service name.

  If the input is a numeric string of allowed range (1-65535), it will be 
  parsed, converted, and returned. If the input is NULL, a special value
  PS_DNSI_SERVICE_NOT_SPECIFIED will be returned and the conversion will be
  successful. All other inputs will be rejected.

  Service names are not currently supported.

PARAMETERS
  service_ptr - Pointer to service string.
  service     - Pointer to the conversion result (output).

RETURN VALUE
  TRUE on parse success:
  - service is either a valid port number or PS_DNSI_SERVICE_NOT_SPECIFIED.

  FALSE on parse fail (string does not contain a valid service specifier):
  - service is undefined.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean dss_dnsi_parse_service_string
(
   const char * service_ptr,
   uint16 * service
)
{
   uint32 atoi_result;

   if (NULL == service_ptr) 
   {
      *service = PS_DNSI_SERVICE_NOT_SPECIFIED;
      return TRUE;
   }

   if (dss_dnsi_atoi(service_ptr, &atoi_result))
   {
      if ((atoi_result > 0) && (atoi_result <= PS_DNSI_MAX_PORTNUM))
      {
         *service = (uint16)atoi_result;
         return TRUE;
      }
   }

   /* Service names are not currently supported. If/when they will be, this
   function will also need to receive the hints and check for 
   AI_NUMERICHOST--if set, then by Posix def. of getaddrinfo only numeric 
   service is allowed. */

   return FALSE;
} /* dss_dnsi_parse_service_string */


/*===========================================================================
FUNCTION DSS_DNSI_VALIDATE_HOSTNAME_QUERY()

DESCRIPTION
  Validates a domain name

  A domain name is made up of one or more label names seperated by '.'.

  Valid characters allowed in the domain name are a-z, A-Z, 0-9
  and '-'(hyphen). The beginning character of the domain name and of labels
  should be an aplphabet.

  The length of the labels should not exceed DSS_DNS_MAX_LABEL_LEN.
  The length of the entire domain name should not exceed
  DSS_DNS_MAX_DOMAIN_LEN.

  An additional check is performed when hints_ptr is not NULL: when the 
  hostname is a numerical address, the corresponding address family or 
  AF_UNSPEC must be specified in the hints structure. Passing a different
  address family in the hints structure fails the validation, as such a query
  would not succeed.

PARAMETERS
  domain_name_ptr   - Pointer to domain name string.
  hints_ptr         - Pointer to the hints structure.

RETURN VALUE
  TRUE on Success or if the domain name argument is NULL.
  FALSE if the domain name is invalid

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean dss_dnsi_validate_hostname_query
(
  const char      * hostname_ptr,
  const struct dss_dns_addrinfo * hints_ptr
)
{
  uint16             index;
  int32              retval = DSS_ERROR;
  unsigned char      curr_char;
  unsigned char      next_char;
  struct ps_in_addr  temp_in_addr;
  struct ps_in6_addr temp_in6_addr;
  int16              dss_errno;
  int16              label_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Return true for NULL hostname.
  -------------------------------------------------------------------------*/
  if( NULL == hostname_ptr )
  {
    LOG_MSG_INFO1_0("NULL domain name");
    return TRUE;
  }

  /*-----------------------------------------------------------------------
    Check if hostname is a IPv4 dotted decimal string.
  -----------------------------------------------------------------------*/
  retval = dss_inet_pton( hostname_ptr,
                          DSS_AF_INET,
                          &temp_in_addr,
                          sizeof(struct ps_in_addr),
                          &dss_errno );
  if( DSS_SUCCESS == retval )
  {
    if (hints_ptr != NULL)
    {
      // See "additional check" in the description
      if (DSS_AF_INET6 == hints_ptr->ai_family)
      {
         LOG_MSG_INFO1_0("Numeric query/address family mismatch.");
         return FALSE;
      }
    }
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Check if hostname is a IPv6 colon seperated hex string.
  -------------------------------------------------------------------------*/
  retval = dss_inet_pton( hostname_ptr,
                          DSS_AF_INET6,
                          &temp_in6_addr,
                          sizeof(struct ps_in6_addr),
                          &dss_errno );
  if( DSS_SUCCESS == retval )
  {
    if (hints_ptr != NULL)
    {
      // See "additional check" in the description
      if (DSS_AF_INET == hints_ptr->ai_family)
      {
         LOG_MSG_INFO1_0("Numeric query/address family mismatch.");
         return FALSE;
      }
    }
    return TRUE;
  }

  /* At this point, numerical queries have already returned. If NUMERICHOST
  flag is set, then other types of hostnames are not allowed */
  /*lint -save -e655 Reason for suppressing error 655*/
  if ((hints_ptr != NULL) && (hints_ptr->ai_flags & DSS_DNS_AI_FLAGS_NUMERICHOST))
  /*lint -restore Restore lint error 655*/
  {
     LOG_MSG_INFO1_0("AI_FLAGS_NUMERICHOST specified, but hostname is not numeric.");
     return FALSE;
  }

  /*-------------------------------------------------------------------------
    Validate that hostname conforms to DNS domain naming guidelines
      1. The first and last character of each label must be either a letter
         or digit
      2. Rest of the characters in a label must be a letter or a digit or a
         hyphen
      3. Length of each label must be <= 63 characters
      4. Length of domain name must be <= 255 characters
  -------------------------------------------------------------------------*/
  label_len = 0;
  if( !ps_isalnum(*hostname_ptr) )
  {
    LOG_MSG_INFO1_0("Label must start with alphabet or digit");
    return FALSE;
  }

  label_len++;

  for( index = 1; index < ( DSS_DNS_MAX_DOMAIN_NAME_LEN - 1 ); index++ )
  {
    curr_char = hostname_ptr[index];

    if( '\0' == curr_char )
    {
      break;
    }
    else if( '.' == curr_char )
    {
      /*---------------------------------------------------------------------
        Reached end of a label. Make sure that length is valid
      ---------------------------------------------------------------------*/
      if( DSS_DNS_MAX_LABEL_LEN <= label_len )
      {
        LOG_MSG_INFO1_1("Label length %d exceeds the max len", label_len );
        return FALSE;
      }

      /*---------------------------------------------------------------------
        Reset the label length and start counting for next label
      ---------------------------------------------------------------------*/
      label_len = 0;

      /*---------------------------------------------------------------------
        Make sure that either we reached end of domain name or first character
        of next label is valid
      ---------------------------------------------------------------------*/
      next_char = hostname_ptr[++index];

      if( '\0' == next_char )
      {
        break;
      }
      else if( !ps_isalnum( next_char ) )
      {
        LOG_MSG_INFO1_0("Label must start with alphabet or digit");
        return FALSE;
      }
    }
    else if( !ps_isalnum( curr_char ) && '-' != curr_char )
    {
      LOG_MSG_INFO1_1("Invalid character %d in domain name", curr_char);
      return FALSE;
    }

    label_len++;
  }

  if(( DSS_DNS_MAX_DOMAIN_NAME_LEN - 1 ) == index )
  {
    /*-----------------------------------------------------------------------
      This means that domain name is of max possible length. Make sure that
        1. Domain name is NULL terminated
        2. Domain name ends with '.'. DNS mandates that a domain name must end
           with '.'. Typically user doesn't do it and hence the resolver
           appends '.' before querying the DNS server. If domian is already of
           max length, resolver can't append '.'
    -----------------------------------------------------------------------*/
    if( '\0' != hostname_ptr[index] )
    {
      LOG_MSG_INFO1_0("Domain name is not null-terminated");
      return FALSE;
    }
    else if( '.' != hostname_ptr[index - 1] )
    {
      LOG_MSG_INFO1_0("Passed max length domain name - doesn't end with '.'");
      return FALSE;
    }
  }

  return TRUE;
} /* dss_dnsi_validate_hostname_query() */

/*===========================================================================
FUNCTION DSS_DNSI_CONSTRUCT_QUERIES()

DESCRIPTION
  Constructs a hostname or pointer query for the given api type.
  For get_addrinfo API, constructs A and/or AAAA queries.
  For get_nameinfo API, constucts IPv4 or IPv6 PTR query
  depending upon the address family.

  An IPv4 query is represented as a sequence of bytes in reverse order,
  represented as decimals, seperated by dots with suffix in-addr.arpa.
  Ex: a.b.c.d --> d.c.b.a.in-addr.arpa.

  An IPv6 query is represented as a sequence of nibbles in reverse order,
  represented as hex digits, seperated by dots with suffix ip6.arpa.
  Ex: abcd::efgh ->
  h.g.f.e.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.d.c.b.a.ip6.arpa.

PARAMETERS
  api_type         - Type of API requesting the construction of query.
                     Depending upon this we could construct a hostname query
                     or a pointer query.
  addr_family      - Address family.
  flags            - Set of possible flags.
  query_data_ptr   - Pointer to the array containing the query.
  query_struct_ptr - Struct containing number of queries created, query type
                     and query class for each query and a query data array.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dss_dnsi_construct_queries
(
  dss_dns_api_type_enum_type    api_type,
  uint16                        addr_family,
  uint32                        flags,
  const char                  * query_data_ptr,
  ps_dnsi_query_struct_type   * query_struct_ptr
)
{
  struct ps_in_addr  temp_in_addr;
  struct ps_in6_addr temp_in6_addr;

  /* Data structure to convert a nibble to hexadecimal printable character */
  unsigned char     hex_to_char[]={ '0','1','2','3','4','5','6','7',
                                    '8','9','A','B','C','D','E','F' };
  int16             msg_index = 0;
  int16             index = 0;
  uint8             temp_nibble = 0;
  boolean           is_numeric_ipv4 = FALSE;
  boolean           is_numeric_ipv6 = FALSE;
  int32             retval = DSS_ERROR;
  int16             dss_errno = 0;
  struct ps_sockaddr_in6 temp_sockaddr_in6;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Sanity check
  -------------------------------------------------------------------------*/
  if( DSS_AF_INET   != addr_family &&
      DSS_AF_INET6  != addr_family &&
      DSS_AF_UNSPEC != addr_family )
  {
    LOG_MSG_ERROR_1("Invalid addr family %d", addr_family);
    ASSERT(0);
    return;
  }

  if( DSS_DNS_API_ADDRINFO != api_type &&
      DSS_DNS_API_NAMEINFO != api_type )
  {
    LOG_MSG_ERROR_1("Invalid API type %d", api_type);
    ASSERT(0);
    return;
  }

  if( NULL == query_struct_ptr )
  {
    DATA_ERR_FATAL("NULL query struct");
    return;
  }

  /*-------------------------------------------------------------------------
    Fill up the query type, query class and query data
  -------------------------------------------------------------------------*/
  if( DSS_DNS_API_ADDRINFO == api_type )
  {
    memset( query_struct_ptr, 0, sizeof( ps_dnsi_query_struct_type ) );

    if( NULL != query_data_ptr )
    // check if the query is numerical
    {
       retval = dss_inet_pton( query_data_ptr,
                               DSS_AF_INET,
                               &temp_in_addr,
                               sizeof(struct ps_in_addr),
                               &dss_errno );
       if( DSS_SUCCESS == retval )
       {
          is_numeric_ipv4 = TRUE;
       }
       else
       {
          retval = dss_inet_pton( query_data_ptr,
                                  DSS_AF_INET6,
                                  &temp_in6_addr,
                                  sizeof(struct ps_in6_addr),
                                  &dss_errno );
          if( DSS_SUCCESS == retval )
          {
             is_numeric_ipv6 = TRUE;
          }
       }
    }

    /*-----------------------------------------------------------------------
      If address family is DSS_AF_UNSPEC then we have to send two queries:
      One for A records and other for AAAA records
    -----------------------------------------------------------------------*/
    if( DSS_AF_UNSPEC == addr_family )
    {
       if( is_numeric_ipv4 )
       {
          query_struct_ptr->num_queries = 1;
          query_struct_ptr->query_class[0] = PS_DNSI_QUERY_CLASS_IN;
          query_struct_ptr->query_type[0] = PS_DNSI_QUERY_TYPE_A;
       }
       else 
       {
          if( is_numeric_ipv6 )
          {
             query_struct_ptr->num_queries = 1;
             query_struct_ptr->query_class[0] = PS_DNSI_QUERY_CLASS_IN;
             query_struct_ptr->query_type[0] = PS_DNSI_QUERY_TYPE_AAAA;
          }
          else
          {
             // the query is not numerical
             query_struct_ptr->num_queries = 2;
             query_struct_ptr->query_class[0] = PS_DNSI_QUERY_CLASS_IN;
             query_struct_ptr->query_type[0] = PS_DNSI_QUERY_TYPE_A;
             query_struct_ptr->query_class[1] = PS_DNSI_QUERY_CLASS_IN;
             query_struct_ptr->query_type[1] = PS_DNSI_QUERY_TYPE_AAAA;
          }
       }
    }
    else
    {
      query_struct_ptr->num_queries = 1;
      query_struct_ptr->query_class[0] = PS_DNSI_QUERY_CLASS_IN;
      query_struct_ptr->query_type[0] = ( DSS_AF_INET == addr_family ) ?
                                          PS_DNSI_QUERY_TYPE_A     :
                                          PS_DNSI_QUERY_TYPE_AAAA;
    }

    /*-----------------------------------------------------------------------
      For addrinfo, if query is NULL then depending upon the PASSIVE flag
      we should be able to connect to localhost or bind to PS_INADDR_ANY or
      IN6ADDR_ANY (Both of which are represented by 0)
    -----------------------------------------------------------------------*/
    if( NULL == query_data_ptr )
    {
      if( (uint32)DSS_DNS_AI_FLAGS_PASSIVE & flags )
      {
        *(query_struct_ptr->query_data) = 0;
      }
      else
      {
        (void) strlcpy( (char *) query_struct_ptr->query_data,
                            (char *) "localhost.",
                            DSS_DNS_MAX_DOMAIN_NAME_LEN );
      }
    }
    else
    {
      (void) strlcpy( (char *) query_struct_ptr->query_data,
                          query_data_ptr,
                          DSS_DNS_MAX_DOMAIN_NAME_LEN );

      ps_dnsi_add_trailing_dot_to_hostname(query_struct_ptr->query_data);
    } /* if-else NULL == query_data_ptr */
  } /* if api type is addrinfo */
  else
  {
    /*-----------------------------------------------------------------------
      For get_nameinfo we send a pointer query.
    -----------------------------------------------------------------------*/
    query_struct_ptr->num_queries = 1;
    query_struct_ptr->query_class[0] = PS_DNSI_QUERY_CLASS_IN;
    query_struct_ptr->query_type[0] = PS_DNSI_QUERY_TYPE_PTR;

    if( NULL != query_data_ptr )
    {
      if( DSS_AF_INET == addr_family )
      {
        memscpy(
          &temp_in_addr,
          sizeof(struct ps_in_addr),
          ((struct ps_sockaddr*)query_data_ptr)->ps_sa_data,
          sizeof(struct ps_in_addr) );
        temp_in_addr.ps_s_addr   = ((temp_in_addr.ps_s_addr & 0xff)    << 24) |
                                ((temp_in_addr.ps_s_addr & 0xff00)     << 8)  |
                                ((temp_in_addr.ps_s_addr & 0xff0000)   >> 8)  |
                                ((temp_in_addr.ps_s_addr & 0xff000000U)>> 24);

        if( DSS_ERROR == dss_inet_ntoa( temp_in_addr,
                                        (uint8 *) query_struct_ptr->query_data,
                                        DSS_DNS_MAX_DOMAIN_NAME_LEN ) )
        {
          DATA_ERR_FATAL("Invalid Pointer address specified.");
          return;
        }
        /*-------------------------------------------------------------------
          Add a trailing dot '.' and "in-addr.arpa."
        -------------------------------------------------------------------*/
        (void) strlcat( query_struct_ptr->query_data,
                            ".",
                            PS_IN_ADDRSTRLEN + sizeof(DNS_INADDR_ARPA_STR) + 1 );

        (void) strlcat( query_struct_ptr->query_data,
                            DNS_INADDR_ARPA_STR,
                            PS_IN_ADDRSTRLEN + sizeof(DNS_INADDR_ARPA_STR) + 1 );
      }
      else /* if address family is DSS_AF_INET6 */
      {
        memscpy(&temp_sockaddr_in6, sizeof(struct ps_sockaddr_in6),
          query_data_ptr, sizeof(struct ps_sockaddr_in6));
        memscpy( &temp_in6_addr, sizeof(struct ps_in6_addr),
          &(temp_sockaddr_in6.ps_sin6_addr), sizeof(struct ps_in6_addr));
        msg_index = 0;
        for( index = 15; index >= 0; index-- )
        {
          temp_nibble = temp_in6_addr.ps_s6_addr[index] & 0x0F;
          query_struct_ptr->query_data[msg_index] =
            hex_to_char[ temp_nibble ];
          msg_index++;
          query_struct_ptr->query_data[msg_index] = '.';
          msg_index++;

          temp_nibble = (temp_in6_addr.ps_s6_addr[index] & 0xF0) >> 4;
          query_struct_ptr->query_data[msg_index] =
            hex_to_char[ temp_nibble ];
          msg_index++;
          query_struct_ptr->query_data[msg_index] = '.';
          msg_index++;
        }
        (void) strlcpy( (char *) &(query_struct_ptr->query_data[msg_index]),
                                   DNS_IN6ADDR_ARPA_STR,
                                   sizeof(DNS_IN6ADDR_ARPA_STR) );
        msg_index+=sizeof(DNS_IN6ADDR_ARPA_STR);
        query_struct_ptr->query_data[msg_index] = '\0';
      } /* if-else  */
    }/* If NULL != Query data */
    else
    {
      *( query_struct_ptr->query_data) = 0;
    }/* If NULL == Query data */

  } /* if api type is nameinfo */

} /* dss_dnsi_construct_queries() */

/*===========================================================================
FUNCTION DSS_DNSI_CONSTRUCT_AI_RESULTS()

DESCRIPTION
  Constructs dss_dns_addrinfo structures from the generic RR Q.

PARAMETERS
  rr_q          - Generic RR Q.
  ai_family     - Address family for the query asked.
  ai_socktype   - Socket type for the socket addresses.
  ai_proto      - Transport protocol
  ai_flags      - Addrinfo options
  dst_buffer    - Buffer where the results need to be stored.
  num_records   - Number of records (max) to be returned.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dss_dnsi_construct_ai_results
(
  q_type                    * rr_q,
  uint16                      ai_family,
  uint16                      ai_socktype,
  uint16                      ai_proto,
  dss_dns_ai_flags_enum_type  ai_flags,
  uint16                      ai_port,
  struct dss_dns_addrinfo   * dst_buffer,
  uint16                      num_records
)
{
  uint16                      index;
  ps_dnsi_generic_rr_type   * rr_node_ptr;
  struct ps_sockaddr_in       temp_in_sockaddr;
  struct ps_sockaddr_in6      temp_in6_sockaddr;
  struct ps_in_addr           temp_in_addr;
  struct ps_in6_addr          temp_in6_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == dst_buffer || 0 == num_records )
  {
    LOG_MSG_ERROR_2("Invalid destination buffer 0x%p or num records %d",
                    dst_buffer, num_records);
    ASSERT(0);
    return;
  }

  if( NULL == rr_q )
  {
    DATA_ERR_FATAL("NULL RR q");
    return;
  }

  if( (DSS_AF_INET != ai_family && DSS_AF_INET6 != ai_family &&                                        DSS_AF_UNSPEC != ai_family ) ||
      ((uint16)DSS_SOCK_DGRAM != ai_socktype && (uint16)DSS_SOCK_STREAM != ai_socktype) ||
      ((uint16)PS_IPPROTO_TCP != ai_proto && (uint16)PS_IPPROTO_UDP != ai_proto && 
                                                                  0 != ai_proto ) )
  {
    LOG_MSG_ERROR_3("Inv. arguments family %d, socktype %d, proto %d",
                    ai_family, ai_socktype, ai_proto);
    ASSERT(0);
    return;
  }

  /*lint -save -e655 Reason for suppressing error 655*/
  if( 0 != ((int)ai_flags & ~((int)DSS_DNS_AI_FLAGS_PASSIVE     |
                              (int)DSS_DNS_AI_FLAGS_CANONICAL   |
                              (int)DSS_DNS_AI_FLAGS_NUMERICHOST |
                              (int)DSS_DNS_AI_FLAGS_V4MAPPED    |
                              (int)DSS_DNS_AI_FLAGS_ALL         |
                              (int)DSS_DNS_AI_FLAGS_ADDRCONFIG  |
                              (int)DSS_DNS_AI_FLAGS_LOCAL_QUERY)) )
  /*lint -restore Restore lint error 655*/
  {
    LOG_MSG_ERROR_1("Invalid flag parameter 0x%x", ai_flags);
    ASSERT(0);
    return;
  }

  rr_node_ptr = q_check(rr_q);
  for( index = 0; index < num_records && NULL != rr_node_ptr; index++ )
  {
    memset(&temp_in_sockaddr, 0, sizeof(struct ps_sockaddr_in));
    memset(&temp_in6_sockaddr, 0, sizeof(struct ps_sockaddr_in6));
    memset( dst_buffer + index, 0, sizeof(struct dss_dns_addrinfo) );

    switch( rr_node_ptr->rr_type )
    {
      case PS_DNSI_QUERY_TYPE_A:
        ASSERT( sizeof(uint32) == rr_node_ptr->rdata_len );

        memscpy( &temp_in_addr, sizeof(struct ps_in_addr),
          rr_node_ptr->rdata, sizeof(struct ps_in_addr) );
        dst_buffer[index].ai_family = DSS_AF_INET;
        temp_in_sockaddr.ps_sin_family = DSS_AF_INET;
        temp_in_sockaddr.ps_sin_port   = ps_htons(ai_port);
        temp_in_sockaddr.ps_sin_addr   = temp_in_addr;

        memscpy( &(dst_buffer[index].ai_sockaddr),
          sizeof(struct ps_sockaddr_in), &temp_in_sockaddr,
          sizeof(struct ps_sockaddr_in) );
        break;

      case PS_DNSI_QUERY_TYPE_AAAA:
        ASSERT( 2 * sizeof(uint64) == rr_node_ptr->rdata_len );

        memscpy( &temp_in6_addr, sizeof(struct ps_in6_addr),
          rr_node_ptr->rdata, sizeof(struct ps_in6_addr) );
        dst_buffer[index].ai_family = DSS_AF_INET6;
        temp_in6_sockaddr.ps_sin6_family = DSS_AF_INET6;
        temp_in6_sockaddr.ps_sin6_port   = ps_htons(ai_port);
        temp_in6_sockaddr.ps_sin6_addr   = temp_in6_addr;

        memscpy( &(dst_buffer[index].ai_sockaddr),
          sizeof(struct ps_sockaddr_in6),
          &temp_in6_sockaddr,
          sizeof(struct ps_sockaddr_in6) );
        break;

      case PS_DNSI_QUERY_TYPE_CNAME:
        dst_buffer[index].ai_family = ai_family;
        (void) strlcpy( dst_buffer[index].ai_canonname,
                            (char *) rr_node_ptr->rdata,
                            DSS_DNS_MAX_DOMAIN_NAME_LEN );
        break;

      default:
        LOG_MSG_ERROR_1("Unkonwn RR type %d", rr_node_ptr->rr_type);
        break;
    } /* switch */

    if( 0 == *(dst_buffer[index].ai_canonname) )
    {
      (void) strlcpy( dst_buffer[index].ai_canonname,
                          (char *) rr_node_ptr->domain_name,
                          DSS_DNS_MAX_DOMAIN_NAME_LEN );
    }

    dst_buffer[index].ai_socktype = ai_socktype;
    dst_buffer[index].ai_protocol = ai_proto;
    dst_buffer[index].ttl = rr_node_ptr->ttl;

    rr_node_ptr = q_next( rr_q, &(rr_node_ptr->link) );
  } /* for */

  return;
} /* dss_dnsi_construct_ai_results() */

/*===========================================================================
FUNCTION DSS_DNSI_CONSTRUCT_NI_RESULTS()

DESCRIPTION
  Constructs dss_dns_nameinfo result struct from RR Q.

PARAMETERS
  rr_q    - Generic RR Q.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dss_dnsi_construct_ni_results
(
  q_type                   * rr_q,
  struct dss_dns_nameinfo  * dst_buffer,
  uint16                     num_records
)
{
  uint16                      index;
  ps_dnsi_generic_rr_type   * rr_node_ptr;
  int                         len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == dst_buffer || 0 == num_records )
  {
    LOG_MSG_ERROR_2("Invalid destination buffer 0x%p or num records %d",
                    dst_buffer, num_records);
    ASSERT(0);
    return;
  }

  if( NULL == rr_q )
  {
    DATA_ERR_FATAL("NULL RR q");
    return;
  }

  rr_node_ptr = q_check(rr_q);
  for( index = 0; index < num_records && NULL != rr_node_ptr; index++ )
  {
    switch( rr_node_ptr->rr_type )
    {
      case PS_DNSI_QUERY_TYPE_PTR:
        len = MIN( rr_node_ptr->rdata_len,
                   (int16)sizeof(dst_buffer[index].hostname) );
        (void) strlcpy( dst_buffer[index].hostname,
                            (char *) rr_node_ptr->rdata,
                            len );
        dst_buffer[index].ttl = rr_node_ptr->ttl;
        break;

      default:
        LOG_MSG_ERROR_1("Unkonwn RR type %d", rr_node_ptr->rr_type);
        break;
    } /* switch */

    rr_node_ptr = q_next( rr_q, &(rr_node_ptr->link) );
  } /* for */

} /* dss_dnsi_construct_ni_results() */

/*===========================================================================
FUNCTION PS_DNSI_API_CREATE_INSTANCE()

DESCRIPTION
  This function creates and populates control block for the API layer.

PARAMETERS
  api_type            - Type of query for which the API CB is being created.
  session_handle      - Session identifier.
  query_data_ptr      - Pointer to query data
  addr_family         - Address family.
  additional_info_ptr - API specific additional info
                        ADDRINFO_API: hints structure
                        NAMEINFO_API: flags parameter
  additional_info_len - Length of the additional info

RETURN VALUE
  On success, pointer to the newly created API control block
  On error, returns NULL and sets the dss_errno to the error occurred.

  dss_errno values
  ------------
  DS_ENOMEM - No memory to create the control block.
  DS_EFAULT - Invalid arguments.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static ps_dnsi_api_cb_type  * ps_dnsi_api_create_instance
(
  dss_dns_api_type_enum_type          api_type,
  dss_dns_session_mgr_handle_type     session_handle,
  const char                        * query_data_ptr,
  uint16                              addr_family,
  const void                        * additional_info_ptr,
  uint16                              additional_info_len,
  int16                             * dss_errno
)
{
  ps_dnsi_api_cb_type               * api_cb_ptr;
  struct dss_dns_addrinfo           * hints_ptr = NULL;
  int16                               buf_index;
  uint32                              flags;
  struct ps_in_addr                   temp_in_addr;
  struct ps_in6_addr                  temp_in6_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("API create instance called for session %d", session_handle);

  if( NULL == dss_errno )
  {
    LOG_MSG_ERROR_0("dss_errno is NULL");
    return NULL;
  }

  if( FALSE == dss_dns_session_mgr_is_handle_valid( session_handle ) )
  {
    LOG_MSG_ERROR_1("Invalid session handle %d", (int)session_handle);
    *dss_errno = DS_EFAULT;
    return NULL;
  }

  if( DSS_AF_INET   != addr_family &&
      DSS_AF_INET6  != addr_family &&
      DSS_AF_UNSPEC != addr_family )
  {
    LOG_MSG_ERROR_1("Invalid addr family %d", addr_family);
    *dss_errno = DS_EFAULT;
    return NULL;
  }

  if( DSS_AF_INET == addr_family )
  // Check if this is an IPv6 numerical address
  {
     if( DSS_SUCCESS == dss_inet_pton( query_data_ptr,
                                       DSS_AF_INET6,
                                       &temp_in6_addr,
                                       sizeof(struct ps_in6_addr),
                                       dss_errno ) )
     {
        LOG_MSG_ERROR_0("Invalid query: requesting INET family for IPv6 numerical address");
        *dss_errno = DS_EINVAL;
        return NULL;
     }
  }
  if( DSS_AF_INET6 == addr_family )
  // Check if this is an IPv4 numerical address
  {
     if( DSS_SUCCESS == dss_inet_pton( query_data_ptr,
                                       DSS_AF_INET,
                                       &temp_in_addr,
                                       sizeof(struct ps_in_addr),
                                       dss_errno ) )
     {
        LOG_MSG_ERROR_0("Invalid query: requesting INET6 family for IPv4 numerical address");
        *dss_errno = DS_EINVAL;
        return NULL;
     }
  }

  /*-------------------------------------------------------------------------
    Allocate a Control block for the API layer.
  -------------------------------------------------------------------------*/
  api_cb_ptr  = (ps_dnsi_api_cb_type *)
                ps_mem_get_buf( PS_MEM_DNSI_API_CB_TYPE );
  if( NULL == api_cb_ptr )
  {
    *dss_errno = DS_ENOMEM;
    return NULL;
  } 
  else
  {
    LOG_MSG_INFO2_1("DNS API CB allocated at %x", api_cb_ptr);
  }

  buf_index = ps_mem_buf_to_index( api_cb_ptr );
  if( -1 == buf_index )
  {
    LOG_MSG_ERROR_1("Invalid buffer 0x%p for API CB", api_cb_ptr);
    PS_MEM_FREE(api_cb_ptr );
    return NULL;
  }

  api_cb_ptr->handle = ps_handle_mgr_get_handle( API_CLIENT, buf_index );
  if( PS_HANDLE_MGR_INVALID_HANDLE == api_cb_ptr->handle )
  {
    LOG_MSG_ERROR_1("Invalid handle %d for API CB", api_cb_ptr->handle);
    PS_MEM_FREE(api_cb_ptr );
    return NULL;
  }

  api_cb_ptr->api_type       = api_type;
  api_cb_ptr->session_handle = session_handle;
  api_cb_ptr->state          = PS_DNSI_API_CB_STATE_OPEN;
  api_cb_ptr->query_status   = QUERY_IN_PROGRESS;
  memset(&api_cb_ptr->answer_q, 0, sizeof(api_cb_ptr->answer_q));
  (void) q_init( &api_cb_ptr->answer_q );

  /*-------------------------------------------------------------------------
    Process the additional info section.
  -------------------------------------------------------------------------*/
  switch( api_cb_ptr->api_type )
  {
    case DSS_DNS_API_ADDRINFO:
      api_cb_ptr->port_num = PS_DNSI_SERVICE_NOT_SPECIFIED;
      if( NULL == additional_info_ptr || 0 == additional_info_len )
      {
        api_cb_ptr->sock_type       = 0;
        api_cb_ptr->transport_proto = 0;
        api_cb_ptr->addr_family     = DSS_AF_UNSPEC;
        api_cb_ptr->flags           = 0;     
      }
      else
      {
        hints_ptr = (struct dss_dns_addrinfo *) additional_info_ptr;
        api_cb_ptr->sock_type       = hints_ptr->ai_socktype;
        api_cb_ptr->transport_proto = hints_ptr->ai_protocol;
        api_cb_ptr->addr_family     = hints_ptr->ai_family;
        api_cb_ptr->flags           = (int)hints_ptr->ai_flags;
      }
      break;

    case DSS_DNS_API_NAMEINFO:
      if( NULL == additional_info_ptr || 0 == additional_info_len )
      {
        api_cb_ptr->flags = 0;
      }
      else
      {
        flags = *((uint32 *) additional_info_ptr);
        api_cb_ptr->flags = flags;
      }
      api_cb_ptr->addr_family = addr_family;
      break;

    default:
      LOG_MSG_ERROR_1("Invalid query API type %d", api_type);
      *dss_errno = DS_EFAULT;
      PS_MEM_FREE(api_cb_ptr );
      return NULL;
  } /* switch */

  api_cb_ptr->response_received = FALSE;
  /*-------------------------------------------------------------------------
    Populate the queries section.
  -------------------------------------------------------------------------*/
  api_cb_ptr->query_index = 0;
  dss_dnsi_construct_queries( api_cb_ptr->api_type,
                              addr_family,
                              api_cb_ptr->flags,
                              query_data_ptr,
                              &(api_cb_ptr->queries) );

  return api_cb_ptr;

} /* ps_dnsi_api_create_instance() */

/*===========================================================================
FUNCTION PS_DNSI_API_IS_HANDLE_VALID()

DESCRIPTION
  Checks if the specified handle is valid.

PARAMETERS
  api_cb_handle  - API control block handle

RETURN VALUE
  Pointer to the control block if the specified handle is valid.
  NULL if the specified handle is not valid.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
ps_dnsi_api_cb_type * ps_dnsi_api_is_handle_valid
(
  dss_dns_query_handle_type  api_cb_handle
)
{
  ps_dnsi_api_cb_type       * api_cb_ptr;
  int16                       buf_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( DSS_DNS_QUERY_INVALID_HANDLE == api_cb_handle )
  {
    LOG_MSG_ERROR_1("Invalid API handle %d", api_cb_handle);
    return NULL;
  }

  buf_index = ps_handle_mgr_get_index( API_CLIENT,
                                       api_cb_handle );
  if( PS_HANDLE_MGR_INVALID_INDEX == buf_index )
  {
    LOG_MSG_ERROR_1("Invalid index for handle %d", buf_index);
    return NULL;
  }

  api_cb_ptr =
    (ps_dnsi_api_cb_type *)
       ps_mem_index_to_buf( buf_index, PS_MEM_DNSI_API_CB_TYPE );

  if( NULL == api_cb_ptr || api_cb_ptr->handle != api_cb_handle )
  {
    LOG_MSG_ERROR_2("Inv API CB 0x%p from handle %d",
                    api_cb_ptr, api_cb_handle);
    return NULL;
  }

  return api_cb_ptr;
} /* ps_dnsi_api_is_handle_valid() */

/*===========================================================================
FUNCTION PS_DNSI_API_DELETE_INSTANCE()

DESCRIPTION
  Deletes the API control block instance specified by the given handle.

PARAMETERS
  api_cb_handle - Handle to the API control block to delete.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_dnsi_api_delete_instance
(
  dss_dns_query_handle_type   api_cb_handle
)
{
  ps_dnsi_api_cb_type          * api_cb_ptr = NULL;
  ps_dnsi_generic_rr_type       * rr_node_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  api_cb_ptr = ps_dnsi_api_is_handle_valid( api_cb_handle );
  LOG_MSG_INFO2_2("API delete instance called for handle %d at %x",
                  api_cb_handle, api_cb_ptr);

  if( NULL == api_cb_ptr )
  {
    LOG_MSG_ERROR_0("Delete instance: API CB is NULL");
    return;
  }

  while ( NULL != (rr_node_ptr = q_get( &api_cb_ptr->answer_q ) ) )
  {
    ps_dnsi_mem_free( (void **) &(rr_node_ptr->rdata) );
    PS_MEM_FREE(rr_node_ptr );
  }
  q_destroy(&api_cb_ptr->answer_q);

  memset(api_cb_ptr, 0, sizeof(ps_dnsi_api_cb_type));
  api_cb_ptr->handle = DSS_DNS_QUERY_INVALID_HANDLE;
  PS_MEM_FREE(api_cb_ptr );
} /* ps_dnsi_api_delete_instance() */

/*===========================================================================
FUNCTION  PS_DNSI_API_RESOLVER_EVENT_NOTIFY()

DESCRIPTION
  Callback function registered with the resolver.

PARAMETERS

  event         - Type of resolver event.
                  PS_DNSI_RES_NOTIFY_SUCCESS or PS_DNSI_RES_NOTIFY_FAILURE
  res_handle    - Handle to the resolver control block.
  user_data_ptr - Points to the API control block.
  dss_errno         - Error code in both events.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_dnsi_api_resolver_event_notify
(
  ps_dnsi_resolver_notify_event_enum_type   event,
  ps_dnsi_resolver_handle_type              res_handle,
  q_type                                  * answer_q,
  void                                    * user_data_ptr,
  int16                                     dss_errno
)
{
  ps_dnsi_api_cb_type               * api_cb_ptr = NULL;
  dss_dns_cback_f_type                app_cback_f_ptr = NULL;
  void                              * app_user_data_ptr = NULL;
  boolean                             notify_flag = FALSE;
  boolean                             notify_error = FALSE;
  dss_dns_session_mgr_handle_type     session_handle;
  int16                               num_records = 0;
  int16                               retval = DSS_ERROR;
  ps_dnsi_generic_rr_type           * rr_node_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3("Resolver %d event occurred for API 0x%p with answer Q 0x%p",
                  res_handle, user_data_ptr, answer_q );

  /*-------------------------------------------------------------------------
    Sanity check.
  -------------------------------------------------------------------------*/
  if( NULL == user_data_ptr ||
    ( PS_DNSI_RES_NOTIFY_SUCCESS != event &&
      PS_DNSI_RES_NOTIFY_FAILURE != event ) )
  {
    LOG_MSG_ERROR_2("Invalid arguments event %d, user data 0x%p",
                    (int) event, user_data_ptr );
    ASSERT(0);
    return;
  }

  if( !ps_mem_is_valid(user_data_ptr, PS_MEM_DNSI_API_CB_TYPE) )
  {
    LOG_MSG_ERROR_2("Event %d recvd on freed API cb 0x%p", event, api_cb_ptr );
    return;
  }

  api_cb_ptr = (ps_dnsi_api_cb_type *) user_data_ptr;
  if( api_cb_ptr != ps_dnsi_api_is_handle_valid( api_cb_ptr->handle ) )
  {
    LOG_MSG_ERROR_1("Invalid api cb 0x%p", api_cb_ptr );
    ASSERT(0);
    return;
  }

  session_handle = api_cb_ptr->session_handle;
  if( !dss_dns_session_mgr_is_handle_valid( session_handle ) )
  {
    LOG_MSG_ERROR_2("Invalid session handle %d in API CB 0x%p",
                    session_handle, api_cb_ptr);
    return;
  }

  if (api_cb_ptr->res_handle != res_handle)
  {
    LOG_MSG_ERROR_3("API CB pointer 0x%p resolver handle 0x%p doesn't match returned 0x%p resolver handle",
                    api_cb_ptr, api_cb_ptr->res_handle, res_handle);
    return;
  }

  switch( event )
  {
    case PS_DNSI_RES_NOTIFY_SUCCESS:
      api_cb_ptr->response_received = TRUE;
      /*---------------------------------------------------------------------
        Read the answer q passed in.
        If the answer Q count in API control block is 0, just change the
        answer_q pointer to point to the answer q of the resolver.
        Else we add the resolver answer q at the end of API CB's answer q.
      ---------------------------------------------------------------------*/
      while( NULL != (rr_node_ptr = q_get(answer_q) ))
      {
        q_put(&api_cb_ptr->answer_q, &(rr_node_ptr->link));
      }

      ps_dnsi_resolver_delete_instance( res_handle );
      /*----------------------------------------------------------------------
        Handle is invalid right now
      ----------------------------------------------------------------------*/
      api_cb_ptr->res_handle = PS_DNSI_RESOLVER_INVALID_HANDLE;
      LOG_MSG_ERROR_1("Resolver handle has been set to invalid for API CB 0x%p",
                      api_cb_ptr);
      
      (api_cb_ptr->query_index)++;

      if( api_cb_ptr->query_index < api_cb_ptr->queries.num_queries )
      {
        /*-------------------------------------------------------------------------
          Sanity check that num_queries doesn't 
          exceed PS_DNSI_MAX_SPLIT_QUERIES (detected by KW).
          ---------------------------------------------------------------------*/
        if ( api_cb_ptr->query_index >= PS_DNSI_MAX_SPLIT_QUERIES )
        {
          LOG_MSG_ERROR_2("query_index (%d) exceeds PS_DNSI_MAX_SPLIT_QUERIES (%d)",
                          api_cb_ptr->query_index, PS_DNSI_MAX_SPLIT_QUERIES);
          ASSERT(0);
          dss_errno = DS_ESYSTEM;
          notify_error = TRUE;
          break;
        }

        /*-------------------------------------------------------------------------
          Get a resolver instance for this query.
        -------------------------------------------------------------------------*/
        api_cb_ptr->res_handle = ps_dnsi_resolver_get_instance( session_handle,
                                                                &dss_errno );
        if( PS_DNSI_RESOLVER_INVALID_HANDLE == api_cb_ptr->res_handle )
        {
          LOG_MSG_ERROR_0("Can't get a resolver for this query; calling the application's callback");
          // First query succeeded so return SUCCESS to the client
          num_records = q_cnt( &api_cb_ptr->answer_q );
          dss_errno = DSS_SUCCESS;
          notify_flag = TRUE;
          break;
        }

        retval = ps_dnsi_resolver_start_query(
          api_cb_ptr->res_handle,
          api_cb_ptr->queries.query_type[api_cb_ptr->query_index],
          api_cb_ptr->queries.query_class[api_cb_ptr->query_index],
          api_cb_ptr->queries.query_data,
          ps_dnsi_api_resolver_event_notify,
          (void *)api_cb_ptr,
          &dss_errno );

        if( DSS_ERROR != retval || DS_EWOULDBLOCK != dss_errno )
        {
          LOG_MSG_ERROR_1("Cant start resolver query: error %d", dss_errno);
          // First query succeeded so return SUCCESS to the client
          num_records = q_cnt( &api_cb_ptr->answer_q );
          dss_errno = DSS_SUCCESS;
          notify_flag = TRUE;
          break;
        }
      }
      else
      {
        num_records = q_cnt( &api_cb_ptr->answer_q );
        notify_flag = TRUE;
      }
      break;

    case PS_DNSI_RES_NOTIFY_FAILURE:
      LOG_MSG_ERROR_1("Resolver failure %d", dss_errno);

      ps_dnsi_resolver_delete_instance( res_handle );
      /*----------------------------------------------------------------------
        Handle is invalid right now
      ----------------------------------------------------------------------*/
      api_cb_ptr->res_handle = PS_DNSI_RESOLVER_INVALID_HANDLE;
      LOG_MSG_ERROR_1("Resolver handle has been set to invalid for API CB 0x%p",
                      api_cb_ptr);

      (api_cb_ptr->query_index)++;

      if( api_cb_ptr->query_index < api_cb_ptr->queries.num_queries )
      {
        /*-------------------------------------------------------------------------
          Sanity check that num_queries doesn't 
          exceed PS_DNSI_MAX_SPLIT_QUERIES (detected by KW).
          ---------------------------------------------------------------------*/
        if ( api_cb_ptr->query_index >= PS_DNSI_MAX_SPLIT_QUERIES )
        {
          LOG_MSG_ERROR_2("query_index (%d) exceeds PS_DNSI_MAX_SPLIT_QUERIES (%d)",
                          api_cb_ptr->query_index, PS_DNSI_MAX_SPLIT_QUERIES);
          ASSERT(0);
          dss_errno = DS_ESYSTEM;
          notify_error = TRUE;
          break;
        }

        /*-------------------------------------------------------------------------
          Get a resolver instance for this query.
        -------------------------------------------------------------------------*/
        api_cb_ptr->res_handle = ps_dnsi_resolver_get_instance( session_handle,
                                                                &dss_errno );
        if( PS_DNSI_RESOLVER_INVALID_HANDLE == api_cb_ptr->res_handle )
        {
          LOG_MSG_ERROR_0("Can't get a resolver for this query; calling the application's callback");
          num_records = q_cnt( &api_cb_ptr->answer_q );
          notify_error = TRUE;
          break;
        }

        retval = ps_dnsi_resolver_start_query(
          api_cb_ptr->res_handle,
          api_cb_ptr->queries.query_type[api_cb_ptr->query_index],
          api_cb_ptr->queries.query_class[api_cb_ptr->query_index],
          api_cb_ptr->queries.query_data,
          ps_dnsi_api_resolver_event_notify,
          (void *)api_cb_ptr,
          &dss_errno );

        if( DSS_ERROR != retval || DS_EWOULDBLOCK != dss_errno )
        {
          LOG_MSG_ERROR_1("Cant start resolver query: error %d", dss_errno);
          notify_error = TRUE;
        }
      }
      else
      {
        if (api_cb_ptr->response_received)
        {
          num_records = q_cnt( &api_cb_ptr->answer_q );
          dss_errno = DSS_SUCCESS;
          notify_flag = TRUE;
        }
        else
        {
          notify_error = TRUE;
        }
      }
      break;

    default:
      LOG_MSG_ERROR_2("Invalid API CB 0x%p event %d", api_cb_ptr, event);
      ASSERT(0);
      notify_error = TRUE;
      break;
  } /* switch */

  /*-----------------------------------------------------------------------
    Get the callback information from the session manager to notify
    the application.
  -----------------------------------------------------------------------*/
  retval = dss_dns_session_mgr_get_cback_info( session_handle,
                                               &app_cback_f_ptr,
                                               &app_user_data_ptr );
  if( DSS_ERROR == retval )
  {
    LOG_MSG_ERROR_1("Cannot get cback info for session %d", session_handle);
    return;
  }

  /*-------------------------------------------------------------------------
    Inform the application of success or failure event.
  -------------------------------------------------------------------------*/
  if( notify_flag || notify_error )
  {
    api_cb_ptr->state = PS_DNSI_API_CB_STATE_FREE;
    
    app_cback_f_ptr( session_handle,
                     api_cb_ptr->handle,
                     api_cb_ptr->api_type,
                     num_records,
                     app_user_data_ptr,
                     dss_errno );
    if( notify_error || ( notify_flag && ( 0 == num_records ) ) )
    {
      api_cb_ptr->query_status = QUERY_FAILED;
    }
  }

  return;
} /* ps_dnsi_api_resolver_event_notify() */

/*===========================================================================

                       EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION  DSS_DNS_INIT()

DESCRIPTION
  Initialize the DNS subsystem during powerup.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  PS mem and Q subsystems must be initialized.

SIDE EFFECTS
  None.
===========================================================================*/
void dss_dns_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize Pool
  -------------------------------------------------------------------------*/
  if(PS_MEM_POOL_INIT_OPT(PS_MEM_DNSI_API_CB_TYPE,
                          ps_dnsi_api_cb_buf_mem,
                          PS_DNSI_API_CB_BUF_SIZE,
                          PS_DNSI_API_CB_BUF_NUM,
                          PS_DNSI_API_CB_BUF_HIGH_WM,
                          PS_DNSI_API_CB_BUF_LOW_WM,
                          NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                          (int *) ps_dnsi_api_cb_buf_hdr,
                          (int *) ps_dnsi_api_cb_buf_ptr
#else
                          NULL,
                          NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                         ) == -1)
  {
    LOG_MSG_ERROR_0("Can't init the module");
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DNSI_SESSION_MGR_CB_TYPE,
                           ps_dnsi_session_mgr_buf_mem,
                           PS_DNSI_SESSION_MGR_CB_BUF_SIZE,
                           PS_DNSI_SESSION_MGR_CB_BUF_NUM,
                           PS_DNSI_SESSION_MGR_CB_BUF_HIGH_WM,
                           PS_DNSI_SESSION_MGR_CB_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_dnsi_session_mgr_cb_buf_hdr,
                           (int *) ps_dnsi_session_mgr_cb_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("Can't init the module");
  }

  if(PS_MEM_POOL_INIT_OPT(PS_MEM_DNSI_SESSION_DNS_SERVERS_TYPE,
                       ps_dnsi_session_dns_servers_buf_mem,
                       PS_DNSI_SESSION_DNS_SERVERS_BUF_SIZE,
                       PS_DNSI_SESSION_DNS_SERVERS_BUF_NUM,
                       PS_DNSI_SESSION_DNS_SERVERS_BUF_HIGH_WM,
                       PS_DNSI_SESSION_DNS_SERVERS_BUF_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) ps_dnsi_session_dns_servers_buf_hdr,
                       (int *) ps_dnsi_session_dns_servers_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    LOG_MSG_ERROR_0("Can't init the module");
  }

  if(PS_MEM_POOL_INIT_OPT(PS_MEM_DNSI_HOSTENT_CB_TYPE,
                          ps_dnsi_hostent_buf_mem,
                          PS_DNSI_HOSTENT_CB_BUF_SIZE,
                          PS_DNSI_HOSTENT_CB_BUF_NUM,
                          PS_DNSI_HOSTENT_CB_BUF_HIGH_WM,
                          PS_DNSI_HOSTENT_CB_BUF_LOW_WM,
                          NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                          (int *) ps_dnsi_hostent_cb_buf_hdr,
                          (int *) ps_dnsi_hostent_cb_buf_ptr
#else
                          NULL,
                          NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                         ) == -1)
  {
    LOG_MSG_ERROR_0("Can't init the module");
  }

  /*-------------------------------------------------------------------------
    Initialize handle manager clients for the DNS API control block,
    session manager control block, resolver control block, and IO manager
    control block.
  -------------------------------------------------------------------------*/
  ps_handle_mgr_init_client( PS_HANDLE_MGR_CLIENT_DNS_API,
                             DSS_DNS_MAX_QUERIES,
                             0,
                             0 );

  ps_handle_mgr_init_client( PS_HANDLE_MGR_CLIENT_DNS_SESSION_MGR,
                             DSS_DNS_MAX_SESSIONS,
                             0,
                             0 );

  ps_handle_mgr_init_client( PS_HANDLE_MGR_CLIENT_DNS_RESOLVER,
                             DSS_DNS_MAX_QUERIES,
                             0,
                             0 );

  ps_handle_mgr_init_client( PS_HANDLE_MGR_CLIENT_DNS_IO_MGR,
                             DSS_DNS_MAX_QUERIES,
                             0,
                             0 );

  /*------------------------------------------------------------------------
    Initialize the Resolver. This takes care of initialzing the IO manager
    the network manager and the cache manager.
  -------------------------------------------------------------------------*/
  dss_dns_session_mgr_init();
  ps_dnsi_resolver_init();

  return;
}/* dss_dns_init() */

/*===========================================================================
FUNCTION  DSS_DNS_GET_ADDRINFO()

DESCRIPTION
  This function starts the resolver to get a list of dss_dns_addrinfo
  structures. This is a blocking function. After the query operations are
  complete, the application would be notified using the callback registered
  with the session manager. The application must call dss_dns_read_addrinfo()
  funtion to read the results.

  Currently service names are not supported. Hence servname argument must
  be NULL or a numeric(decimal) port string.

PARAMETERS
  session_handle - Handle to a valid session control block.
                   (obtained by calling dss_dns_create_session)

  hostname       - Hostname argument
                   Can be a NULL pointer or a IPv4 dotted decimal string
                   or IPv6 colon seperated string or a valid domain name.

  servname       - Currently service names are not supported. Hence this
                   argument must be NULL or a decimal port string.

  hints          - A dss_dns_addrinfo struct pointer which directs the
                   operation of the resolver by providing input values
                   or by limiting the return information.

                   A NULL hints parameter is treated as one with
                   ai_family=DSS_AF_UNSPEC, ai_socktype=0, ai_protocol=0,
                   and ai_flags=0

                   Only the following fields of hints are processed.

                   ai_family
                     DSS_AF_UNSPEC/0 : Caller shall accept any address family
                     DSS_AF_INET     : Caller shall accept only DSS_AF_INET
                     DSS_AF_INET6    : Caller shall accept only DSS_AF_INET6

                   ai_socktype
                     0           : Caller shall accept any socktype
                     DSS_SOCK_STREAM : Caller shall accept DSS_SOCK_STREAM only
                     DSS_SOCK_DGRAM  : Caller shall accept DSS_SOCK_DGRAM only

                   ai_protocol
                     0           : Caller shall accept any protocol
                     PS_IPPROTO_TCP : Caller shall accpet PS_IPPROTO_TCP only
                     PS_IPPROTO_UDP : Caller shall accpet PS_IPPROTO_UDP only

                   ai_flags

                     AI_PASSIVE
                       If hostname argument is NULL, PS_INADDR_ANY or
                       IN6ADDR_ANY will be returned depending upon
                       supported family for the iface and ai_family
                       parameter of the hints message. Ignored if hostname
                       is non-NULL.

                     AI_CANONICAL
                       If the hostname is non-NULL attempts to find the
                       canonical name. Ignored if hostname is NULL.

                     AI_NUMERICHOST
                       If the hostname is non-NULL and contains a valid
                       V4 or V6 numeric address, that will be returned.
                       Otherwise dss_errno will be set to DS_NAMEERR.

                     AI_NUMERICSERV
                       This flag is currently ignored.

                     AI_V4MAPPED
                       If address family is DSS_AF_INET6/DSS_AF_UNSPEC and no
                       IPv6 adresses are found, all IPv4 addresses found
                       will be returned as IPv4-mapped-IPv6 address.
                       Ignored if address family is DSS_AF_INET.

                     AI_ALL
                       All addresses found will be returned IPv6 format.
                       Ignored if AI_V4MAPPED is not set.

                     AI_ADDRCONFIG
                       Addresses configured on local systems are returned.

                   A NULL hints parameter is treated as one with
                   ai_family=DSS_AF_UNSPEC, ai_socktype=0, ai_protocol=0,
                   and ai_flags=0

RETURN VALUE
  On success, returns a query handle identifying the query started.
  On error, returns DSS_DNS_QUERY_INVALID_HANDLE and sets the
  dss_errno to the error code.

  dss_errno values
  ----------------
  DS_EFAULT     - Invalid arguments
  DS_EBADF      - Invalid session handle
  DS_ENOMEM     - Out of memory
  DS_NAMERR     - Malformed query.
  DS_EWOULBLOCK - Operation would block

DEPENDENCIES
  A valid DNS session must be created prior to calling this function.

SIDE EFFECTS
  None.
===========================================================================*/
dss_dns_query_handle_type dss_dns_get_addrinfo
(
  dss_dns_session_mgr_handle_type     session_handle,
  const char                        * hostname_ptr,
  const char                        * service_ptr,
  const struct dss_dns_addrinfo     * hints_ptr,
  int16                             * dss_errno
)
{
  int16                               retval = DSS_ERROR;
  ps_dnsi_api_cb_type               * api_cb_ptr = NULL;
  uint16                              addr_family;
  ps_dnsi_session_mgr_cb_type       * session_mgr_ptr = NULL;
  uint16                              service;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("dss_dns_get_addrinfo(): "
    "caller 0x%p",
    caller_ptr);
  LOG_MSG_INFO2_3("dss_dns_get_addrinfo(): "
    "dss_dns_get_addrinfo called with sess %d, hostname 0x%p, hints 0x%x",
    session_handle, hostname_ptr, hints_ptr);

  if( NULL == dss_errno )
  {
    LOG_MSG_ERROR_0("dss_errno is NULL");
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  if( FALSE == dss_dns_session_mgr_is_handle_valid(session_handle) )
  {
    LOG_MSG_ERROR_1("Invalid session handle %d", (int)session_handle);
    *dss_errno = DS_EBADF;
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  if( FALSE == dss_dnsi_validate_hostname_query( hostname_ptr, hints_ptr ) )
  {
    LOG_MSG_ERROR_1("Invalid hostname argument 0x%p", hostname_ptr);
    *dss_errno = DS_NAMEERR;
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  if( FALSE == dss_dnsi_parse_service_string(service_ptr, &service) )
  {
    LOG_MSG_ERROR_1("Invalid service argument 0x%p", service_ptr);
    *dss_errno = DS_EFAULT;
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  if ((NULL == service_ptr) && (NULL == hostname_ptr))
  {
     LOG_MSG_ERROR_0("Either hostname or service must not be NULL");
     *dss_errno = DS_EFAULT;
     return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  if( NULL != hints_ptr )
  {
    if( DSS_AF_INET   != hints_ptr->ai_family &&
        DSS_AF_INET6  != hints_ptr->ai_family &&
        DSS_AF_UNSPEC != hints_ptr->ai_family )
    {
      *dss_errno = DS_EAFNOSUPPORT;
      LOG_MSG_ERROR_1("Invalid Addr family %d", hints_ptr->ai_family);
      return DSS_DNS_QUERY_INVALID_HANDLE;
    }
    addr_family = hints_ptr->ai_family;

    if( DSS_SOCK_DGRAM  != hints_ptr->ai_socktype &&
        DSS_SOCK_STREAM != hints_ptr->ai_socktype &&
        0           != hints_ptr->ai_socktype )
    {
      *dss_errno = DS_EOPNOTSUPP;
      LOG_MSG_ERROR_1("Invalid Socket type %d", hints_ptr->ai_socktype);
      return DSS_DNS_QUERY_INVALID_HANDLE;
    }

    if( (int)PS_IPPROTO_TCP != hints_ptr->ai_protocol &&
        (int)PS_IPPROTO_UDP != hints_ptr->ai_protocol &&
        0                   != hints_ptr->ai_protocol )
    {
      *dss_errno = DS_EOPNOTSUPP;
      LOG_MSG_ERROR_1("Invalid transport proto %d", hints_ptr->ai_protocol);
      return DSS_DNS_QUERY_INVALID_HANDLE;
    }

    /*lint -save -e655 Reason for suppressing error 655*/
    if( 0 != ((int)hints_ptr->ai_flags & ~( (int)DSS_DNS_AI_FLAGS_PASSIVE     |
                                            (int)DSS_DNS_AI_FLAGS_CANONICAL   |
                                            (int)DSS_DNS_AI_FLAGS_NUMERICHOST |
                                            (int)DSS_DNS_AI_FLAGS_V4MAPPED    |
                                            (int)DSS_DNS_AI_FLAGS_ALL         |
                                            (int)DSS_DNS_AI_FLAGS_ADDRCONFIG  |
                                            (int)DSS_DNS_AI_FLAGS_LOCAL_QUERY )) )
    /*lint -restore Restore lint error 655*/
    {
      *dss_errno = DS_EOPNOTSUPP;
      LOG_MSG_ERROR_1("Invalid Hints parameter 0x%x", hints_ptr->ai_flags);
      return DSS_DNS_QUERY_INVALID_HANDLE;
    }
  }
  else
  {
    addr_family = DSS_AF_UNSPEC;
  }

  /*-------------------------------------------------------------------------
    Create a API control block for the query
  -------------------------------------------------------------------------*/
  api_cb_ptr = ps_dnsi_api_create_instance( DSS_DNS_API_ADDRINFO,
                                            session_handle,
                                            hostname_ptr,
                                            addr_family,
                                            (void *) hints_ptr,
                                            sizeof(struct dss_dns_addrinfo),
                                            dss_errno );
  if( NULL == api_cb_ptr )
  {
    LOG_MSG_ERROR_1("Cant create API CB instance: error %d", *dss_errno);
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }
  api_cb_ptr->port_num = service;

  /*-------------------------------------------------------------------------
    Get a resolver instance for this query.
  -------------------------------------------------------------------------*/
  api_cb_ptr->res_handle = ps_dnsi_resolver_get_instance( session_handle,
                                                          dss_errno );
  if( PS_DNSI_RESOLVER_INVALID_HANDLE == api_cb_ptr->res_handle )
  {
    LOG_MSG_ERROR_0("Cant get resolver for query");
    *dss_errno = DS_ENOMEM;
    ps_dnsi_api_delete_instance( api_cb_ptr->handle );
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }
     
  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);
  if (NULL == session_mgr_ptr)
  {
    LOG_MSG_ERROR_0("Can't get session pointer");
    *dss_errno = DS_EINVAL;
    ps_dnsi_resolver_delete_instance(api_cb_ptr->res_handle);
    ps_dnsi_api_delete_instance(api_cb_ptr->handle);
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }
  // Clearing failed query
  if ((NULL != session_mgr_ptr->api_cb_ptr ) &&
                    (QUERY_FAILED == session_mgr_ptr->api_cb_ptr->query_status))
  {
    ps_dnsi_api_delete_instance( session_mgr_ptr->api_cb_ptr->handle );
    session_mgr_ptr->api_cb_ptr = NULL;
  }
  if (NULL != session_mgr_ptr->api_cb_ptr)
  {
    LOG_MSG_ERROR_0("dss_dns_get_addrinfo(): Session has already started a query");
    *dss_errno = DS_EOPNOTSUPP;
    ps_dnsi_resolver_delete_instance(api_cb_ptr->res_handle);
    ps_dnsi_api_delete_instance(api_cb_ptr->handle);
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }
  session_mgr_ptr->api_cb_ptr = api_cb_ptr;

  retval = ps_dnsi_resolver_start_query( api_cb_ptr->res_handle,
                                         api_cb_ptr->queries.query_type[0],
                                         api_cb_ptr->queries.query_class[0],
                                         api_cb_ptr->queries.query_data,
                                         ps_dnsi_api_resolver_event_notify,
                                         (void *)api_cb_ptr,
                                         dss_errno );

  if( DSS_ERROR != retval || DS_EWOULDBLOCK != *dss_errno )
  {
    LOG_MSG_ERROR_1("Cant start resolver query: error %d", *dss_errno);
    ps_dnsi_resolver_delete_instance( api_cb_ptr->res_handle );
    ps_dnsi_api_delete_instance( api_cb_ptr->handle );
    session_mgr_ptr->api_cb_ptr = NULL;
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  *dss_errno = DS_EWOULDBLOCK;
  return api_cb_ptr->handle;
} /* dss_dns_get_addrinfo() */

/*===========================================================================
FUNCTION  DSS_DNS_GET_NAMEINFO()

DESCRIPTION
  This function queries for dss_dns_nameinfo records. This function is used
  to convert a socket address to a pair of hostname and service name
  arguments. Currently service name argument is not supported.

PARAMETERS

  session_handle  - Session Identifier
  sa_ptr          - Socket address ptr
  sa_len          - Socket address length
  flags           - Flags
  dss_errno           - Error code

RETURN VALUE
  Returns a handle to the query started on success
  Returns DSS_DNS_QUERY_INVALID_HANDLE on error.

  dss_errno values
  ------------
  DS_EFAULT     - Invalid arguments
  DS_EBADF      - Invalid handle.
  DS_ENOMEM     - Out of memory
  DS_NAMERR     - Malformed query.
  DS_EWOULBLOCK - Operation would block

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
dss_dns_query_handle_type dss_dns_get_nameinfo
(
  dss_dns_session_mgr_handle_type     session_handle,
  const struct ps_sockaddr          * sa_ptr,
  uint16                              sa_len,
  uint32                              flags,
  int16                             * dss_errno
)
{
  ps_dnsi_api_cb_type           * api_cb_ptr = NULL;
  int16                           retval = DSS_ERROR;
  ps_dnsi_session_mgr_cb_type   * session_mgr_ptr = NULL;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_dns_get_nameinfo(): "
    "caller 0x%p",
    caller_ptr);

  LOG_MSG_INFO2_3("dss_dns_get_nameinfo(): "
    "dss_dns_get_nameinfo called with sess %d, sockaddr 0x%p, flags 0x%x",
    session_handle, sa_ptr, flags);

  if( NULL == dss_errno )
  {
    LOG_MSG_ERROR_0("dss_errno is NULL");
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  if( FALSE == dss_dns_session_mgr_is_handle_valid(session_handle) )
  {
    LOG_MSG_ERROR_1("Invalid session handle %d", (int)session_handle);
    *dss_errno = DS_EBADF;
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  if( 0 != flags )
  {
    LOG_MSG_ERROR_1("Invalid operation flags = 0x%x", flags);
    *dss_errno = DS_EOPNOTSUPP;
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  if( NULL == sa_ptr || 0 == sa_len )
  {
    LOG_MSG_ERROR_0("ps_sockaddr is NULL or length is 0");
    *dss_errno = DS_EFAULT;
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  if( !( (DSS_AF_INET == sa_ptr->ps_sa_family && 
          sizeof(struct ps_sockaddr_in) == sa_len) ||
         (DSS_AF_INET6 == sa_ptr->ps_sa_family && 
          sizeof(struct ps_sockaddr_in6) == sa_len) ) )
  {
    LOG_MSG_ERROR_2("Invalid ps_sockaddr 0x%p or length %d", sa_ptr, sa_len);
    *dss_errno = DS_EAFNOSUPPORT;
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  /*-------------------------------------------------------------------------
    Create a API control block for the query
  -------------------------------------------------------------------------*/
  api_cb_ptr = ps_dnsi_api_create_instance( DSS_DNS_API_NAMEINFO,
                                            session_handle,
                                            (char *) sa_ptr,
                                            sa_ptr->ps_sa_family,
                                            (void *) &flags,
                                            sizeof(uint32),
                                            dss_errno );
  if( NULL == api_cb_ptr )
  {
    LOG_MSG_ERROR_1("Cant create API CB instance: error %d", *dss_errno);
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  /*-------------------------------------------------------------------------
    Get a resolver instance for this query.
  -------------------------------------------------------------------------*/
  api_cb_ptr->res_handle = ps_dnsi_resolver_get_instance( session_handle,
                                                          dss_errno );
  if( PS_DNSI_RESOLVER_INVALID_HANDLE == api_cb_ptr->res_handle )
  {
    LOG_MSG_ERROR_0("Cant get resolver for query");
    *dss_errno = DS_ENOMEM;
    ps_dnsi_api_delete_instance( api_cb_ptr->handle );
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);
  if (NULL == session_mgr_ptr)
  {
    LOG_MSG_ERROR_0("Can't get session pointer");
    *dss_errno = DS_EINVAL;
    ps_dnsi_resolver_delete_instance(api_cb_ptr->res_handle);
    ps_dnsi_api_delete_instance(api_cb_ptr->handle);
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }
  // Clearing failed query
  if ((NULL != session_mgr_ptr->api_cb_ptr ) &&
                    (QUERY_FAILED == session_mgr_ptr->api_cb_ptr->query_status))
  {
    ps_dnsi_api_delete_instance( session_mgr_ptr->api_cb_ptr->handle );
    session_mgr_ptr->api_cb_ptr = NULL;
  }
  if (NULL != session_mgr_ptr->api_cb_ptr)
  {
    LOG_MSG_ERROR_0("dss_dns_get_nameinfo(): Session has already started a query");
    *dss_errno = DS_EOPNOTSUPP;
    ps_dnsi_resolver_delete_instance(api_cb_ptr->res_handle);
    ps_dnsi_api_delete_instance(api_cb_ptr->handle);
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }
  session_mgr_ptr->api_cb_ptr = api_cb_ptr;

  retval = ps_dnsi_resolver_start_query( api_cb_ptr->res_handle,
                                         api_cb_ptr->queries.query_type[0],
                                         api_cb_ptr->queries.query_class[0],
                                         api_cb_ptr->queries.query_data,
                                         ps_dnsi_api_resolver_event_notify,
                                         (void *)api_cb_ptr,
                                         dss_errno );
  if( DSS_ERROR != retval || DS_EWOULDBLOCK != *dss_errno )
  {
    LOG_MSG_ERROR_1("Cant start resolver query: error %d", *dss_errno);
    ps_dnsi_resolver_delete_instance( api_cb_ptr->res_handle );
    ps_dnsi_api_delete_instance( api_cb_ptr->handle );
    session_mgr_ptr->api_cb_ptr = NULL;
    return DSS_DNS_QUERY_INVALID_HANDLE;
  }

  *dss_errno = DS_EWOULDBLOCK;
  return api_cb_ptr->handle;
} /* dss_dns_get_nameinfo() */

/*===========================================================================
FUNCTION  DSS_DNS_READ_ADDRINFO()

DESCRIPTION
  This function reads the dss_dns_addrinfo records which were looked up
  for earlier using dss_dns_get_addrinfo.

PARAMETERS
 session_handle -  Session Identifier
 query_handle   -  Query identifier
 results_buf    -  Results buffer
 num_records    -  Number of records
 dss_errno      -  Error code

RETURN VALUE
  DSS_SUCCESS on success and reads the results into the results buffer
  DSS_ERROR on error.

  dss_errno values
  ----------------
  DS_EBADF       - Invalid session or query handle
  DS_EWOULDBLOCK - Operation not complete yet.


DEPENDENCIES
  This function must be called after the application has been signalled
  using callback function after dss_dns_get_addrinfo.

SIDE EFFECTS
  The API control block instance, as well as the results Q are freed.
===========================================================================*/
int16 dss_dns_read_addrinfo
(
  dss_dns_session_mgr_handle_type     session_handle,
  dss_dns_query_handle_type           query_handle,
  dss_dns_addrinfo                  * results_buf,
  uint16                              num_records,
  int16                             * dss_errno
)
{
  ps_dnsi_api_cb_type              * api_cb_ptr = NULL;
  ps_dnsi_session_mgr_cb_type      * session_mgr_ptr = NULL;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_dns_read_addrinfo(): "
    "caller 0x%p",
    caller_ptr);

  LOG_MSG_INFO2_3("dss_dns_read_addrinfo(): "
    "dss_dns_read_addrinfo called with sess %d, handle %d, num recs %d",
    session_handle, query_handle, num_records);

  if( NULL == dss_errno )
  {
    LOG_MSG_ERROR_0("dss_errno is NULL");
    return DSS_ERROR;
  }

  if( FALSE == dss_dns_session_mgr_is_handle_valid( session_handle ) )
  {
    LOG_MSG_ERROR_1("Session handle %d is invalid ", session_handle);
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }

  api_cb_ptr = ps_dnsi_api_is_handle_valid( query_handle );
  if( NULL == api_cb_ptr )
  {
    LOG_MSG_ERROR_1("Query handle %d is invalid ", query_handle);
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }

  if( PS_DNSI_API_CB_STATE_FREE != api_cb_ptr->state )
  {
    LOG_MSG_ERROR_1("API control block invalid state %d", api_cb_ptr->state);
    *dss_errno = DS_EWOULDBLOCK;
    return DSS_ERROR;
  }

  if( NULL == results_buf || 0 == num_records )
  {
    LOG_MSG_INFO1_0("NULL results buf or 0 length");
  }
  else
  {
    dss_dnsi_construct_ai_results( &api_cb_ptr->answer_q,
                                   api_cb_ptr->addr_family,
                                   api_cb_ptr->sock_type,
                                   api_cb_ptr->transport_proto,
                                   (dss_dns_ai_flags_enum_type)api_cb_ptr->flags,
                                   api_cb_ptr->port_num,
                                   results_buf,
                                   num_records );
  }

  ps_dnsi_resolver_delete_instance( api_cb_ptr->res_handle );
  ps_dnsi_api_delete_instance( api_cb_ptr->handle );
  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);
  if (NULL != session_mgr_ptr)
  {
    session_mgr_ptr->api_cb_ptr = NULL;
  }

  return DSS_SUCCESS;
} /* dss_dns_read_addrinfo() */

/*===========================================================================
FUNCTION  DSS_DNS_READ_NAMEINFO()

DESCRIPTION
  This function reads the dss_dns_nameinfo records which were looked up
  for earlier using dss_dns_get_nameinfo.

PARAMETERS
 session_handle -  Session Identifier
 query_handle   -  Query identifier
 results_buf    -  Results buffer
 num_records    -  Number of records
 dss_errno      -  Error code

RETURN VALUE
  DSS_SUCCESS on success and reads the results into the results buffer
  DSS_ERROR on error.

  dss_errno values
  ----------------
  DS_EBADF       - Invalid session or query handle
  DS_EWOULDBLOCK - Operation not complete yet.

DEPENDENCIES
  This function must be called after the application has been signalled
  using callback function after dss_dns_get_nameinfo.

SIDE EFFECTS
  The API control block instance, as well as the results Q are freed.
===========================================================================*/
int16 dss_dns_read_nameinfo
(
  dss_dns_session_mgr_handle_type     session_handle,
  dss_dns_query_handle_type           query_handle,
  dss_dns_nameinfo                  * results_buf,
  uint16                              num_records,
  int16                             * dss_errno
)
{
  ps_dnsi_api_cb_type              * api_cb_ptr = NULL;
  ps_dnsi_session_mgr_cb_type      * session_mgr_ptr = NULL;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_dns_read_nameinfo(): "
    "caller 0x%p",
    caller_ptr);

  LOG_MSG_INFO2_3("dss_dns_read_nameinfo(): "
    "dss_dns_read_nameinfo called with sess %d, handle %d, num recs %d",
    session_handle, query_handle, num_records);

  if( NULL == dss_errno )
  {
    LOG_MSG_ERROR_0("dss_errno is NULL");
    return DSS_ERROR;
  }

  if( FALSE == dss_dns_session_mgr_is_handle_valid( session_handle ) )
  {
    LOG_MSG_ERROR_1("Session handle %d is invalid ", session_handle);
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }

  api_cb_ptr = ps_dnsi_api_is_handle_valid( query_handle );
  if( NULL == api_cb_ptr )
  {
    LOG_MSG_ERROR_1("Query handle %d is invalid ", query_handle);
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }

  if( PS_DNSI_API_CB_STATE_FREE != api_cb_ptr->state )
  {
    LOG_MSG_ERROR_1("API control block invalid state %d", api_cb_ptr->state);
    *dss_errno = DS_EWOULDBLOCK;
    return DSS_ERROR;
  }

  if( NULL == results_buf || 0 == num_records )
  {
    LOG_MSG_INFO1_0("NULL results buf or 0 length");
  }
  else
  {
    dss_dnsi_construct_ni_results( &api_cb_ptr->answer_q,
                                   results_buf,
                                   num_records );
  }

  ps_dnsi_resolver_delete_instance( api_cb_ptr->res_handle );
  ps_dnsi_api_delete_instance( api_cb_ptr->handle );
  session_mgr_ptr = dss_dns_session_mgri_get_cb_ptr(session_handle);
  if (NULL != session_mgr_ptr)
  {
    session_mgr_ptr->api_cb_ptr = NULL;
  }
  return DSS_SUCCESS;
} /* dss_dns_read_nameinfo() */

/*===========================================================================
FUNCTION  DSS_DNS_CLEAR_CACHE()

DESCRIPTION
  This function clears the cache of the specified iface asynchronously.

PARAMETERS
 iface_id           -  Interface ID
 dss_errno          -  Error code

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on error.

  dss_errno values
  ----------------
  DS_EFAULT        - Invalid arguments.

DEPENDENCIES
  None.
SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_clear_cache
(
  dss_iface_id_type                   iface_id,
  int16                             * dss_errno
)
{
  ps_iface_type * iface_ptr = NULL;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_dns_clear_cache(): "
    "caller 0x%p", caller_ptr);

  LOG_MSG_INFO2_1("dss_dns_clear_cache(): "
    "dss_dns_clear_cache called with iface_id %d", iface_id);

  if( NULL == dss_errno )
  {
    LOG_MSG_ERROR_0("dss_errno is NULL");
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Determine the specific ps_iface_ptr based on the interface name and
    instance passed in.
  -------------------------------------------------------------------------*/
  if ( DSS_IFACE_INVALID_ID == iface_id )
  {
    LOG_MSG_ERROR_0 ("Iface id is invalid");
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }
  // TODO: use a DSS function that will handle this transformation
  iface_ptr = ps_iface_get_handle( (ps_iface_id_type) ((iface_id & 0xFF000000) | 0x00FFFF00) );
  if (iface_ptr == NULL)
  {
    LOG_MSG_ERROR_1("Invalid iface id: 0x%x", iface_id);
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  return ps_iface_dns_cache_flush( iface_ptr, dss_errno );
}

/*===========================================================================
FUNCTION  DSS_DNS_CLEAR_CACHE_ENTRY()

DESCRIPTION
  This function clears the specified host name from the cache 
  of the specified iface asynchronously.

PARAMETERS
 iface_id           -  Interface ID
 hostname_ptr       -  Host name
 dss_errno          -  Error code

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on error.

  dss_errno values
  ----------------
  DS_NAMEERR - Invalid host name.
  DS_EFAULT  - Invalid arguments.
  DS_ENOMEM  - Out of memory.

DEPENDENCIES
  None.
SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_clear_cache_entry
(
  dss_iface_id_type                   iface_id,
  const char                        * hostname_ptr,
  int16                             * dss_errno
)
{
  ps_iface_type * iface_ptr = NULL;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_dns_clear_cache_entry(): "
    "caller 0x%p", caller_ptr);

  LOG_MSG_INFO2_1("dss_dns_clear_cache_entry(): "
    "dss_dns_clear_cache_entry called with iface_id %d",
    iface_id);

  if( NULL == dss_errno )
  {
    LOG_MSG_ERROR_0("dss_errno is NULL");
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Determine the specific ps_iface_ptr based on the interface name and
    instance passed in.
  -------------------------------------------------------------------------*/
  if ( DSS_IFACE_INVALID_ID == iface_id )
  {
    LOG_MSG_ERROR_0 ("Iface id is invalid");
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }
  // TODO: use a DSS function that will handle this transformation
  iface_ptr = ps_iface_get_handle( (ps_iface_id_type) ((iface_id & 0xFF000000) | 0x00FFFF00) );
  if (iface_ptr == NULL)
  {
    LOG_MSG_ERROR_1("Invalid iface id: 0x%x", iface_id);
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if( (NULL == hostname_ptr) ||
      (FALSE == dss_dnsi_validate_hostname_query( hostname_ptr, NULL ) ) 
    )
  {
    LOG_MSG_ERROR_1("Invalid hostname argument 0x%p", hostname_ptr);
    *dss_errno = DS_NAMEERR;
    return DSS_ERROR;
  }

  return ps_iface_dns_cache_flush_entry( iface_ptr, hostname_ptr, dss_errno );
}
