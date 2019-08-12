#ifndef DSSDNS_H
#define DSSDNS_H
/*===========================================================================

     D A T A   S E R V I C E S   D N S   A P I   H E A D E R   F I L E

DESCRIPTION

  The Data Services DNS Header File. Contains declarations of functions
  exported by the DNS subsystem API as well as shared variable, macros,
  common structs and enums.

EXTERNALIZED FUNCTIONS

  1. Initialization
  dss_dns_init()
    Initializes the DNS subsystem during powerup.

  2. For Session Management
  dss_dns_create_session()
    Creates a session corresponding to a set of DNS lookups.

  dss_dns_set_config_params()
    Sets the specified configuration options for a given session.

  dss_dns_get_config_params()
    Retrieves the configuration option for a given session.

  dss_dns_delete_session()
    Deletes a specified session.

  3. For DNS Lookup (RFC 3493)
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

  4. Address utility Functions
  dss_inet_aton()
    Given a string with an IPv4 address in dot format returns the host
    address in network order.

  dss_inet_ntoa()
    Given an IPv4 address returns it in dot string format.

  dss_inet_pton()
    Given an IPv4 or IPv6 address in presentation format, converts
    to ps_in_addr or ps_in6_addr.

  dss_inet_ntop()
    Given an IPv4 or IPv6 address in network format(ps_in_addr or ps_in6_addr)
    converts to dotted decimal(for IPv4) or colon seperated(for IPv6)
    presentation.

  5. Deprecated API (All new applications should use new DNS API.)
  dss_getipnodebyname()
    POSIX IPv6 compliant function returns the resolved IP address in
    dss_hostent structure given a host name.  Reference RFC 2553.

  dss_getipnodebyaddr()
    POSIX IPv6 compliant.  Returns resolved hostname in dss_hostent structure
    given a host IP address. Reference RFC 2553.

  dss_freehostent()
    Frees the dss_hostent structure returned by dss_getipnodebyname() and
    dss_getipnodebyaddr().  Reference RFC 2553.

  dss_gethostaddrs()
    Obtain host IP address by host name from the DNS server.  Legacy API
    function, deprecated.

  6. For Cache Management
  dss_dns_clear_cache()
    Clears the cache of the specified iface.
  
  dss_dns_clear_cache_entry()
    Clears the specified host name from the cache 
    of the specified iface.

INITIALIZATIONS AND SEQUENCING REQUIREMENTS
  The DNS subsystem must be inited by calling dss_dns_init() function
  during powerup.

Copyright (c) 2007-2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "dssocket_defs.h"

/*===========================================================================

                            PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Max number of supported simultaneous queries (in all sessions).
---------------------------------------------------------------------------*/
#define DSS_DNS_MAX_QUERIES        16

/*---------------------------------------------------------------------------
  Max number of supported simultaneous sessions.
---------------------------------------------------------------------------*/
#define DSS_DNS_MAX_SESSIONS       10

/*---------------------------------------------------------------------------
  Maximum length of domain names and service names
---------------------------------------------------------------------------*/
#define DSS_DNS_MAX_DOMAIN_NAME_LEN     256
#define DSS_DNS_MAX_SERV_NAME_LEN       256
#define DSS_DNS_MAX_LABEL_LEN            64

/*---------------------------------------------------------------------------
  Typedef for session handle and query handles.
---------------------------------------------------------------------------*/
typedef int32 dss_dns_session_mgr_handle_type;
#define DSS_DNS_SESSION_MGR_INVALID_HANDLE  (-1)

typedef int32 dss_dns_query_handle_type;
#define DSS_DNS_QUERY_INVALID_HANDLE  (-1)

/*---------------------------------------------------------------------------
  Typedef for retry mechanism parameters
---------------------------------------------------------------------------*/
typedef struct dss_dns_linear_backoff_retry_info_type
{
  int32 first_retry_time;
  int32 backoff;
  int32 max_retries;
} dss_dns_linear_backoff_retry_info_type;

