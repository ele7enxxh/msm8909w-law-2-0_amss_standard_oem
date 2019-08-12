/*===========================================================================

                       P S _ D N S I _ I O _ M G R _ C

DESCRIPTION

  The Data Services DNS Subsystem IO manager module. Contains
  declarations of functions, variables, macros, structs and enums
  used by IO manager. This module is internal to the DNS subsystem.

EXTERNALISED FUNCTIONS

  ps_dnsi_io_mgr_create_instance()
    Creates an instance of the IO manager.

  ps_dnsi_io_mgr_send_query()
    Sends query to the DNS server in a specified instance, waits for the
    reply and notifies client after receiving the reply.

  ps_dnsi_io_mgr_delete_instance()
    Deletes an instance of IO manager. Closes corresponding socket and
    frees the IO manager control block.

INTIALIZATIONS AND SEQUENCING REQUIREMENTS
  This module should be run only in PS task context.

Copyright (c) 2007-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                             EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Note that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssvc/src/ps_dnsi_io_mgr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/24/11   ea      Change ps_dnsi_io_mgri_sock_send() to return a value.
04/21/11   ea      Cancel the notification to the resolver on CLOSE event.
10/14/10   ea      Add a flag that indicates if filter deregistration is needed
03/02/08   hm      Fixes for DNS cache poisoning.
12/01/08   hm      Getting iface id using get_iface_id_by_policy
08/29/08   am      Fixed low WM counts for Low-tier build.
06/11/08   hm      Supporting net policy as session config param
12/29/07   hm      Support for DNS lookups during laptop calls.
08/06/07   vp      Use Simple IP iface when doing MIP CCoA
05/19/07   hm      Posting command for processing socket events
03/26/07   hm      Initial development work.

===========================================================================*/


/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>
#include "dssdns.h"
#include "ps_dnsi_io_mgr.h"
#include "dssinternal.h"
#include "dss_netpolicy.h"
#include "ps_handle_mgr.h"
#include "ps_mem.h"
#include "ps_svc.h"
#include "dssinternal.h"
#include "ds_Utils_DebugMsg.h"
#include "dss_dns_config.h"
#include "ps_utils.h"


/*===========================================================================

                              LOCAL DATA DEFINITIONS

===========================================================================*/
#define SOCKADDR_IN_SIZE   (sizeof (struct ps_sockaddr_in))
#define SOCKADDR_IN6_SIZE  (sizeof (struct ps_sockaddr_in6))
#define IO_MGR_CLIENT      (PS_HANDLE_MGR_CLIENT_DNS_IO_MGR)

/*---------------------------------------------------------------------------
  Tuning the number of ps dnsi io manager buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_DNSI_IO_MGR_BUF_SIZE  ((sizeof(ps_dnsi_io_mgr_cb_type) + 3) & ~3)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

#define PS_DNSI_IO_MGR_BUF_NUM      DSS_DNS_MAX_QUERIES
#define PS_DNSI_IO_MGR_BUF_HIGH_WM  ((PS_DNSI_IO_MGR_BUF_NUM) - 1)
#define PS_DNSI_IO_MGR_BUF_LOW_WM   1

#else

#define PS_DNSI_IO_MGR_BUF_NUM      DSS_DNS_MAX_QUERIES
#define PS_DNSI_IO_MGR_BUF_HIGH_WM  ((PS_DNSI_IO_MGR_BUF_NUM) - 1)
#define PS_DNSI_IO_MGR_BUF_LOW_WM   1
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_dnsi_io_mgr along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_dnsi_io_mgr_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                  (
                                    PS_DNSI_IO_MGR_BUF_NUM,
                                    PS_DNSI_IO_MGR_BUF_SIZE
                                  )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to ps_dnsi_io_mgr_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type     * ps_dnsi_io_mgr_buf_hdr[PS_DNSI_IO_MGR_BUF_NUM];
static ps_dnsi_io_mgr_cb_type  * ps_dnsi_io_mgr_buf_ptr[PS_DNSI_IO_MGR_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */



/*===========================================================================

                         INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_DNSI_IO_MGRI_IS_HANDLE_VALID

DESCRIPTION
  Checks if the specified handle is valid.

PARAMETERS
  io_mgr_handle  - IO mgr control block handle

RETURN VALUE
  TRUE if the specified handle is valid.
  FALSE if the specified handle is not valid.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
ps_dnsi_io_mgr_cb_type * ps_dnsi_io_mgri_is_handle_valid
(
  ps_dnsi_io_mgr_handle_type  io_mgr_handle
)
{
  ps_dnsi_io_mgr_cb_type      * io_mgr_cb_ptr;
  int16                         buf_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( PS_DNSI_IO_MGR_INVALID_HANDLE == io_mgr_handle )
  {
    LOG_MSG_ERROR_1("Invalid io handle %d", io_mgr_handle);
    return NULL;
  }

  buf_index = ps_handle_mgr_get_index( IO_MGR_CLIENT,
                                       io_mgr_handle );
  if( PS_HANDLE_MGR_INVALID_INDEX == buf_index )
  {
    LOG_MSG_ERROR_1("Invalid index for handle %d", buf_index);
    return NULL;
  }

  io_mgr_cb_ptr =
    (ps_dnsi_io_mgr_cb_type *)
      ps_mem_index_to_buf( buf_index, PS_MEM_DNSI_IO_MGR_CB_TYPE );

  if( NULL == io_mgr_cb_ptr ||
      io_mgr_cb_ptr->io_mgr_handle != io_mgr_handle )
  {
    LOG_MSG_ERROR_2("Inv io CB 0x%p from handle %d",
                    io_mgr_cb_ptr, io_mgr_handle);
    return NULL;
  }

  return io_mgr_cb_ptr;

} /* ps_dnsi_io_mgri_is_handle_valid() */


