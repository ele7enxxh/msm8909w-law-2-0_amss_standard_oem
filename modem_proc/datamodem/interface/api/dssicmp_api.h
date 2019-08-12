#ifndef DSSICMP_API_H
#define DSSICMP_API_H

/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  I C M P  F I L E

DESCRIPTION

 The Data Services sockets ICMP interface file. This contains all the
 functions used by the sockets library to access ICMP socket function calls.

EXTERNALIZED FUNCTIONS

  dssicmp_socket()
    This function creates an ICMP socket. NOTE: This function is cannot be
    mapped by the protocol specific function pointer. To create an ICMP
    socket, this function must be invoked directly (instead of through
    dss_socket()).

  dssicmp_write()
    Send DSM item out to ICMP (write to ICMP socket). The actual write is
    performed in icmp_output() in icmp.c This function is the ICMP
    specific dss_sendto()

  dssicmp_read()
    Reads 'nbytes' bytes into the buffer from the ICMP transport.  Fills in
    address structure with values from who sent the data in fromaddr.
    This function asserts that fromaddr is not NULL. This function is the
    ICMP specific dss_recvfrom()

  dssicmp_close()
    This function is the ICMP specific dss_close() function.  It simply sets
    the socket state to DSSOCKI_NULL and frees up the socket control block
    and related data structures.

  dssicmp_ps_input ()
    When an ICMP message is arrived, if it is NOT ECHO, INFO_RQST or
    TIMESTAMP, the ICMP message will be handled by this fuction in the
    ps context.

 dssicmp_setopt()
    This function sets the option value to the user specified value.

 dssicmp_set_route_meta_info()
    This function sets Routing Meta info inside IPCB.

Copyright (c) 2001-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/api/dssicmp_api.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/15/09    pp     CMI Phase 4: SU Level API Effort.
09/12/08    pp     Metainfo optimizations.
08/15/05    vp     Changed the signature of dssicmp_ps_input().
08/15/05    vp     Addition of flags argument to dssicmp_read()
06/11/04    vp     Included ps_in.h. Changes for representation of IP addresses
                   as struct in_addr
03/30/04    vp     Merged changes from June04Dev_05.00.00 ps branch.
03/10/04    vp     Added declaration for dssicmp_register_upcall()
01/21/04    sv     Changed icmp_socket to take sock_cb and netpolicy
                   instead of appid.
10/17/03    sv     Modified protocol specific function to take protocol control
                   blocks as argument.
08/18/03    sv     Modified code to use V4mappedV6 addresses.
08/08/03    ss/sv  Restructured the socket code.
04/15/03    sv     Modified dssicmp_read to accept iov and iovcount as
                   arguments instead of buffer and bufferlen.
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
05/02/02    mvl    File clean up, added meta_info_ptr to write()
03/27/01    mt     Changed instances of errno to dss_errno.  Changed second
                   parameter type of dssicmp_read() from byte * to void *.
03/21/01    js     Created module.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "comdef.h"
#include "dssocket.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
===========================================================================*/
typedef enum
{
  DSSICMP_ICMP_V4  = 0,
  DSSICMP_ICMP_V6  = 1
} dssicmp_icmp_ver_type;


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
FUNCTION DSSICMP_SOCKET()

DESCRIPTION
  This function creates an ICMP socket. NOTE: This function is cannot be
  mapped by the protocol specific function pointer. To create an ICMP
  socket, this function must be invoked directly (instead of through
  dss_socket()).

DEPENDENCIES
  None.

PARAMETERS
  uint8  type       -  ICMP socket type
  uint8  code       -  ICMP socket code
  dss_sock_cb_fcn sock_cb - socket callback function
  void * sock_cb_user_data - User data for socket call back
  dss_net_policy_info_type * policy_info_ptr-  Network policy info
  sint15 *dss_errno -  error condition value

RETURN VALUE
  dss_errno Values
  ----------------
  DS_EBADAPP          invalid application ID
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.
  DS_EPROTOTYPE       specified protocol invalid for socket type.


SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dssicmp_socket
(
  dssicmp_icmp_ver_type  icmp_ver,
  uint8 type,
  uint8 code,
  dss_sock_cb_fcn sock_cb,                     /* socket callback function */
  void * sock_cb_user_data,              /* User data for socket call back */
  dss_net_policy_info_type * policy_info_ptr,       /* Network policy info */
  sint15 *dss_errno                               /* error condition value */
);

#ifdef __cplusplus
}
#endif
#endif /* DSSICMP_API_H */