/*---------------------------------------------------------------------------
  Definition of various configuration parameters that affect the behavior of
  DNS subsystem.

+-------------------------------------------+--------------+----------------------------------------+
| Param name                                |     Type     |  Default value                         |
+-------------------------------------------+--------------+----------------------------------------+
| DSS_DNS_CONFIG_PARAMS_MIN                 |  N/A         |  N/A                                   |
| DSS_DNS_CONFIG_PARAMS_IFACE_ID            |  dss_iface_  |  DSS_IFACE_INVALID_ID                  |
|                                           |  id_type     |                                        |
| DSS_DNS_CONFIG_PARAMS_ROUTEABLE_IFACE_MODE|  boolean     |  FALSE                                 |
| DSS_DNS_CONFIG_PARAMS_NET_POLICY          |  dss_net_    |  dss_init_net_policy_info()            |
|                                           |  policy_info_|                                        |
|                                           |  type        |                                        |
| DSS_DNS_CONFIG_PARAMS_USE_CACHE_RESULTS   |  boolean     |  TRUE                                  |
| DSS_DNS_CONFIG_PARAMS_ADD_TO_CACHE        |  boolean     |  TRUE                                  |
|                                           |              |                                        |
| DSS_DNS_CONFIG_PARAMS_RECURSION_DESIRED   |  boolean     |  TRUE                                  |
|                                           |              |                                        |
| DSS_DNS_CONFIG_PARAMS_                    |  dss_dns_    |  first_retry_time = 3000 (milliseconds)|
|         LINEAR_BACKOFF_RETRY_INFO         |  query_retry_|  backoff = 0                           |
|                                           |  parameters_ |  retry_number = 7                      |
|                                           |  info_type   |                                        |
|                                           |              |                                        |
| DSS_DNS_CONFIG_RESERVED_1                 |  int         |  0                                     |
|                                           |              |                                        |
| DSS_DNS_CONFIG_PARAMS_MAX                 |  N/A         |  N/A                                   |
+-------------------------------------------+--------------+----------------------------------------+

  Explanation of parameters:
  IFACE_ID             - The DNS subsystem sets the iface id parameter in the
                         default network policy and performs a lookup for the
                         corresponding interface. It however does not bring 
                         up the interface. 
  ROUTEABLE_IFACE_MODE - Specifies whether to do DNS lookup when
                         laptop call is active.
  NET_POLICY           - Using this option the application can set the network
                         policy to specify the interface.It is the app's 
                         responsibility to bring up the network.
  USE_CACHE_RESULTS    - Specifies if the cache results can be used. If the
                         application does not want to use cache results,
                         it should set this flag to FALSE.
  ADD_TO_CACHE         - Specifies whether the results of the DNS lookup
                         should be cached or not.
  RECURSION_DESIRED    - Specifies if the RD (Recursion Desired) should be
                         requested from the server. In any case the mobile 
                         is not doing recursion. Therefore, it should 
                         not be disabled unless it is to overcome a 
                         carrier specific DNS server behavior issue or 
                         something similar.

  LINEAR_BACKOFF_RETRY_INFO
                       - Using this option the application can set the retry 
                         mechanism information:
                         first_retry_time - number of miliseconds the resolver will
                                            wait for an answer, from the moment the
                                            query was sent until a retry will be
                                            performed.
                         backoff          - number of miliseconds added to the
                                            waiting time between two consecutive
                                            retries; e.g. if the first_retry_time is
                                            3000 and the backoff is 1000, we will wait
                                            3 seconds until the first retry; then 4
                                            seconds until the second retry; then 5
                                            seconds until the third retry etc.
                         retry_number     - number of total retries.

  RESERVED_1           - Do not use this parameter.
                
---------------------------------------------------------------------------*/

typedef enum
{
  DSS_DNS_CONFIG_PARAMS_MIN                       = 0,
  DSS_DNS_CONFIG_PARAMS_IFACE_ID                  = DSS_DNS_CONFIG_PARAMS_MIN,
  DSS_DNS_CONFIG_PARAMS_ROUTEABLE_IFACE_MODE      = 1,
  DSS_DNS_CONFIG_PARAMS_NET_POLICY                = 2,
  DSS_DNS_CONFIG_PARAMS_USE_CACHE_RESULTS         = 3,
  DSS_DNS_CONFIG_PARAMS_ADD_TO_CACHE              = 4,
  DSS_DNS_CONFIG_PARAMS_RECURSION_DESIRED         = 5,
  DSS_DNS_CONFIG_PARAMS_LINEAR_BACKOFF_RETRY_INFO = 6,
  DSS_DNS_CONFIG_PARAMS_RESERVED_1                = 7,
  DSS_DNS_CONFIG_PARAMS_MAX
} dss_dns_config_params_enum_type;

 /*--------------------------------------------------------------------------
   Enum for the different types of DNS API supported.
 --------------------------------------------------------------------------*/