/*===========================================================================
FUNCTION PS_DNSI_IO_MGRI_SOCK_CLOSE()

DESCRIPTION
  Closes the socket, handles EWOULDBLOCKS, and cleans up IO manager
  control block.

PARAMETERS
  io_mgr_cb_ptr - ptr to IO control block

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Frees the IO Mgr CB if socket is closed successfully
===========================================================================*/
static void ps_dnsi_io_mgri_sock_close
(
  ps_dnsi_io_mgr_cb_type  * io_mgr_cb_ptr
)
{
  int16  ps_errno = DSS_SUCCESS;
  int16  retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == io_mgr_cb_ptr )
  {
    LOG_MSG_ERROR_0("NULL IO Mgr CB");
    return;
  }

  /*-------------------------------------------------------------------------
    Close the socket.
  -------------------------------------------------------------------------*/
  retval = dss_close( io_mgr_cb_ptr->sockfd, &ps_errno );
  if( DSS_ERROR == retval )
  {
    if( DS_EWOULDBLOCK == ps_errno )
    {
      (void) dss_async_select( io_mgr_cb_ptr->sockfd, DS_CLOSE_EVENT, &ps_errno );
    }
    else
    {
      LOG_MSG_ERROR_2( "dss_close returned error: %d, sockfd=%d",
                       ps_errno, io_mgr_cb_ptr->sockfd );
    }
  }
  else if( DSS_SUCCESS == retval )
  {
    /*-----------------------------------------------------------------------
      Clean up the IO manager control block.
    -----------------------------------------------------------------------*/
    if( NULL != io_mgr_cb_ptr->dns_query_ptr )
    {
      dsm_free_packet( &(io_mgr_cb_ptr->dns_query_ptr) );
    }
    memset( io_mgr_cb_ptr, 0, sizeof( ps_dnsi_io_mgr_cb_type ) );
    io_mgr_cb_ptr->io_mgr_handle = PS_DNSI_IO_MGR_INVALID_HANDLE;
    PS_MEM_FREE(io_mgr_cb_ptr);
  }

  return;
} /* ps_dnsi_io_mgri_sock_close() */


/*===========================================================================
FUNCTION PS_DNSI_IO_MGRI_ERROR_HANDLER()

DESCRIPTION
  Prints the error message, notifies the client of the error,
  and calls to close IO manager.

PARAMETERS
  io_mgr_cb_ptr - ptr to IO control block
  errno         - error passed back on error

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_dnsi_io_mgri_error_handler
(
  ps_dnsi_io_mgr_cb_type  * io_mgr_cb_ptr,
  int16                     ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == io_mgr_cb_ptr )
  {
    DATA_ERR_FATAL("NULL IO Mgr CB");
    return;
  }

  /*-------------------------------------------------------------------------
    Notify the client of the error occurred.
  -------------------------------------------------------------------------*/
  dsm_free_packet( &(io_mgr_cb_ptr->dns_query_ptr) );
  io_mgr_cb_ptr->cback_f_ptr( io_mgr_cb_ptr->io_mgr_handle,
                              PS_DNSI_IO_MGR_ERROR_EVENT,
                              NULL,
                              io_mgr_cb_ptr->user_data_ptr,
                              ps_errno );

  return;

} /* ps_dnsi_io_mgri_error_handler() */


/*===========================================================================
FUNCTION  PS_DNSI_IO_MGRI_SOCK_SEND()

DESCRIPTION
  This function is a wrapper around dss_sendto_dsm_chain. It takes care of
  error conditions and registers for WRITE events on EWOULDBLOCK.

PARAMETERS
  io_mgr_cb_ptr - ptr to IO control block
  ps_errno      - error code returned in case of error

RETURN VALUE
  DSS_SUCCESS  Operation succeeded.
  DSS_ERROR    Operation failed.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int16 ps_dnsi_io_mgri_sock_send
(
  ps_dnsi_io_mgr_cb_type  * io_mgr_cb_ptr,
  int16                   * ps_errno
)
{
  int16                     retval;
  int16                     addrlen = 0;
  int16                     copylen = 0;
  struct ps_sockaddr      * temp_sockaddr = NULL;
  struct ps_sockaddr_in     temp_sockaddr_in;
  struct ps_sockaddr_in6    temp_sockaddr_in6;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == io_mgr_cb_ptr)
  {
    LOG_MSG_ERROR_1("Invalid IO Mgr CB 0x%p", io_mgr_cb_ptr);
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if (NULL == io_mgr_cb_ptr->dns_query_ptr)
  {
    LOG_MSG_ERROR_0 ("DNS query NULL in IO control blk");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  temp_sockaddr  = (struct ps_sockaddr *) &(io_mgr_cb_ptr->server_sockaddr);
  if( DSS_AF_INET == temp_sockaddr->ps_sa_family )
  {
    addrlen = sizeof(struct ps_sockaddr_in);
    memscpy(&temp_sockaddr_in, sizeof(struct ps_sockaddr_in),
      temp_sockaddr, sizeof(struct ps_sockaddr_in));
    LOG_MSG_INFO2_1("Sending DNS query to V4 server at port %d",
                    dss_ntohs( temp_sockaddr_in.ps_sin_port ));
  }
  else if( DSS_AF_INET6 == temp_sockaddr->ps_sa_family )
  {
    addrlen = sizeof(struct ps_sockaddr_in6);
    copylen = MIN((uint16)addrlen, sizeof(struct ps_sockaddr)); // to avoid KW error
    memscpy(&temp_sockaddr_in6, copylen, temp_sockaddr, copylen);
    LOG_MSG_INFO2_1("Sending DNS query to V6 server at port %d",
                    dss_ntohs( temp_sockaddr_in6.ps_sin6_port ));
  }
  else
  {
    LOG_MSG_ERROR_2("Invalid address family %d for IO mgr 0x%p",
                    temp_sockaddr->ps_sa_family, io_mgr_cb_ptr);
    *ps_errno = DS_EAFNOSUPPORT;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Send the message to the DNS server over UDP socket.
  -------------------------------------------------------------------------*/
  retval =
    dss_sendto_dsm_chain ( io_mgr_cb_ptr->sockfd,
                           &(io_mgr_cb_ptr->dns_query_ptr),
                           0,
                           temp_sockaddr,
                           addrlen,
                           ps_errno );

  /*-------------------------------------------------------------------------
    Be careful about if-else here as we are reusing errno.
  -------------------------------------------------------------------------*/
  if( DSS_ERROR == retval )
  {
    if( DS_EWOULDBLOCK == *ps_errno )
    {
      (void) dss_async_select(io_mgr_cb_ptr->sockfd, DS_WRITE_EVENT, ps_errno);
    }
    else
    {
      /*---------------------------------------------------------------------
        Handle error condition.
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_2("IO mgr sock sendto error=%d, sock=%d",
                      *ps_errno, io_mgr_cb_ptr->sockfd);
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Register for READ event on the socket and set the DSM item ptr to NULL.
    -----------------------------------------------------------------------*/
    (void) dss_async_select(io_mgr_cb_ptr->sockfd, DS_READ_EVENT, ps_errno);
    io_mgr_cb_ptr->dns_query_ptr = NULL;
    retval = DSS_SUCCESS;
  }

  return retval;
} /* ps_dnsi_io_mgri_sock_send() */

