#ifndef DSSINTERNAL_H
#define DSSINTERNAL_H

/*===========================================================================

         I N T E R N A L  D A T A  S E R V I C E  S O C K E T S
                       H E A D E R  F I L E

DESCRIPTION

 The Internal Data Services Sockets Header File. Contains shared variables
 and enums, as well as declarations for functions for each protocol.


Copyright (c) 2003-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssinternal.h_v   1.3   12 Feb 2003 19:34:06   akuzhiyi  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dssinternal.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/26/09    pp     CMI De-featurization.
07/11/06    msr    Added more filter validation in dss_reg_ip_filter()
04/11/05    ifk    Added dss_reg_ip_filter and dss_dereg_ip_filter
03/30/04    vp     Merged changes from June04Dev_05.00.00 ps branch.
12/29/03    usb    Deprecated non sys socket flow functionality.
08/28/03    ss     Modified to use dss_iface_id_type
07/03/03    usb    Added interface to set routeable app option.
06/25/03    ss     Moved dss_sendto_dsm_chain() and dss_recvfrom_dsm_chain()
                   from dssocket.h to this file
05/05/03    jd     Removed dependency on dsiface.h
04/25/03    aku    Added dss_set_specified_iface() and removed unused
                   dss_is_mip_up_on_um() and dssi_force_dorm() prototypes.
02/10/03    aku    Added support for flow control of non system sockets on a
                   per interface basis.
12/13/02    ifk    Renamed function parameter from errno to dss_errno.
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/27/02    ss     Moved socket options related declarations to dssocket.h
08/16/02    ss     Added DSS_SO_SILENT_CLOSE socket option.
07/24/02    aku    Featurized dss_ppp_in_use() with FEATURE_DATA_MM.
01/02/02    aku    dss_ppp_in_use() now returns uint instead of a boolean.
11/26/01    js     Added prototype for dssi_force_dorm().
11/15/01    aku    Added prototype for dss_is_ppp_in_use().
07/28/01    na     Added prototype for dss_is_mip_up_on_um().
06/26/01    na     Added the prototype for dssocki_set_non_sys_socket_flow()
06/26/01    sjy    Added system socket option DSS_SO_SYS_SOCK, and deleted
                   DSS_SO_ZERO_IP_OK option.
04/06/01    js     Added definitinon for socket option DSS_SO_ZERO_IP_OK.
04/06/01    js     Added support for socket level socket options.
03/28/01    js     Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "dssocket_defs.h"
#include "ps_ipfltr_defs.h"
#include "dss_netpolicy.h"


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION DSS_WRITE_DSM_CHAIN

DESCRIPTION
  Sends the specified DSM item chain over the TCP transport.  Lower layers
  will be responsible for deallocating the DSM item chain.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  Errno Values
  ------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block
  DS_EOPNOTSUPP       invalid server address specified

SIDE EFFECTS
  The pointer to the DSM item chain is set to NULL.

===========================================================================*/
extern sint15 dss_write_dsm_chain
(
  sint15          sockfd,            /* socket descriptor                  */
  dsm_item_type **item_ptr,          /* DSM item chain containing the data */
  sint15         *dss_errno          /* error condition value              */
);


/*===========================================================================

FUNCTION DSS_READ_DSM_CHAIN

DESCRIPTION
  Reads a DSM item chain from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read.  A return of 0 indicates that an End-of-File
      condition has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  Errno Values
  ------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_read_dsm_chain
(
  sint15           sockfd,            /* socket descriptor                 */
  dsm_item_type  **item_ptr,          /* ptr to item chain containing data */
  sint15          *dss_errno          /* error condition value             */
);

/*===========================================================================
FUNCTION DSS_SET_SPECIFIED_IFACE()

DESCRIPTION
  This function is used to set the app specified interface for an
  application. For instance, MIP and GPS which uses only the 1X interface,
  use this API to set the specified interface in the acb.

DEPENDENCIES
  None.

PARAMETERS
  sint15 dss_nethandle - Application ID.
  dss_iface_id_type *iface_id - Parameter which contains the
  name and instance of the iface that application would like to use.
  sint15 *ds_errno - DS_EBADAPP if invalid dss_nethandle is specified.
                     DS_EBADF if invalid interface is specified.

RETURN VALUE
  0 on success.
  -1 on failure.

SIDE EFFECTS
  None.
===========================================================================*/
int dss_set_specified_iface
(
  sint15            dss_nethandle,
  dss_iface_id_type *iface_id,
  sint15            *ds_errno
);