typedef enum
{
  DSS_DNS_API_MIN           = 0,
  DSS_DNS_API_ADDRINFO      = DSS_DNS_API_MIN,
  DSS_DNS_API_NAMEINFO      = 1,
  DSS_DNS_API_MAX           = 5
} dss_dns_api_type_enum_type;

/*---------------------------------------------------------------------------
  Addrinfo flags type
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_DNS_AI_FLAGS_NONE        = 0x0000,
  DSS_DNS_AI_FLAGS_PASSIVE     = 0x0001,
  DSS_DNS_AI_FLAGS_CANONICAL   = 0x0002,
  DSS_DNS_AI_FLAGS_NUMERICHOST = 0x0004,
  DSS_DNS_AI_FLAGS_V4MAPPED    = 0x0008,
  DSS_DNS_AI_FLAGS_ALL         = 0x0010,
  DSS_DNS_AI_FLAGS_ADDRCONFIG  = 0x0020,
  DSS_DNS_AI_FLAGS_RESERVED_1  = 0x8000
} dss_dns_ai_flags_enum_type;

/*---------------------------------------------------------------------------
  Nameinfo flags type
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_DNS_NI_FLAGS_NOFQDN      = 0x0001,
  DSS_DNS_NI_FLAGS_NUMERICHOST = 0x0002,
  DSS_DNS_NI_FLAGS_NAMEREQD    = 0x0004
} dss_dns_ni_flags_enum_type;


/*---------------------------------------------------------------------------
  The host entry structure
---------------------------------------------------------------------------*/
typedef struct dss_hostent
{
  char  *h_name;                       /* Official host name               */
  char **h_aliases;                    /* NULL-terminated alias list       */
  int    h_addrtype;                   /* host address type                */
  int    h_length;                     /* length of address                */
  char **h_addr_list;                  /* NULL-terminated address list     */
} dss_hostent;

/*---------------------------------------------------------------------------
  The addrinfo structure
---------------------------------------------------------------------------*/
typedef struct dss_dns_addrinfo
{
  dss_dns_ai_flags_enum_type   ai_flags;    /* AI_PASSIVE,AI_CANONNAME etc */
  uint16                       ai_family;   /* DSS_AF_INET,DSS_AF_INET6,
                                                              DSS_AF_UNSPEC*/
  uint16                       ai_socktype; /* DSS_SOCK_STREAM, DSS_SOCK_DGRAM     */
  uint16                       ai_protocol; /* PS_IPPROTO_TCP, PS_IPPROTO_UDP*/
  char                         ai_canonname[DSS_DNS_MAX_DOMAIN_NAME_LEN];
                                            /* Canonical name if any       */
  struct ps_sockaddr_storage   ai_sockaddr; /* Generic socket address      */
  uint32                       ttl;         /* Record time to live (sec)   */
} dss_dns_addrinfo;

/*---------------------------------------------------------------------------
  The nameinfo structure
---------------------------------------------------------------------------*/
typedef struct dss_dns_nameinfo
{
  char  hostname[DSS_DNS_MAX_DOMAIN_NAME_LEN];  /* Hostname String         */
  char  servname[DSS_DNS_MAX_SERV_NAME_LEN];    /* Service name string     */
  uint32        ttl;                            /* Record time to live (sec) */
} dss_dns_nameinfo;

/*---------------------------------------------------------------------------
  CALLBACK FUNCTION DECLARATIONS
---------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION TYPEDEF DSS_DNS_CBACK_F_TYPE

DESCRIPTION
  Callback function to set in the session control block. This function
  should be set in the session control block while creating a session.

PARAMETERS

  session_handle -  Identifies the session
  query_handle   -  Identifies query instance
  api_type       -  Type of resource record
  num_records    -  Number of records
  user_data_ptr  -  Application user data
  dss_errno      -  Error code if any

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
typedef void (*dss_dns_cback_f_type)
(
  dss_dns_session_mgr_handle_type   session_handle,
  dss_dns_query_handle_type         query_handle,
  dss_dns_api_type_enum_type        api_type,
  uint16                            num_records,
  void                            * user_data_ptr,
  int16                             dss_errno
);

/*===========================================================================

FUNCTION TYPEDEF DSS_DNS_HOSTENT_CBACK_F_TYPE

DESCRIPTION

  Typedef for the dss_getipnodebyname() and dss_getipnodebyaddr() callback
  function.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
typedef void (*dss_dns_hostent_cback_f_type)
(
  struct dss_hostent * hostent_ptr,
  void               * cb_voidptr,
  int16                dss_errno
);

/* For legacy applications which use dss_dns_cb_f_type for hostent cbacks  */
#define dss_dns_cb_f_type dss_dns_hostent_cback_f_type


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

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
);

