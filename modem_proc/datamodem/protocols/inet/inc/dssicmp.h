#ifndef DSSICMP_NEW_H
#define DSSICMP_NEW_H
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

Copyright (c) 2001-2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/13/08    pp     Metainfo optimizations.
08/15/05    vp     Changed the signature of dssicmp_ps_input().
08/15/05    vp     Addition of flags argument to dssicmp_read()
06/11/04    vp     Included ps_in.h. Changes for representation of IP addresses
                   as struct ps_in_addr
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
#include "datamodem_variation.h"
#include "customer.h"
#include "ds_Sock_Def.h"
#include "ps_icmp.h"
#include "ps_in.h"
#include "ps_rt_meta_info.h"
#include "ps_pkt_meta_info.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_icmp6_msg.h"
#endif


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

  int16 *dss_errno -  error condition value

RETURN VALUE
  Errno Values
  ------------
  DS_EBADAPP          invalid application ID
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.
  DS_EPROTOTYPE       specified protocol invalid for socket type.


SIDE EFFECTS
  None.

===========================================================================*/
int16 dssicmp_int_socket
(
  uint16     family,
  void    *  scb_handle,
  int32      sockfd,
  int32   *  pcb_handle_ptr,
  void    *  reader_ptr,
  void    *  writer_ptr,
  void    *  errq_ptr,
  int16   *  errno_ptr
);

/*===========================================================================
FUNCTION DSSICMP_WRITE()

DESCRIPTION
  Send DSM item out to ICMP (write to ICMP socket). The actual write is
  performed in icmp_output() in icmp.c This function is the ICMP
  specific dss_sendto()

DEPENDENCIES
  None.

PARAMETERS
  protocol_ctl_block_type pcb       -   protocol control block
  struct ps_sockaddr_in   *ta_ptr   -   destination socket info
  dsm_item_type           *item_ptr -  ptr to head of dsm memory pool items

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dssicmp_write
(

  int32                     pcb_handle,
  int32                     sockfd,
  int32                     pkt_length,
  struct ps_sockaddr_in6  * ta_ptr,
  ps_pkt_meta_info_type   * pkt_meta_info_ptr
);

/*===========================================================================
FUNCTION DSSICMP_CLOSE()

DESCRIPTION
  This function is the ICMP specific dss_close() function.  It simply sets
  the socket state to DSSOCKI_NULL and frees up the socket control block and
  related data structures.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success; DSS_ERROR otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
void dssicmp_close
(
  int32    pcb_handle,
  int32    sockfd,
  boolean  silent_close_flag
);

/*===========================================================================
FUNCTION DSSICMP_SETOPT()

DESCRIPTION
  This function sets the option to the user specified value.

DEPENDENCIES
  None.

PARAMETERS
  tcb_ptr - pointer to tcb

RETURN VALUE
  NONE.

SIDE EFFECTS
  None.
===========================================================================*/
void dssicmp_setopt
(
  int32  pcb_handle,
  int32  sockfd,
  int32  opt_level,
  int32  opt_name,
  int32  val,
  int32  len
);

/*===========================================================================
FUNCTION DSSICMP_SET_ROUTE_META_INFO()

DESCRIPTION
  This function processes setting of routing meta info inside IPCB.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssicmp_set_route_meta_info
(
  int32                   pcb_handle,
  int32                   sockfd,
  ps_rt_meta_info_type *  rt_mi_ptr             /* Routing Meta info */
);

/*===========================================================================
FUNCTION DSSICMP_PS_INPUT()

DESCRIPTION
  When an ICMP message is arrived, if it is NOT ECHO, INFO_RQST or
  TIMESTAMP, the ICMP message will be handled by this fuction in the
  ps context.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success; DSS_ERROR otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
int16 dssicmp_ps_input
(
  struct icmp           *icmp_hdr,               /* ptr to the icmp header */
  dsm_item_type         *pkt_ptr,               /* ptr to the arriving pkt */
  ip_pkt_info_type      *ip_pkt_info_ptr        /* IP Packet Info          */
);

/*===========================================================================
FUNCTION DSSICMP6_PS_INPUT()

DESCRIPTION
  When an ICMP message is arrived, if it is NOT ECHO, INFO_RQST or
  TIMESTAMP, the ICMP message will be handled by this fuction in the
  ps context.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success; DSS_ERROR otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_DATA_PS_IPV6
int16 dssicmp6_ps_input
(
  ps_icmp6_msg_type     *icmp_hdr,               /* ptr to the icmp header */
  dsm_item_type         *pkt_ptr,               /* ptr to the arriving pkt */
  ip_pkt_info_type      *ip_pkt_info_ptr        /* IP Packet Info          */
);
#endif

#ifdef __cplusplus
}
#endif

#endif /* DSSICMP_NEW_H */