/*===========================================================================
FUNCTION DSS_SENDTO_DSM_CHAIN()

DESCRIPTION
  Sends the specified DSM item chain over the TCP transport.  Lower layers
  will be responsible for deallocating the DSM item chain.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EOPNOTSUPP       invalid server address specified

SIDE EFFECTS
  The pointer to the DSM item chain is set to NULL.
===========================================================================*/
sint15 dss_sendto_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type **item_ptr,          /* DSM item chain containing the data */
  uint32 flags,                 /* used for SDB (if enabled)               */
  struct ps_sockaddr *toaddr,   /* destination address                     */
  uint16 addrlen,               /* address length                          */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================
FUNCTION DSS_RECVFROM_DSM_CHAIN()

DESCRIPTION
  Reads a DSM item chain from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read.  A return of 0 indicates that an End-of-File
      condition has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           can't read from a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_recvfrom_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type  **item_ptr,          /* ptr to item chain containing data */
  uint32 flags,                                                  /* unused */
  struct ps_sockaddr *fromaddr,                     /* destination address */
  uint16 *addrlen,                                       /* address length */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================
FUNCTION DSS_SET_ROUTEABLE_APP_OPT()

DESCRIPTION
  This function is used to set routeable options for an application.
  If this option is set, the application is allowed to access an iface which
  is bridged to another iface or only allows pkt forwarding. This option
  signifies that the applicaiton data can be routed even for such ifaces.

DEPENDENCIES
  None.

PARAMETERS
  sint15 dss_nethandle - Application ID.
  boolean opt  - Whenther to set or unset the option
  sint15 *ds_errno - DS_EBADAPP if invalid dss_nethandle is specified.

RETURN VALUE
  0 on success.
  -1 on failure.

SIDE EFFECTS
  None.
===========================================================================*/
int dss_set_routeable_app_opt
(
  sint15                  dss_nethandle,
  boolean                 opt,
  sint15                  *ds_errno
);


/*===========================================================================
FUNCTION DSS_REG_IP_FILTER()

DESCRIPTION
  This function registers IP filters for the specified socket and iface. The
  filters are validated and any error cases detected.

  If this function is called on a bound socket it verifies that the binding
  matches the passed filter.  It also requires that for tcp socket the
  filter be registered before a connect.

  Next header protocol must be specified. Otherwise all IP traffic will be
  snooped and sent up the stack starving laptop of all traffic.

  It is recommended that filter specifies Source address, Source port and
  Destination port to make the filter as narrow as possible. This way filter
  doesn't snoop in to the data destined for the laptop. Here destination
  is the mobile and source is the peer as sockets filters are inbound.

  Right now it is not enforced that Source address, Source port and
  Destination port are specified in the filter, but we need to revisit this
  API when it is externalized.

DEPENDENCIES
  None.

PARAMETERS
  sockfd      - Descriptor of the Socket requesting filtering
  iface_id    - Iface to add filters to
  filter_type - Type of filter
  filters     - Ptr to list of filters (total num_filters) where each
                filter is of type specified above -
                  => ip_filter_type for IP_FILTER_DEFAULT_TYPE
  num_filters - Number of filters passed in
  dss_errno   - Ptr to contain error code on the return

RETURN VALUE
  Num filters added successfully, or
  DSS_ERROR for error (sets dss_errno appropriately)

SIDE EFFECTS
  Adds socket client filters to the passed iface
===========================================================================*/
extern int dss_reg_ip_filter
(
  sint15                     sockfd,
  dss_iface_id_type          iface_id,
  ipfltr_type_enum_type      filter_type,
  uint8                      num_filters,
  void                       *filters,
  sint15                     *dss_errno
);

/*===========================================================================
FUNCTION DSS_REG_IP_FILTER_EX()

DESCRIPTION
  This function registers IP filters for the specified socket and iface. The
  filters are validated and any error cases detected.

  If this function is called on a bound socket it verifies that the binding
  matches the passed filter.  It also requires that for tcp socket the
  filter be registered before a connect.

  Next header protocol must be specified. Otherwise all IP traffic will be
  snooped and sent up the stack starving laptop of all traffic.

  It is recommended that filter specifies Source address, Source port and
  Destination port to make the filter as narrow as possible. This way filter
  doesn't snoop in to the data destined for the laptop. Here destination
  is the mobile and source is the peer as sockets filters are inbound.

  Right now it is not enforced that Source address, Source port and
  Destination port are specified in the filter, but we need to revisit this
  API when it is externalized.

DEPENDENCIES
  None.

PARAMETERS
  sockfd      - Descriptor of the Socket requesting filtering
  iface       - serves as either iface_id or iface_group
  filter_type - Type of filter
  filters     - Ptr to list of filters (total num_filters) where each
  filter is of type specified above -
  => ip_filter_type for IP_FILTER_DEFAULT_TYPE
  num_filters - Number of filters passed in
  filter_handle - handle for the filter registered
  dss_errno   - Ptr to contain error code on the return

RETURN VALUE
  Num filters added successfully, or
  DSS_ERROR for error (sets dss_errno appropriately)

SIDE EFFECTS
  Adds socket client filters to the passed iface
===========================================================================*/