/*===========================================================================

FUNCTION DSS_DNS_GETIPNODEBYNAME

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

  At present support for IPv6 is not added.

PARAMETERS
  name              string with name of an ipv4 or ipv6 host or dotted
                    decimal ipv4 or colon seperated/dotted decimal ipv6 host
  af                address family of name.  DSS_AF_INET or DSS_AF_INET6
  flags             options - none currently supported.
  callback          callback function pointer
  cb_voidptr        voidptr passed in by the application.  This is returned
                    back to the application in the callback.
  dss_errno         error code returned

DEPENDENCIES
  None.

RETURN VALUE
  Address of a dss_hostent structure if successful.
  If no mapping found then returns a NULL and the *dss_errno is set to one of
  the following values.

  dss_errno values
  ----------------
  DS_EHOSTNOTFOUND    domain name asked is not known.
  DS_ETRYAGAIN        temporary and transient error e.g., resolver out of
                      resources, server temporarily unreachable.
  DS_ENORECOVERY      irrecoverable server error occurred
  DS_EFAULT           invalid parameters passed to function
  DS_EAFNOSUPPORT     invalid value for the address family parameter
  DS_EOPNOTSUPP       invalid value for the flags parameter
  DS_ENOMEM           out of memory in the DNS subsystem
  DS_NAMEERR          domain name is malformed
  DS_EWOULDBLOCK      DNS servers being queried would invoke the callback
                      with the answer

SIDE EFFECTS
  Memory allocated to hostent structures is from DNS subsystem heap. The
  application must copy the hostent results to its memory and call
  dss_freehostents().

===========================================================================*/
struct dss_hostent *dss_getipnodebyname
(
  char                            * hostname_ptr,
  int32                             addr_family,
  int32                             flags,
  dss_dns_hostent_cback_f_type      cback_f_ptr,
  void                            * user_data_ptr,
  int16                           * dss_errno
);

/*===========================================================================

FUNCTION DSS_GETIPNODEBYADDR

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
  addr              based on the value of 'af' this will be either a
                    'struct ps_in_addr' (for af == DSS_AF_INET)  or 'struct
                    ps_in6_addr' (for af == DSS_AF_INET6)
  len               length of the addr field
  af                address family of addr DSS_AF_INET or DSS_AF_INET6
  callback          callback function pointer
  cb_voidptr        voidptr passed in by the application.  This is returned
                    back to the application in the callback.
  dss_errno         error code returned

DEPENDENCIES
  None.

RETURN VALUE
  Address of a hostent structure if successful.
  If no mapping found then returns a NULL and the *dss_errno is set to one of
  the following values.

  dss_errno values
  ----------------
  DS_EHOSTNOTFOUND    the specified host is unknown
  DS_ETRYAGAIN        temporary and transient error e.g., resolver out of
                      resources, server temporarily unreachable.
  DS_EFAULT           invalid parameters passed to function
  DS_EAFNOSUPPORT     invalid value for the address family parameter
  DS_ENORECOVERY      irrecoverable server error occurred
  DS_ENOMEM           out of memory in the DNS subsystem
  DS_NAMEERR          query is malformed
  DS_EWOULDBLOCK      query being processed would invoke the callback
                      with the answer

SIDE EFFECTS
  Memory allocated to hostent structures is from DNS subsystem heap. The
  application must copy the hostent results to its memory and call
  dss_freehostents().

===========================================================================*/
struct dss_hostent *dss_getipnodebyaddr
(
  void                            * addr_ptr,
  int32                             addr_len,
  int32                             addr_family,
  dss_dns_hostent_cback_f_type      cback_f_ptr,
  void                            * user_data_ptr,
  int16                           * dss_errno
);

/*===========================================================================
FUNCTION DSS_FREEHOSTENT

DESCRIPTION
  This function is called to free the dss_hostent structure returned by
  dss_getipnodebyname() or dss_getipnodebyaddr() functions.  It also frees
  all associated data structures used to resolve the query.

PARAMETERS
  phostent    pointer to dss_hostent structure to be freed

DEPENDENCIES
  The query entry to be freed should be in the valid q

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dss_freehostent
(
  struct dss_hostent *phostent         /* Pointer to dss_hostent to free   */
);