/*===========================================================================
FUNCTION  PS_DNSI_IO_MGRI_SOCK_RECV()

DESCRIPTION
  This function is a wrapper around dss_recvfrom_dsm_chain. Takes care of
  error conditions and handles EWOULDBLOCK.

PARAMETERS
  io_mgr_cb_ptr - ptr to IO control block

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_dnsi_io_mgri_sock_recv
(
  ps_dnsi_io_mgr_cb_type  * io_mgr_cb_ptr
)
{
  dsm_item_type              * dns_response_ptr = NULL;
  int16                        ps_errno;
  int16                        retval;
  uint16                       addrlen = 0;
  struct ps_sockaddr_storage   remote_sockaddr;
  uint32                          i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == io_mgr_cb_ptr )
  {
    DATA_ERR_FATAL("NULL IO Mgr CB");
    return;
  }

  if( DSS_AF_INET == io_mgr_cb_ptr->server_sockaddr.ps_ss_family )
  {
    addrlen = sizeof(struct ps_sockaddr_in);
  }
  else if ( DSS_AF_INET6 == io_mgr_cb_ptr->server_sockaddr.ps_ss_family )
  {
    addrlen = sizeof(struct ps_sockaddr_in6);
  }
  else
  {
    LOG_MSG_ERROR_1("Invalid address family %d",
                    io_mgr_cb_ptr->server_sockaddr.ps_ss_family);
    ASSERT(0);
    return;
  }

  retval =
    dss_recvfrom_dsm_chain( io_mgr_cb_ptr->sockfd,
                            &dns_response_ptr,
                            0,
                            (struct ps_sockaddr *)&remote_sockaddr,
                            &addrlen,
                            &ps_errno );

  if( DSS_ERROR == retval )
  {
    LOG_MSG_ERROR_2("Failed to read, sock=%d, error=%d",
                    io_mgr_cb_ptr->sockfd, ps_errno);
    //ASSERT(0); //MSR will there be a case where READ_EV is posted but there
    // is no data to read??? In general possible, but in this case may be not
    dsm_free_packet( &dns_response_ptr );
    ps_dnsi_io_mgri_error_handler( io_mgr_cb_ptr, ps_errno );
    return;
  }

  /*-------------------------------------------------------------------------
    Check if the response is received from valid server
  -------------------------------------------------------------------------*/
  for (i = 0 ; i < io_mgr_cb_ptr->max_pending_servers_index ; i++)
  {

    if (io_mgr_cb_ptr->p_pending_servers[i].ps_ss_family !=
        remote_sockaddr.ps_ss_family)
    {
      continue;
    }

    if (DSS_AF_INET == io_mgr_cb_ptr->p_pending_servers[i].ps_ss_family)
    {
      if
      (
       ((struct ps_sockaddr_in*)&io_mgr_cb_ptr->p_pending_servers[i])->ps_sin_port !=
       ((struct ps_sockaddr_in*)&remote_sockaddr)->ps_sin_port  ||
       ((struct ps_sockaddr_in*)&io_mgr_cb_ptr->p_pending_servers[i])->ps_sin_addr.ps_s_addr !=
       ((struct ps_sockaddr_in*)&remote_sockaddr)->ps_sin_addr.ps_s_addr
      )
      {
        continue;
      }
    }
    else if (DSS_AF_INET6 == io_mgr_cb_ptr->p_pending_servers[i].ps_ss_family)
    {
      if(
          ((struct ps_sockaddr_in6*)&io_mgr_cb_ptr->p_pending_servers[i])->ps_sin6_port !=
          ((struct ps_sockaddr_in6*)&remote_sockaddr)->ps_sin6_port  ||
          !PS_IN6_ARE_ADDR_EQUAL
          (
           (&((struct ps_sockaddr_in6*)&io_mgr_cb_ptr->p_pending_servers[i])->ps_sin6_addr.in6_u.u6_addr32),
           (&((struct ps_sockaddr_in6*)&remote_sockaddr)->ps_sin6_addr.in6_u.u6_addr32)
          )
        )
      {
        continue;
      }
    }
    break;
  }

  if (i == io_mgr_cb_ptr->max_pending_servers_index)
  {
    LOG_MSG_ERROR_0 ("Inv. server addr/port");
    (void)dss_async_select(io_mgr_cb_ptr->sockfd, DS_READ_EVENT, &ps_errno);
    dsm_free_packet (&dns_response_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Delete filters, if any are installed.
  -------------------------------------------------------------------------*/
  if (io_mgr_cb_ptr->is_registered_to_filters)
  {
    if (TRUE == io_mgr_cb_ptr->net_policy.is_routeable &&
        DSS_IFACE_INVALID_ID != io_mgr_cb_ptr->iface_id)
    {
      retval = dss_dereg_ip_filter( io_mgr_cb_ptr->sockfd,
                                    io_mgr_cb_ptr->iface_id,
                                    &ps_errno );
      if(DSS_ERROR == retval)
      {
        LOG_MSG_ERROR_2("Error %d deregistering filters on sock %d",
                        ps_errno, io_mgr_cb_ptr->sockfd);
      }
      else
      {
        LOG_MSG_INFO1_2("Deregistered filters installed on sock %d, iface 0x%x",
                        io_mgr_cb_ptr->sockfd, io_mgr_cb_ptr->iface_id);
        io_mgr_cb_ptr->is_registered_to_filters = FALSE;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Notify client, delete the query message stored earlier.
  -------------------------------------------------------------------------*/
  dsm_free_packet( &(io_mgr_cb_ptr->dns_query_ptr) );
  io_mgr_cb_ptr->cback_f_ptr ( io_mgr_cb_ptr->io_mgr_handle,
                               PS_DNSI_IO_MGR_READ_EVENT,
                               dns_response_ptr,
                               io_mgr_cb_ptr->user_data_ptr,
                               ps_errno );

  dsm_free_packet( &dns_response_ptr );
  return;

} /* ps_dnsi_io_mgri_sock_recv() */


/*===========================================================================
FUNCTION PS_DNSI_IO_MGRI_CMD_HANDLER()

DESCRIPTION
  This function is called when the sockets layer notifies the IO manager
  of events of interest.

PARAMETERS
  ps_cmd        - Should be PS_DNS_IO_MGR_SOCK_EVENT_CMD
  io_data_ptr   - Points to the IO manager control block.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_dnsi_io_mgri_cmd_handler
(
  ps_cmd_enum_type              ps_cmd,
  void                        * io_data_ptr
)
{
  ps_dnsi_io_mgr_cb_type      * io_mgr_cb_ptr;
  ps_dnsi_io_mgr_handle_type    io_mgr_handle;
  int16                         retval = DSS_SUCCESS;
  int16                         ps_errno = DSS_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == io_data_ptr || PS_DNS_IO_MGR_SOCK_EVENT_CMD != ps_cmd )
  {
    LOG_MSG_ERROR_0("ps_dnsi_io_mgri_cmd_handler(): wrong parameters");
    return;
  }

  io_mgr_handle = (ps_dnsi_io_mgr_handle_type) io_data_ptr;

  LOG_MSG_INFO2_1("Processing IO_MGR_SOCK_EVENT_CMD for %d", io_mgr_handle);

  io_mgr_cb_ptr = ps_dnsi_io_mgri_is_handle_valid( io_mgr_handle );

  if( NULL ==  io_mgr_cb_ptr )
  {
    LOG_MSG_ERROR_1("Invalid handle %d", io_mgr_handle);
    return;
  }

  if (0 != (DS_WRITE_EVENT & io_mgr_cb_ptr->sock_event_mask))
  {
    retval = ps_dnsi_io_mgri_sock_send( io_mgr_cb_ptr, &ps_errno );
    if( DSS_ERROR == retval && DS_EWOULDBLOCK != ps_errno )
    {
      ps_dnsi_io_mgri_error_handler( io_mgr_cb_ptr, ps_errno );
    }
  }

  // check if the io_mgr is still valid after ps_dnsi_io_mgri_sock_send()
  if( NULL != ps_dnsi_io_mgri_is_handle_valid( io_mgr_handle ) )
  {
    if (0 != (DS_READ_EVENT & io_mgr_cb_ptr->sock_event_mask))
    {
      ps_dnsi_io_mgri_sock_recv( io_mgr_cb_ptr );
    }
  }

  // check if the io_mgr is still valid after ps_dnsi_io_mgri_sock_recv()
  if( NULL != ps_dnsi_io_mgri_is_handle_valid( io_mgr_handle ) )
  {
    if (0 != (DS_CLOSE_EVENT & io_mgr_cb_ptr->sock_event_mask))
    {
      ps_dnsi_io_mgri_sock_close( io_mgr_cb_ptr );
    }
  }

  return;
} /* ps_dnsi_io_mgri_cmd_handler() */


/*===========================================================================
FUNCTION PS_DNSI_IO_MGRI_SOCK_EVENT_CBACK()

DESCRIPTION
  This function is called by the sockets layer to notify IO manager of
  socket events of interest.
  This functions posts a command to PS to do further processing. This is
  required because socket events are notified under task locks.

PARAMETERS
  nethandle     -  Network handle
  sockfd        -  Socket descriptor on which the event occurred
  event         -  Events occurred on sockfd
  user_data_ptr -  user data (IO manager pointer)

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_dnsi_io_mgri_sock_event_cback
(
  int16     nethandle,
  int16     sockfd,
  uint32    sock_event_mask,
  void    * user_data_ptr
)
{
  ps_dnsi_io_mgr_cb_type  * io_mgr_cb_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3("Socket event %d on socket %d occurred for IO mgr 0x%p",
                  sock_event_mask, sockfd, user_data_ptr);

  if( NULL == user_data_ptr )
  {
    DATA_ERR_FATAL("Null user data");
    return;
  }

  io_mgr_cb_ptr = (ps_dnsi_io_mgr_cb_type *) user_data_ptr;
  if( io_mgr_cb_ptr->sockfd == 0 )
  {
    LOG_MSG_ERROR_1("Invalid IO Mgr CB 0x%p", io_mgr_cb_ptr);
    return;
  }

  LOG_MSG_INFO2_2("Rcvd ev %d on sock %d", sock_event_mask, sockfd);

  io_mgr_cb_ptr->sock_event_mask = sock_event_mask;

  /*------------------------------------------------------------------------
    Passing the IO manager handle to make sure that we don’t end up
    accessing the control block if the memory associated with the handle
    is deleted.
  -------------------------------------------------------------------------*/

  ps_send_cmd( PS_DNS_IO_MGR_SOCK_EVENT_CMD,
               (void *)io_mgr_cb_ptr->io_mgr_handle );

  return;
} /* ps_dnsi_io_mgri_sock_event_cback() */


/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION  PS_DNSI_IO_MGR_INIT()

DESCRIPTION
  Initialize the DNS IO manager during powerup.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_dnsi_io_mgr_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize Pool
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DNSI_IO_MGR_CB_TYPE,
                           ps_dnsi_io_mgr_buf_mem,
                           PS_DNSI_IO_MGR_BUF_SIZE,
                           PS_DNSI_IO_MGR_BUF_NUM,
                           PS_DNSI_IO_MGR_BUF_HIGH_WM,
                           PS_DNSI_IO_MGR_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_dnsi_io_mgr_buf_hdr,
                           (int *) ps_dnsi_io_mgr_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("Can't init the module");
  }

  (void) ps_set_cmd_handler (PS_DNS_IO_MGR_SOCK_EVENT_CMD,
                             ps_dnsi_io_mgri_cmd_handler );

} /* ps_dnsi_io_mgr_init() */


/*===========================================================================
FUNCTION  PS_DNSI_IO_MGR_CREATE_INSTANCE()

DESCRIPTION
  Creates an instance of the IO manager. Allocates and fills the IO manager
  control block and creates socket to be used to send messages to DNS server.

PARAMETERS
  family          - Address family (DSS_AF_INET/DSS_AF_INET6) to be 
                    used for this IO manager.
  net_policy_ptr  - Network policy to be used for this IO manager.
  cback_f_ptr     - Client callback function.
  user_data_ptr   - Callback user data (Optional - Can be null)
  errno           - Error code returned in case of error.

RETURN VALUE
  On success, returns a handle to the IO manager control block.
  On Failure, returns PS_DNSI_IO_MGR_INVALID_HANDLE and sets the error
  code in errno.

  errno values
  ------------
  DS_ENOMEM       - No memory for allocating control block.
  DS_SOCKERROR    - Socket subsystem error.
  DS_EBADF        - Bad descriptor

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_dnsi_io_mgr_handle_type  ps_dnsi_io_mgr_create_instance
(
  uint16                              family,
  dss_net_policy_info_type          * net_policy_ptr,
  ps_dnsi_io_mgr_notify_f_type        cback_f_ptr,
  void                              * user_data_ptr,
  int16                             * ps_errno
)
{
  ps_dnsi_io_mgr_cb_type            * io_mgr_cb_ptr = NULL;
  int16                               buf_index;
  int16                               retval = PS_DNSI_IO_MGR_INVALID_HANDLE;
  struct ps_sockaddr_storage          this_sockaddr;
  uint16                              this_addrlen;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == ps_errno )
  {
    LOG_MSG_ERROR_0("NULL errno");
    return retval;
  }

  if( NULL == cback_f_ptr || NULL == user_data_ptr || NULL == net_policy_ptr)
  {
    LOG_MSG_ERROR_0("NULL args");
    *ps_errno = DS_EFAULT;
    return retval;
  }

  if( DSS_AF_INET != family && DSS_AF_INET6 != family)
  {
     LOG_MSG_ERROR_1("Wrong family passed: %d", family);
     *ps_errno = DS_EFAULT;
     return retval;
  }

  LOG_MSG_INFO2_2("Create instance called with cback 0x%p, user data 0x%p",
                  cback_f_ptr, user_data_ptr);

  /*------------------------------------------------------------------------
    Allocate and fill IO manager control block
  -------------------------------------------------------------------------*/
  io_mgr_cb_ptr =
    (ps_dnsi_io_mgr_cb_type *) ps_mem_get_buf( PS_MEM_DNSI_IO_MGR_CB_TYPE );
  if( NULL == io_mgr_cb_ptr )
  {
    *ps_errno = DS_ENOMEM;
    return retval;
  }

  memset( io_mgr_cb_ptr, 0, sizeof(ps_dnsi_io_mgr_cb_type) );

  buf_index = ps_mem_buf_to_index((void *) io_mgr_cb_ptr);
  io_mgr_cb_ptr->io_mgr_handle =
    (ps_dnsi_io_mgr_handle_type) ps_handle_mgr_get_handle( IO_MGR_CLIENT,
                                                           buf_index );

  if( io_mgr_cb_ptr->io_mgr_handle == PS_HANDLE_MGR_INVALID_HANDLE )
  {
    PS_MEM_FREE(io_mgr_cb_ptr);
    LOG_MSG_ERROR_0("Invalid handle");
    *ps_errno = DS_EBADF;
    return retval;
  }

  io_mgr_cb_ptr->dns_query_ptr        = NULL;
  io_mgr_cb_ptr->cback_f_ptr          = cback_f_ptr;
  io_mgr_cb_ptr->user_data_ptr        = user_data_ptr;
  io_mgr_cb_ptr->is_registered_to_filters = FALSE;

  memscpy (&io_mgr_cb_ptr->net_policy, sizeof (dss_net_policy_info_type),
    net_policy_ptr, sizeof (dss_net_policy_info_type));

  io_mgr_cb_ptr->p_pending_servers = NULL;
  io_mgr_cb_ptr->max_pending_servers_index = 0;


  /*-------------------------------------------------------------------------
    Create socket using dss_socket2().
  -------------------------------------------------------------------------*/
  io_mgr_cb_ptr->sockfd = dss_socket2( (int)family,
                                       (int)DSS_SOCK_DGRAM,
                                       (int)PS_IPPROTO_UDP,
                                       ps_dnsi_io_mgri_sock_event_cback,
                                       (void *) io_mgr_cb_ptr,
                                       &io_mgr_cb_ptr->net_policy,
                                       ps_errno );
  if( DSS_ERROR == io_mgr_cb_ptr->sockfd )
  {
    LOG_MSG_ERROR_1("Error (%d) creating socket", *ps_errno);
    PS_MEM_FREE(io_mgr_cb_ptr);
    return PS_DNSI_IO_MGR_INVALID_HANDLE;
  }

  /*-------------------------------------------------------------------------
    Bind the socket to an ephemeral port.
  -------------------------------------------------------------------------*/
  memset(&this_sockaddr, 0, sizeof(this_sockaddr));
  if (DSS_AF_INET == family)
  {
    this_addrlen = sizeof(struct ps_sockaddr_in);

    ((struct ps_sockaddr_in *) &this_sockaddr)->ps_sin_family = DSS_AF_INET;
    ((struct ps_sockaddr_in *) &this_sockaddr)->ps_sin_port = 0;
    ((struct ps_sockaddr_in *) &this_sockaddr)->ps_sin_addr.ps_s_addr =
                                                                PS_INADDR_ANY;
  }
  else // DSS_AF_INET6
  {
     this_addrlen = sizeof(struct ps_sockaddr_in6);

     ((struct ps_sockaddr_in6 *) &this_sockaddr)->ps_sin6_family = DSS_AF_INET6;
     ((struct ps_sockaddr_in6 *) &this_sockaddr)->ps_sin6_port = 0;
     ((struct ps_sockaddr_in6 *) &this_sockaddr)->
                                ps_sin6_addr.in6_u.u6_addr64[0] = PS_INADDR_ANY;
     ((struct ps_sockaddr_in6 *) &this_sockaddr)->
                                ps_sin6_addr.in6_u.u6_addr64[1] = PS_INADDR_ANY;
  }

  retval = dss_bind (io_mgr_cb_ptr->sockfd,
                     (struct ps_sockaddr *) &this_sockaddr,
                     this_addrlen,
                     ps_errno);
  if (DSS_ERROR == retval)
  {
    LOG_MSG_ERROR_1("Cant bind DNS socket, err %d", *ps_errno);
    PS_MEM_FREE(io_mgr_cb_ptr);
    return PS_DNSI_IO_MGR_INVALID_HANDLE;
  }

  return  io_mgr_cb_ptr->io_mgr_handle;

} /* ps_dnsi_io_mgr_create_instance() */