extern int dss_reg_ip_filter_ex
(
  sint15                     sockfd,
  dss_iface_type             iface,
  ipfltr_type_enum_type      filter_type,
  uint8                      num_filters,
  void                       *filters,
  uint32                     *filter_handle,
  sint15                     *dss_errno
);


/*===========================================================================
FUNCTION DSS_DEREG_IP_FILTER()

DESCRIPTION
  This function deregisters all the IP filters previosuly added to an iface
  associated with the specified socket.

DEPENDENCIES
  None.

PARAMETERS
  sockfd     - Descriptor of the Socket requesting filtering
  iface_id   - Iface to remove filters from
  dss_errno  - Ptr to contain error code on the return

RETURN VALUE
  0 for success, DSS_ERROR for error (sets dss_errno appropriately)

SIDE EFFECTS
  Removes socket client filters from the passed iface
===========================================================================*/
extern int dss_dereg_ip_filter
(
  sint15             sockfd,
  dss_iface_id_type  iface_id,
  sint15            *dss_errno
);
/*===========================================================================
FUNCTION DSS_REG_IP_FILTER_BY_GROUP()

DESCRIPTION
  This function registers IP filters for the specified socket and iface. The
  filters are validated and any error cases detected.

  If this function is called on a bound socket it verifies that the binding
  matches the passed filter.  It also requires that for tcp socket the
  filter be registered before a connect.

  Next header protocol must be specified. Otherwise all IP traffic will be
  snooped and sent up the stack starving laptop of all traffic.

  It is recommended that filter specifies Source address, Source port and
  Destination port to make the filter as narrow as possible. This way filter
  doesn't snoop in to the data destined for the laptop. Here destination
  is the mobile and source is the peer as sockets filters are inbound.

  Right now it is not enforced that Source address, Source port and
  Destination port are specified in the filter, but we need to revisit this
  API when it is externalized.

DEPENDENCIES
  None.

PARAMETERS
  sockfd      - Descriptor of the Socket requesting filtering
  iface_id    - Iface to add filters to
  filter_type - Type of filter
  filters     - Ptr to list of filters (total num_filters) where each
                filter is of type specified above -
                  => ip_filter_type for IP_FILTER_DEFAULT_TYPE
  num_filters - Number of filters passed in
  dss_errno   - Ptr to contain error code on the return

RETURN VALUE
  Num filters added successfully, or
  DSS_ERROR for error (sets dss_errno appropriately)

SIDE EFFECTS
  Adds socket client filters to the passed iface
===========================================================================*/
extern int dss_reg_ip_filter_by_group
(
  sint15                      sockfd,
  dss_iface_name_enum_type   iface_group,
  ipfltr_type_enum_type      filter_type,
  uint8                      num_filters,
  void                       *filters,
  sint15                     *dss_errno
);


/*===========================================================================
FUNCTION DSS_DEREG_IP_FILTER_BY_GROUP()

DESCRIPTION
  This function deregisters all the IP filters previosuly added to an iface
  associated with the specified socket.

DEPENDENCIES
  None.

PARAMETERS
  sockfd     - Descriptor of the Socket requesting filtering
  iface_id   - Iface to remove filters from
  dss_errno  - Ptr to contain error code on the return

RETURN VALUE
  0 for success, DSS_ERROR for error (sets dss_errno appropriately)

SIDE EFFECTS
  Removes socket client filters from the passed iface
===========================================================================*/
extern int dss_dereg_ip_filter_by_group
(
  sint15                     sockfd, 
  sint15                  * dss_errno
);

/*===========================================================================
FUNCTION DSS_DEREG_IP_FILTER_BY_HANDLE()

DESCRIPTION
  This function deregisters a IP filter by the filter handle
  received via dss_reg_ip_filter_ex.

DEPENDENCIES
  None.

PARAMETERS
  filter_handle - The handle for the filter to be deregistered
  dss_errno  - Ptr to contain error code on the return

RETURN VALUE
  0 for success, DSS_ERROR for error (sets dss_errno appropriately)

SIDE EFFECTS
  Removes socket client filters from the passed iface
===========================================================================*/
extern int dss_dereg_ip_filter_by_handle
(
  uint32                    filter_handle,
  int16                   * dss_errno
);

#ifdef __cplusplus
}
#endif

#endif  /* DSSINTERNAL_H */