/*===========================================================================
FUNCTION DSS_INET_ATON

DESCRIPTION
  Convert internet address from dotted string to network ordered struct
  ps_in_addr.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_ERROR    For failure
  DSS_SUCCESS  For success

  The IP address is returned in struct ps_in_addr *addr

SIDE EFFECTS
  None.
===========================================================================*/
int32 dss_inet_aton
(
  const  char       *cp,               /* IPv4 addr in dotted quad string  */
  struct ps_in_addr *addr              /* Returned IPv4 address            */
);

/*===========================================================================
FUNCTION  DSS_INET_NTOA

DESCRIPTION
  Convert network order internet address to dotted string form.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS   If successful
  DSS_ERROR     If an error occured

SIDE EFFECTS
  None.
===========================================================================*/
int32 dss_inet_ntoa
(
  const struct ps_in_addr  in,     /* IPv4 address to be converted         */
  uint8                   *buf,    /* Buffer to hold the converted address */
  int32                    buflen  /* Length of buffer                     */
);

/*===========================================================================
FUNCTION  DSS_INET_PTON

DESCRIPTION
  This function is called with a presentation (printable or ASCII) format
  address to be converted to its network address (binary) format.  The af
  argument can be either DSS_AF_INET if the address to be converted is an IPv4
  address or DSS_AF_INET6 if the address is an IPv6 address.  In case of error
  the error code is returned in the dss_errno argument.

DEPENDENCIES
  The dst argument should have sufficient memory for the network address
  of the appropriate family.  For IPv4 it should be at least
  sizeof(struct ps_in_addr) while for IPv6 it should be at least
  sizeof(struct ps_in6_addr).

RETURN VALUE
  DSS_SUCCESS in case of success with the network format address
              returned in the dst argument.
  DSS_ERROR   in case of error with the error code returned in dss_errno
              argument.

              dss_errno values returned:
              DS_EFAULT         invalid arguments passed to function
              DS_EAFNOSUPPORT   invalid value for the address family
                                argument
              DS_NAMEERR        Malformed address passed to be converted
              DS_EMSGTRUNC      Insufficient buffer space in return argument

SIDE EFFECTS
  Returns the converted printable format IPv6 address in the dst argument.
  Any errors are returned in dss_errno argument.
===========================================================================*/
int32 dss_inet_pton
(
  const char *src,       /* String containing presentation form IP address */
  int32       af,        /* Address family of address in src argument      */
  void       *dst,       /* Memory for returning address in network format */
  uint32      dst_size,  /* Size of memory passed in dst argument          */
  int16      *dss_errno  /* Error code returned in case of DSS_ERROR return*/
);

/*===========================================================================
FUNCTION  DSS_INET_NTOP

DESCRIPTION
  This function is called with an IPv4 or IPv6 address in network (binary)
  format to be converted into the presentation (dotted decimal for IPv4
  or colon seperated for IPv6) format.  The af argument can be either
  DSS_AF_INET if the address to be converted is an IPv4 address or DSS_AF_INET6 if
  the address is an IPv6 address.  In case of error the error code is
  returned in the dss_errno argument.

DEPENDENCIES
  The dst argument should have sufficient memory for the string address.
  To convert an IPv4 address it is suggested to be PS_IN_ADDRSTRLEN
  in size while to convert an IPv6 address it is suggested to be 
  PS_IN6_ADDRSTRLEN.

RETURN VALUE
  DSS_SUCCESS in case of success with the presentation format address
              returned in the dst argument.
  DSS_ERROR   in case of error with the error code returned in dss_errno
              argument.

              dss_errno values returned:
              DS_EFAULT         invalid arguments passed to function
              DS_EAFNOSUPPORT   invalid value for the address family
                                argument
              DS_NAMEERR        Malformed address passed to be converted
              DS_EMSGTRUNC      Insufficient buffer space in return argument
              DS_ENORECOVERY    Failed to convert

SIDE EFFECTS
  Returns the converted printable format IPv6 address in the dst argument.
  Any errors are returned in dss_errno argument.
===========================================================================*/
int32 dss_inet_ntop
(
  const char *src,       /* Memory containing network form IP address      */
  int32       af,        /* Address family of address in src argument      */
  void       *dst,       /* Memory to return address in presentation format*/
  uint32      dst_size,  /* Size of memory passed in dst argument          */
  int16      *dss_errno  /* Error code returned in case of DSS_ERROR return*/
);

