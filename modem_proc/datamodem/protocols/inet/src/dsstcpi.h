#ifndef DSSTCPI_H
#define DSSTCPI_H

/*===========================================================================

     D A T A  S E R V I C E S  S O C K E T  T C P  H E A D E R  F I L E

DESCRIPTION

 The Data Services sockets TCP interface file. This contains all the
 functions used by the sockets library to access TCP socket function calls.

EXTERNALIZED FUNCTIONS

  dsstcp_socket()
    TCP specific socket() function.  Sets socket state to INIT.

  dsstcp_connect()
    Connects a TCP socket.

  dsstcp_listen()
    Does the passive open for listener socket child sockets.

  dsstcp_accept()
    Removes and return the a child socket from a listener sockets
    ready queue. This will also try to refill the half open
    connections queue.

  dsstcp_write()
    TCP specific write() function.  Sends the dsm memory item out
    the TCP transport.

  dsstcp_close()
    TCP specific close() function.  Initiates the TCP active close.
    If the connection is already closed, cleans up the TCP socket and
    associated resources.

  dsstcp_bind()
    This is the TCP specific bind() function. It writes the port value into
    local.conn.port of the socket control block structure.

  dsstcp_setopt
    This function sets the tcp option value to the user specified value.

  dsstcp_flow_enabled_ind
    This function calls tcp_output() to send data out once socket is
    flow enabled

  dsstcp_lookup_conn()
    Given a connection structure, looks up the connection to determine
    if this is a valid connection.

  dsstcp_lookup_serv()
    Given a connection structure, looks up to determine if there are
    any valid listener sockets(server socket)

  dsstcp_new_ip_upcall()
    If a new IP address is negotiated with the Base Station, then send a
    reset to the peer, and then close down sockets and notify the user.


Copyright (c) 1998-2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsstcp.h_v   1.2   13 Feb 2003 12:21:28   ssinghai  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dsstcpi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/13/06   msr     Added APP_TO_PS_SOCKET_FLOW_ENABLED_IND
07/31/06   rt      Route Scope modifications.
03/27/06   rt      Added a new function dsstcp_window_reopen.
08/15/05   vp      Addition of flags argument to dsstcp_read().
04/18/05   vp      Change in signature of bind and listen functions.
08/16/04   ks/msr  Removed dsstcp_init_tcb_array function.
08/30/04   sv      Moved graceful dormant close flag and keep alive idle time
                   constants to socket config control block.
04/14/04   vp      Added declaration for tcp_keepalive_idle_time.
03/30/04   vp      Merged changes from June04Dev_05.00.00 PS branch.
01/27/04   vp      Added declaration of tcp_graceful_dormant_close flag
11/11/03   sv      Modified is_writeable to take scb_ptr instead of pcb.
10/17/03   sv      Modified protocol specific functions to take PCB instead
                   of SCB. Removed dsstcp_abort as it is no longer used.
09/09/03   sv      Added dsstcp_reset function to reset tcb.
08/18/03   sv      Modified the code to use IPv6 addresses internally.
08/08/03   ss/sv   Restructured the socket code.
06/27/03   sv      Modified dsstcp_is_writeable to return integer.
04/15/03   sv      Modified dsstcp_read to accept iov and iovcount as
                   arguments instead of buffer and bufferlen.
02/13/03   ss      Added dsstcp_shutdown()
12/22/02   aku     Changed dsstcp_new_ip_upcall() signature to take
                   ps_iface_ptr as argument instead of IP addresses.
05/02/02   mvl     File clean up, added meta_info_ptr to write() prototype
02/22/02   pjb     Added accept and listen
02/12/02   ss      Added prototype for dsstcp_cleanup_all_reseq().
02/11/02   ss      Added prototype for dsstcp_is_writeable().
10/11/01   mt      Add dss_errno argument to dsstcp_connect_setup().
03/27/01   mt      Changed type of second argument in dsstcp_read() from
                   byte * to void *.
03/27/01   js      Added extra parameter to function dsstcp_write(). The
                   extra parameter holds a pointer to the structure holding
                   the destination information. Though not used, the addition
                   is necessary to comply with the dss_fcn_ptr_table format.
02/08/01   yll     Added dsstcp_read_dsm_chain().
11/10/00   snn     Changed scb_type to scb_s
10/10/00   na      Removed tcp_ptr which is not longer used. Added prototype
                   for dsstcp_init_tcb_array().
08/03/00   mvl/js  Added prototype for dsstcp_bind() function.
03/22/00   rc/na   Added prototype for function dsstcp_new_ip_upcall().
09/22/99   rc      Incorporated code review changes and comments.
08/11/99   rc/ak   Extern for tcp abort function.
12/14/98   na/hcg  Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ps_tcp.h"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSTCPI_CREATE_TCB()

DESCRIPTION
  This function creates a new TCP control block.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to TCP control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
struct tcb *dsstcpi_create_tcb
(
  void  * scb_handle,
  int32   sockfd
);

/*===========================================================================
FUNCTION DSSTCPI_FREE_TCB()

DESCRIPTION
  This function frees TCP control block, making it available for re-use.

  This function executes in the context of the PS task.

ARGUMENTS:
  tcb_ptr: This is a pointer to the TCP control block that is being freed.
  This argument must point to a valid TCP control block and cannot be NULL.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the protocol control block of the scb_ptr to NULL.
===========================================================================*/
void dsstcpi_free_tcb
(
  struct tcb* tcb_ptr                            /* Ptr to TCB to be freed */
);

#endif /* DSSTCPI_H */