/*===========================================================================
FUNCTION  PS_DNSI_IO_MGR_SEND_QUERY

DESCRIPTION
  Sends a query to the address specified by server_address. When the
  operation is complete, calls the registered callback function to return
  the results. The caller must register a callback function.

PARAMETERS
  io_mgr_handle  - Handle identifying the IO manager control block.
  dns_query_ptr  - Pointer to the DSM item chain to be sent.
  server_sa_addr - Address of the server where the query should be sent.
  errno          - Error code returned in case of error.

RETURN VALUE
  Returns DSS_ERROR on error and sets the errno to the error code. If the
  error code is DS_EWOULDBLOCK, it indicates that the results will be
  returned through callback function.

  errno values
  ------------
  DS_EWOULDBLOCK  Operation would block.
  DS_EFAULT       Invalid argument

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int16 ps_dnsi_io_mgr_send_query
(
  ps_dnsi_io_mgr_handle_type    io_mgr_handle,
  dsm_item_type               * dns_query_ptr,
  struct ps_sockaddr          * server_sa_ptr,
  int16                       * ps_errno
)
{
  ps_dnsi_io_mgr_cb_type      * io_mgr_cb_ptr = NULL;
  ip_filter_type                filters[1];
  struct ps_sockaddr_storage    this_sockaddr;
  uint16                        this_addrlen;
  int16                         retval=0;
  int16                         local_errno;
  struct ps_sockaddr_in         temp_sockaddr_in;
  struct ps_sockaddr_in6        temp_sockaddr_in6;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3("Send query called for data 0x%p, using handle %d, to addr 0x%p",
                  dns_query_ptr, io_mgr_handle, server_sa_ptr);

  /*------------------------------------------------------------------------
    Validate function arguments
  -------------------------------------------------------------------------*/
  if( NULL == ps_errno )
  {
    LOG_MSG_ERROR_0("NULL errno");
    return DSS_ERROR;
  }

  if( NULL == dns_query_ptr || NULL == server_sa_ptr )
  {
    LOG_MSG_ERROR_2("Null argument(s) 0x%p, 0x%p",
                    dns_query_ptr, server_sa_ptr);
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  io_mgr_cb_ptr = ps_dnsi_io_mgri_is_handle_valid( io_mgr_handle );
  if( NULL ==  io_mgr_cb_ptr )
  {
    LOG_MSG_ERROR_1("Invalid handle %d", io_mgr_handle);
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if( DSS_AF_INET == server_sa_ptr->ps_sa_family )
  {
    memscpy( &(io_mgr_cb_ptr->server_sockaddr), sizeof(struct ps_sockaddr_in),
      server_sa_ptr, sizeof(struct ps_sockaddr_in) );
  }
  else if( DSS_AF_INET6 == server_sa_ptr->ps_sa_family )
  {
    memscpy( &(io_mgr_cb_ptr->server_sockaddr), sizeof(struct ps_sockaddr_in6),
      server_sa_ptr, sizeof(struct ps_sockaddr_in6) );
  }
  else
  {
    LOG_MSG_ERROR_1("Invalid address family %d", server_sa_ptr->ps_sa_family );
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if( io_mgr_cb_ptr->is_registered_to_filters )
  {
    LOG_MSG_INFO1_0("Filters deregistration is needed");
    if (io_mgr_cb_ptr->net_policy.is_routeable &&
        DSS_IFACE_INVALID_ID != io_mgr_cb_ptr->iface_id)
    {
      retval = dss_dereg_ip_filter( io_mgr_cb_ptr->sockfd,
                                    io_mgr_cb_ptr->iface_id,
                                    &local_errno );
      if(DSS_ERROR == retval)
      {
        LOG_MSG_ERROR_2("Error %d deregistering filters on sock %d",
                        local_errno, io_mgr_cb_ptr->sockfd);
      }
      else
      {
        LOG_MSG_INFO1_2("Deregistered filters installed on sock %d, iface 0x%x",
                        io_mgr_cb_ptr->sockfd, io_mgr_cb_ptr->iface_id);
        io_mgr_cb_ptr->is_registered_to_filters = FALSE;
      }
    } /* if(TRUE == io_mgr_cb_ptr->net_policy.is_routeable) */
  } /* if(io_mgr_cb_ptr->is_registered_to_filters) */

  if( io_mgr_cb_ptr->dns_query_ptr != NULL )
  {
    dsm_free_packet( &(io_mgr_cb_ptr->dns_query_ptr) );
  } /* if(io_mgr_cb_ptr->dns_query_ptr != NULL) */

  if (PS_UTILS_DSM_DUP_PACKET (&(io_mgr_cb_ptr->dns_query_ptr),
                      dns_query_ptr,
                      0,
                      (uint16) dsm_length_packet(dns_query_ptr))  !=
      (uint16) dsm_length_packet (dns_query_ptr))
  {
    LOG_MSG_ERROR_0 ("Cant dup DSM packet, aborting DNS send");
    dsm_free_packet (&(io_mgr_cb_ptr->dns_query_ptr));
    *ps_errno = DS_ENOMEM;
    return DSS_ERROR;
  };

  /*-------------------------------------------------------------------------
    Register filters if net_policy.is_routeable is TRUE
  -------------------------------------------------------------------------*/
  if (TRUE == io_mgr_cb_ptr->net_policy.is_routeable)
  {

    /*-----------------------------------------------------------------------
      Perform route lookup to determine iface_id. This is required if filters
      need to be installed.
    -----------------------------------------------------------------------*/
    io_mgr_cb_ptr->iface_id =
      dss_get_iface_id_by_policy (io_mgr_cb_ptr->net_policy, &local_errno);
    if (DSS_IFACE_INVALID_ID == io_mgr_cb_ptr->iface_id)
    {
      LOG_MSG_ERROR_1 ("Route lookup failed, cant install filters, err %d",
                       local_errno);
      *ps_errno = local_errno;
      return DSS_ERROR;
    }

    LOG_MSG_INFO1_2("Installing filters for socket %d on iface 0x%x",
                    io_mgr_cb_ptr->sockfd, io_mgr_cb_ptr->iface_id);

    memset(&this_sockaddr, 0, sizeof(this_sockaddr));
    this_addrlen = sizeof(struct ps_sockaddr_in);

    retval = dss_getsockname( io_mgr_cb_ptr->sockfd,
                              (struct ps_sockaddr *) &this_sockaddr,
                              &this_addrlen,
                              &local_errno);
    if (-1 == retval)
    {
      LOG_MSG_ERROR_1("Error in getsockname %d", local_errno);
      *ps_errno = local_errno;
      return DSS_ERROR;
    }

    LOG_MSG_INFO1_1("DNS socket bound to port %d",
                    ((struct ps_sockaddr_in *) &this_sockaddr)->ps_sin_port);

    if( DSS_AF_INET == server_sa_ptr->ps_sa_family )
    {
      filters[0].ip_vsn = IP_V4;
      /*lint -save -e655 Reason for suppressing error 655*/
      filters[0].ip_hdr.v4.field_mask = (uint32)(IPFLTR_MASK_IP4_NEXT_HDR_PROT |
                                        IPFLTR_MASK_IP4_SRC_ADDR);
      /*lint -restore Restore lint error 655*/

      memscpy(&temp_sockaddr_in, sizeof(struct ps_sockaddr_in),
        server_sa_ptr, sizeof(struct ps_sockaddr_in));

      filters[0].ip_hdr.v4.src.addr = temp_sockaddr_in.ps_sin_addr;
      filters[0].ip_hdr.v4.src.subnet_mask.ps_s_addr = 0xFFFFFFFF;

      filters[0].ip_hdr.v4.next_hdr_prot = (uint8)PS_IPPROTO_UDP;
      /*lint -save -e655 Reason for suppressing error 655*/
      filters[0].next_prot_hdr.udp.field_mask = (uint32)(IPFLTR_MASK_UDP_SRC_PORT |
                                                IPFLTR_MASK_UDP_DST_PORT);
      /*lint -restore Restore lint error 655*/

      filters[0].next_prot_hdr.udp.dst.port =
        ((struct ps_sockaddr_in *) &this_sockaddr)->ps_sin_port;
      filters[0].next_prot_hdr.udp.dst.range = 0;
      filters[0].next_prot_hdr.udp.src.port = temp_sockaddr_in.ps_sin_port;
      filters[0].next_prot_hdr.udp.src.range = 0;
    }
    else if( DSS_AF_INET6 == server_sa_ptr->ps_sa_family )
    {
      filters[0].ip_vsn = IP_V6;
      /*lint -save -e655 Reason for suppressing error 655*/
      filters[0].ip_hdr.v6.field_mask = (uint32)(IPFLTR_MASK_IP6_NEXT_HDR_PROT |
                                        IPFLTR_MASK_IP6_SRC_ADDR);
      /*lint -restore Restore lint error 655*/

      memscpy(&temp_sockaddr_in6, sizeof(struct ps_sockaddr_in6),
        server_sa_ptr, sizeof(struct ps_sockaddr_in6));

      filters[0].ip_hdr.v6.src.addr = temp_sockaddr_in6.ps_sin6_addr;
      filters[0].ip_hdr.v6.src.prefix_len = 64;

      filters[0].ip_hdr.v6.next_hdr_prot = (uint8)PS_IPPROTO_UDP;
      /*lint -save -e655 Reason for suppressing error 655*/
      filters[0].next_prot_hdr.udp.field_mask = (uint32)(IPFLTR_MASK_UDP_SRC_PORT |
                                                IPFLTR_MASK_UDP_DST_PORT);
      /*lint -restore Restore lint error 655*/

      filters[0].next_prot_hdr.udp.dst.port =
        ((struct ps_sockaddr_in6 *) &this_sockaddr)->ps_sin6_port;
      filters[0].next_prot_hdr.udp.dst.range = 0;
      filters[0].next_prot_hdr.udp.src.port = temp_sockaddr_in6.ps_sin6_port;
      filters[0].next_prot_hdr.udp.src.range = 0;
    }
    else
    {
      LOG_MSG_ERROR_1("Invalid address family %d",
                      server_sa_ptr->ps_sa_family );
      *ps_errno = DS_EAFNOSUPPORT;
      return DSS_ERROR;
    }

    retval = dss_reg_ip_filter(io_mgr_cb_ptr->sockfd,
                               io_mgr_cb_ptr->iface_id,
                               IPFLTR_DEFAULT_TYPE,
                               1,
                               filters,
                               &local_errno);
    if( DSS_ERROR == retval )
    {
      LOG_MSG_INFO2_1("Reg filter failed for sock %d, error %d", local_errno);
    }
    else
    {
      LOG_MSG_INFO2_1("Reg filter successfull for sock %d",
                      io_mgr_cb_ptr->sockfd);
      io_mgr_cb_ptr->is_registered_to_filters = TRUE;
    }
  } /* if(net_policy.is_routeable) */

  retval = ps_dnsi_io_mgri_sock_send( io_mgr_cb_ptr, ps_errno );
  return retval;
} /* ps_dnsi_io_mgr_send_query() */

/*===========================================================================
FUNCTION  PS_DNSI_IO_MGR_DELETE_INSTANCE()

DESCRIPTION
  This function is called when the client of IO manager times out in waiting
  for a reply or the client is through with all the queries and wants to free
  the IO manager. This function cleans up the IO manager control block, closes
  the sockets.

PARAMETERS
  io_mgr_handle - Handle to identify the IO manager control block. This was
                  returned to the client from create instance.
  errno         - Error code incase of error.

RETURN VALUE
  Returns DSS_SUCCESS in case of success.
  Returns DSS_ERROR on error and sets errno to the error code.

  errno values
  ------------
  DS_EFAULT       - if invalid/already cleaned IO manager handle is
                    given as argument

DEPENDENCIES
  None

SIDE EFFECTS
  Aborts all pending operations initiated by ps_dnsi_io_mgr_send_query().
===========================================================================*/
int16 ps_dnsi_io_mgr_delete_instance
(
  ps_dnsi_io_mgr_handle_type     io_mgr_handle,
  int16                        * ps_errno
)
{
  ps_dnsi_io_mgr_cb_type       * io_mgr_cb_ptr = NULL;
  int16                          retval;
  int16                          local_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("Delete instance called for handle %d", io_mgr_handle);

  if( NULL == ps_errno )
  {
    LOG_MSG_ERROR_0("NULL errno");
    return DSS_ERROR;
  }

  io_mgr_cb_ptr = ps_dnsi_io_mgri_is_handle_valid( io_mgr_handle );
  if( NULL ==  io_mgr_cb_ptr )
  {
    LOG_MSG_ERROR_1("Invalid handle %d", io_mgr_handle);
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    If IO manager is being deleted after a partial query, delete the
    installed filters if any and free up the DNS query DSM item.
  -------------------------------------------------------------------------*/
  if( io_mgr_cb_ptr->is_registered_to_filters )
  {
    if (TRUE == io_mgr_cb_ptr->net_policy.is_routeable &&
        DSS_IFACE_INVALID_ID != io_mgr_cb_ptr->iface_id)
    {
      retval = dss_dereg_ip_filter( io_mgr_cb_ptr->sockfd,
                                    io_mgr_cb_ptr->iface_id,
                                    &local_errno );
      if(DSS_ERROR == retval)
      {
        LOG_MSG_ERROR_2("Error %d deregistering filters on sock %d",
                        local_errno, io_mgr_cb_ptr->sockfd);
      }
      else
      {
        LOG_MSG_INFO1_2("Deregistered filters installed on sock %d, iface 0x%x",
                        io_mgr_cb_ptr->sockfd, io_mgr_cb_ptr->iface_id);
        io_mgr_cb_ptr->is_registered_to_filters = FALSE;
      }
    }
  }

  io_mgr_cb_ptr->iface_id = DSS_IFACE_INVALID_ID;

  ps_dnsi_io_mgri_sock_close( io_mgr_cb_ptr );
  return DSS_SUCCESS;
} /* ps_dnsi_io_mgr_delete_instance() */