/*===========================================================================
FUNCTION  DSS_DNS_CREATE_SESSION()

DESCRIPTION
  The function creates a session control block corresponding to a set of DNS
  lookups. The created session must be cleaned by the application after
  all the DNS lookups are complete. The application MUST specify callback
  information while creating a session.

PARAMETERS
  app_cback_f_ptr   - Application registered callback function pointer.
  user_data_ptr     - Application user data. (Optional)
  dss_errno             - Error code returned in case of error.

DEPENDENCIES
  None.

RETURN VALUE
  Session handle on success
  DSS_DNS_SESSION_MGR_INVALID_HANDLE on error and sets the dss_errno to
  error code.

  dss_errno values
  ------------
  DS_EFAULT     - NULL callback info provided.
  DS_ENOMEM     - No memory available to allocate control block.

SIDE EFFECTS
  None.
===========================================================================*/
dss_dns_session_mgr_handle_type dss_dns_create_session
(
  dss_dns_cback_f_type    app_cback_f_ptr,
  void                  * user_data_ptr,
  int16                 * dss_errno
);

/*===========================================================================
FUNCTION  DSS_DNS_SET_CONFIG_PARAMS()

DESCRIPTION
  This function sets a specified configuration parameter for a given session.

PARAMETERS
  session_handle    - Identifies the session.
  param_name        - Name of the configuration parameter to be set.
  param_val_ptr     - Value of the parameter to be set.
  param_len         - Identifies the length of the param_val_ptr that
                      can be used.
  dss_errno             - Error code in case of error.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on failure and sets error code in dss_errno.

  dss_errno values
  ------------
  DS_EBADF       - Wrong session handle
  DS_EFAULT      - Invalid arguments.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_set_config_params
(
  dss_dns_session_mgr_handle_type     session_handle,
  dss_dns_config_params_enum_type     param_name,
  void                              * param_val_ptr,
  uint32                              param_len,
  int16                             * dss_errno
);

/*===========================================================================
FUNCTION  DSS_DNS_GET_CONFIG_PARAMS()

DESCRIPTION
  This function gets a specified configuration parameter for a given session.

PARAMETERS
  session_handle    - Identifies the session.
  param_name        - Name of the configuration parameter to get.
  param_val_ptr     - Value of the parameter to be set.
  param_len         - Identifies the length of the param_val_ptr that
                      can be filled in.
  dss_errno             - Error code in case of error.

DEPENDENCIES
  None.

RETURN VALUE

  DSS_SUCCESS on success: param_val_ptr contains the result and param_len
  contains the length of the results.
  DSS_ERROR on failure and sets error code in dss_errno.

  dss_errno values
  ----------------
  DS_EBADF       - Wrong session handle
  DS_EFAULT      - Invalid arguments.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_get_config_params
(
  dss_dns_session_mgr_handle_type     session_handle,
  dss_dns_config_params_enum_type     param_name,
  void                              * param_val_ptr,
  uint32                              param_len,
  int16                             * dss_errno
);

/*===========================================================================
FUNCTION  DSS_DNS_DELETE_SESSION()

DESCRIPTION
  This function deletes a session specified by the session handle.

DEPENDENCIES
  None

PARAMETERS
  session_handle  - Session identifier
  dss_errno           - Error code in case of error

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on failure and sets the error code in dss_errno.

  dss_errno values
  ------------
  DS_EBADF     - Wrong session handle

SIDE EFFECTS
  All the active queries associated with the session will be silently
  dropped without notifying the application.
===========================================================================*/
int16 dss_dns_delete_session
(
  dss_dns_session_mgr_handle_type     session_handle,
  int16                             * dss_errno
);

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
  DS_NAMERR     - Malformed query
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
);

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
  dss_errno       - Error code

RETURN VALUE
  Returns a handle to the query started on success
  Returns DSS_DNS_QUERY_INVALID_HANDLE on error.

  dss_errno values
  ------------
  DS_EFAULT     - Invalid arguments
  DS_EBADF      - Invalid session handle
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
);

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
 dss_errno          -  Error code

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
);

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
 dss_errno          -  Error code

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
);

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
);

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
);

#endif  /* DSSDNS_H */
