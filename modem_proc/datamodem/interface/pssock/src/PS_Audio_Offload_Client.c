#include "datamodem_variation.h"
#ifdef FEATURE_DATA_AUDIO_OFFLOAD
/*===========================================================================
                         PS _ AUDIO_ OFFLOAD _ CLIENT . C

DESCRIPTION

  VOIP Audio Offload Client - Implementation.

  Implements Offload Client APIs available for external clients

FUNCTIONS

  ps_audio_offload_client_add_fltr
  ps_audio_offload_client_delete_fltr
  ps_audio_offload_client_get_statistics
  ps_audio_offload_client_get_max_fltrs

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Audio_Offload_Client.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/14/13    az     Initial version of VoIP audio OC APIs implementation 
09/09/13    msr    Second revision
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/

#include <stringl/stringl.h>
#include "dssinternal.h"
#include "dssocket.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"
#include "ps_utils.h"
#include "ps_crit_sect.h"
#include "ps_handle_mgr.h"
#include "ps_iface.h"
#include "ps_ipfltr_defs.h"
#include "ps_iface_ipfltr.h"
#include "ps_ifacei_addr_v6.h"
#include "PS_Audio_Offload_Client_SIO.c"
#include "PS_Audio_Offload_Client.h"
#include "ps_audio_offload_clienti.h"


/*===========================================================================

                                INTERNAL DATA

===========================================================================*/
ps_crit_sect_type  ps_audio_offload_client_crit_sect;

static ps_audio_offload_clienti_fltr_info_type  *
  ps_audio_offload_client_fltr_info_ptr[PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR] = {0};


/*===========================================================================

                            INTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_CONVERT_ID_TO_FILTER_MASK()

  DESCRIPTION:
    The function converts the given ID to a mask which is a multiple of two  

  INPUT/OUTPUT PARAMETERS:
  filter_id - ID to be converted into a mask
    
  RETURN VALUE:
    Corresponding filter mask

  SIDE EFFECTS:
    None
===========================================================================*/
static uint8 ps_audio_offload_clienti_convert_id_to_filter_mask
(
  uint8 filter_id
)
{
  return (0x01 << filter_id);
}

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_ALLOC_FLTR_INFO()

  DESCRIPTION:
    Function allocates an audio client filter and return the handle to the
    caller

  INPUT/OUTPUT PARAMETERS:
  fltr_id -  Filter ID for which filter needs to be allocated
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any

  RETURN VALUE:
    Pointer to the filter allocated
 
    On error, return DSS_ERROR and places the error condition value in
    *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
ps_audio_offload_clienti_fltr_info_type * ps_audio_offload_clienti_alloc_fltr_info
(
  int32    fltr_id,
  int32  * err_reason,
  int16  * dss_errno
)
{
  ps_audio_offload_clienti_fltr_info_type  * fltr_info_ptr = NULL;
  int32                                      ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------
      Make sure that fltr id is valid
    -----------------------------------------------------------------------*/
    if (0 > fltr_id || PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR <= fltr_id)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_HANDLE;
      *dss_errno  = DS_EINVAL;
      break;
    }

    /*-----------------------------------------------------------------------
      Check if fltr info is not already in use
    -----------------------------------------------------------------------*/
    if (NULL != ps_audio_offload_client_fltr_info_ptr[fltr_id])
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_FLTR_ID_IN_USE;
      *dss_errno  = DS_EINVAL;
      break;
    }

    /*-----------------------------------------------------------------------
      Allocate memory for filter info
    -----------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC(fltr_info_ptr,
                             sizeof(ps_audio_offload_clienti_fltr_info_type),
                             ps_audio_offload_clienti_fltr_info_type*);

    if (NULL == fltr_info_ptr)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_OUT_OF_SYSTEM_HEAP_MEM;
      *dss_errno = DS_ENOMEM;
      break;
    }

    memset(fltr_info_ptr, 0, sizeof(ps_audio_offload_clienti_fltr_info_type));

    /*-----------------------------------------------------------------------
      Generate a handle
    -----------------------------------------------------------------------*/
    fltr_info_ptr->audio_offload_client_handle =
      ps_handle_mgr_get_handle(PS_HANDLE_MGR_CLIENT_AUDIO_OFFLOAD_CLIENT,
                               (uint16)fltr_id);
    if (PS_HANDLE_MGR_INVALID_HANDLE ==
          fltr_info_ptr->audio_offload_client_handle)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_HANDLE_MGR;
      *dss_errno = DS_EINVAL;
      break;
    }

    ps_audio_offload_client_fltr_info_ptr[fltr_id] = fltr_info_ptr;

    ret_val = 0;
  } while (0);

  /*-------------------------------------------------------------------------
    Free memory in case of error
  -------------------------------------------------------------------------*/
  if (-1 == ret_val)
  {
    PS_SYSTEM_HEAP_MEM_FREE(fltr_info_ptr);
  }

  return fltr_info_ptr;
} /* ps_audio_offload_clienti_alloc_fltr_info() */


/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_GET_FLTR_INFO()

  DESCRIPTION:
  Given filter ID, returns handle to the filter caller

  INPUT/OUTPUT PARAMETERS:
  fltr_id - Filter ID whose handle is required
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any

  RETURN VALUE:
    Handle to the filter
 
    On error, return NULL and places the error condition value in
    *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
ps_audio_offload_clienti_fltr_info_type * ps_audio_offload_clienti_get_fltr_info
(
  int32    fltr_id,
  int32  * err_reason,
  int16  * dss_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 > fltr_id || PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR <= fltr_id)
  {
    *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_FLTR_ID;
    *dss_errno  = DS_EINVAL;
    return NULL;
  }

  /*-----------------------------------------------------------------------
    Fetch the filter info corresponding to the filter ID
  -----------------------------------------------------------------------*/
  return ps_audio_offload_client_fltr_info_ptr[fltr_id];

} /* ps_audio_offload_clienti_get_fltr_info() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_GET_FLTR_INFO_BY_HANDLE()

  DESCRIPTION:
    Given filter handle, returns pointer to the filter

  INPUT/OUTPUT PARAMETERS:
  fltr_id - Filter ID whose handle is required
  err_reason - Error reason to be returned, if any

  RETURN VALUE:
    Pointer to the filter

  SIDE EFFECTS:
    None
===========================================================================*/
ps_audio_offload_clienti_fltr_info_type * ps_audio_offload_clienti_get_fltr_info_by_handle
(
  ps_audio_offload_client_handle_type    audio_offload_client_handle,
  int32                                * err_reason
)
{
  ps_audio_offload_clienti_fltr_info_type  * fltr_info_ptr;
  int16                                      fltr_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get index corresponding to the handle
  -------------------------------------------------------------------------*/
  fltr_index =
    ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_AUDIO_OFFLOAD_CLIENT,
                            audio_offload_client_handle);

  if (PS_HANDLE_MGR_INVALID_INDEX == fltr_index ||
      0 > fltr_index ||
      PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR <= fltr_index)
  {
    *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_HANDLE;
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Make sure that fltr info is in use and with the same handle as the input
  -------------------------------------------------------------------------*/
  fltr_info_ptr = ps_audio_offload_client_fltr_info_ptr[fltr_index];

  if (NULL == fltr_info_ptr ||
      audio_offload_client_handle != fltr_info_ptr->audio_offload_client_handle)
  {
    *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_HANDLE;
    return NULL;
  }

  return fltr_info_ptr;
} /* ps_audio_offload_clienti_get_fltr_info_by_handle() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_FREE_FLTR_INFO()

  DESCRIPTION:
  Free the resources associated with the given filter handle caller

  INPUT/OUTPUT PARAMETERS:
  fltr_info_ptr - Filter handle which needed to be deallocated 
  fltr_id - Filter ID given

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
===========================================================================*/
void ps_audio_offload_clienti_free_fltr_info
(
  ps_audio_offload_clienti_fltr_info_type  * fltr_info_ptr,
  int32                                      fltr_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Free the memory
  -------------------------------------------------------------------------*/
  ps_audio_offload_client_fltr_info_ptr[fltr_id] = NULL;

  PS_SYSTEM_HEAP_MEM_FREE(fltr_info_ptr);

  return;
} /* ps_audio_offload_clienti_free_fltr_info() */


/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_SET_ERROR()

  DESCRIPTION:
    The funtion maps the given error to thr selected error codes and propagates
    to the OSA/ADSP layer via SIO layer

  INPUT/OUTPUT PARAMETERS:
    fltr_info_ptr    : Filter ID whose handle is required
    err_reason       : Error reason to be returned, if any
    dss_errno        : Error code to be returned, if any

  RETURN VALUE
    Handle to the filter
 
   On error, the error condition value is placed in *dss_errno.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ps_audio_offload_clienti_set_error
(
   ps_audio_offload_clienti_fltr_info_type   * fltr_info_ptr,
   int32                                     * err_reason,
   int16                                     * dss_errno
)
{
  int32   sio_err_map;
  uint8   fltr_id;

  switch (*err_reason)
  {
    case PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_DSM_FLOW_CONTROLLED:
    {
      sio_err_map = PS_AUDIO_OFFLOAD_CLIENT_SIO_DSM_EWOULDBLOCK;

      /*-------------------------------------------------------------------------
        Send DSM flow control message to SIO against all the active filters
      --------------------------------------------------------------------------*/
      for (fltr_id = 0; fltr_id < PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR; fltr_id++)
      {
        fltr_info_ptr = ps_audio_offload_clienti_get_fltr_info(fltr_id,
                                                               err_reason,
                                                               dss_errno);
        if (NULL != fltr_info_ptr)
        {
          ps_audio_offload_client_sio_set_error(fltr_info_ptr->audio_offload_client_handle,
                                                sio_err_map,
                                                err_reason,
                                                dss_errno);
        }
      }
      break;
    }
     
    case PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SOCK_FLOW_CONTROLLED:
    {
      sio_err_map = PS_AUDIO_OFFLOAD_CLIENT_SIO_DATA_EWOULDBLOCK;

      

      
      ps_audio_offload_client_sio_set_error(fltr_info_ptr->audio_offload_client_handle,
                                            sio_err_map,
                                            err_reason,
                                            dss_errno);
      break;
    }

    case PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_READ_DATA:
    case PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_WRITE_DATA:
    case PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_READ_SOCK:
    case PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_WRITE_SOCK:
    case PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SOCK_COULD_NOT_REG_EVENT:
      sio_err_map = PS_AUDIO_OFFLOAD_CLIENT_SIO_ENETNONET_FATAL;
      break;

    case PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_OUT_OF_SYSTEM_HEAP_MEM:
    case PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_OUT_OF_DSM_ITEM:
      sio_err_map = PS_AUDIO_OFFLOAD_CLIENT_SIO_ENOMEM_FATAL;
      break;
  }

  return;
}

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_RESET_ERROR()

  DESCRIPTION:
    The funtion propagates the error, set earlier, to the OSA/ADSP layer via
    SIO layer

  INPUT/OUTPUT PARAMETERS:
  fltr_info_ptr - Filter ID whose handle is required
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any

  RETURN VALUE:
    Handle to the filter
 
    On error, the error condition value is placed in *dss_errno.

  DEPENDENCIES:
    None

  SIDE EFFECTS:
    None
===========================================================================*/
static void ps_audio_offload_clienti_reset_error
(
   ps_audio_offload_clienti_fltr_info_type   * fltr_info_ptr,
   int32                                     * err_reason,
   int16                                     * dss_errno
)
{
  uint8                                       fltr_id;

  switch (*err_reason)
  {
    case PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_DSM_FLOW_CONTROLLED:
    {
      /*-------------------------------------------------------------------------
        Send DSM flow control reset message to SIO against all the active filters
      --------------------------------------------------------------------------*/
      for (fltr_id = 0; fltr_id < PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR; fltr_id++)
      {
        fltr_info_ptr = ps_audio_offload_clienti_get_fltr_info(fltr_id, err_reason, dss_errno);

        if(NULL != fltr_info_ptr)
        {
          ps_audio_offload_client_sio_reset_error(
                                    fltr_info_ptr->audio_offload_client_handle,
                                    PS_AUDIO_OFFLOAD_CLIENT_SIO_DSM_EWOULDBLOCK,
                                    err_reason,
                                    dss_errno);
        }
      }
      break;
    }

    case PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SOCK_FLOW_CONTROLLED:
    {
      ps_audio_offload_client_sio_reset_error(
                                fltr_info_ptr->audio_offload_client_handle,
                                PS_AUDIO_OFFLOAD_CLIENT_SIO_DATA_EWOULDBLOCK,
                                err_reason,
                                dss_errno);
      break;
    }
  }
  
  return;
}

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_SOCKET_EVENT_CBACK()

 DESCRIPTION:
  This function gets called by the sockets layer when there is an
  event on the socket. Posts a command to PS task to process the event

  INPUT/OUTPUT PARAMETERS:
  dss_nethandle - The app id (if any) associated with the socket
  sockfd - The socket descriptor
  event_mask - The event(s) that occured on the socket
  user_data_ptr - The audio filter handle

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
===========================================================================*/
static void ps_audio_offload_clienti_socket_event_cback
(
  int16     dss_nethandle,
  int16     sockfd,
  uint32    event_mask,
  void    * user_data_ptr
)
{
  ps_audio_offload_clienti_socket_event_info_type  * socket_event_info_ptr;
  int32                                              err_reason;
  int32                                              ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (NULL == user_data_ptr)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NULL_ARG;
      break;
    }

    /*-------------------------------------------------------------------------
      Allocate memory for event info
    -------------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC
      (
      socket_event_info_ptr,
      sizeof(ps_audio_offload_clienti_socket_event_info_type),
      ps_audio_offload_clienti_socket_event_info_type*
      );
    if (NULL == socket_event_info_ptr)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_OUT_OF_SYSTEM_HEAP_MEM;
      break;
    }

    socket_event_info_ptr->sockfd     = sockfd;
    socket_event_info_ptr->event_mask = event_mask;

    socket_event_info_ptr->audio_offload_client_handle =
      (ps_audio_offload_client_handle_type) user_data_ptr;

    /*-------------------------------------------------------------------------
      Post a command to PS task to handle this event
    -------------------------------------------------------------------------*/
    ps_send_cmd(PS_AUDIO_OFFLOAD_CLIENT_SOCKET_EVENT_CMD,
                socket_event_info_ptr );

    ret_val = 0;
  } while (0);

  if (-1 == ret_val)
  {
    LOG_MSG_ERROR_3("ps_audio_offload_clienti_socket_event_cback(): "
                        "Error reason %d sockfd %d event mask %d",
                        err_reason, sockfd, event_mask);
    ASSERT(0);
  }

  return;
} /* ps_audio_offload_clienti_socket_event_cback() */


/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_OPEN_SOCKET()

  DESCRIPTION:
    Opens a socket with the given parameters

  INPUT/OUTPUT PARAMETERS:
  fltr_info_ptr - Filter ID whose handle is required
  iface_id - Interface ID on which socket has to be opened
  fltr_ptr - IP filter containing IP address, transport, port etc
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any

  RETURN VALUE:
    Socket descriptor or error
 
    On error, return DSS_ERROR and places the error condition value in
    *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
static int16 ps_audio_offload_clienti_open_socket
(
  ps_audio_offload_clienti_fltr_info_type  * fltr_info_ptr,
  ps_iface_id_type                           iface_id,
  ip_filter_type                           * fltr_ptr,
  int32                                    * err_reason,
  int16                                    * dss_errno
)
{
  struct ps_sockaddr        * sockaddr_ptr;
  dss_net_policy_info_type    net_policy;
  struct ps_sockaddr_in       v4_sockaddr;
  struct ps_sockaddr_in6      v6_sockaddr;
  int                         sock_opt_val;
  uint32                      sock_opt_val_len = sizeof(int);
  uint16                      addr_len;
  int16                       sockfd;
  int16                       ret_val = DSS_ERROR;
  byte                        family;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------
      Open socket with policy as iface_id
    -----------------------------------------------------------------------*/
    memset(&net_policy, 0, sizeof(dss_net_policy_info_type));
    dss_init_net_policy_info(&net_policy);

    family = (IP_V4 == fltr_ptr->ip_vsn) ? DSS_AF_INET : DSS_AF_INET6;

    net_policy.iface.kind    = DSS_IFACE_ID;
    net_policy.iface.info.id = (dss_iface_id_type) iface_id;
    net_policy.is_routeable  = TRUE;
    net_policy.family        = family;

    ret_val = dss_socket2(family,
                          DSS_SOCK_DGRAM,
                          PS_IPPROTO_UDP,
                          ps_audio_offload_clienti_socket_event_cback,
                          (void *) (fltr_info_ptr->audio_offload_client_handle),
                          &net_policy,
                          dss_errno);
    if (DSS_ERROR == ret_val)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_CREATE_SOCK;
      break;
    }

    sockfd = ret_val;

    /*-----------------------------------------------------------------------
      Decrease receive buf size in order to not hold too many DSM items
      on this socket
    -----------------------------------------------------------------------*/
    sock_opt_val = PS_AUDIO_OFFLOAD_CLIENT_SOCKET_RCV_BUF_SIZE;

    ret_val = dss_setsockopt(sockfd,
                             DSS_SOCK,
                             DSS_SO_RCVBUF,
                             &sock_opt_val,
                             &sock_opt_val_len,
                             dss_errno);
    if (DSS_ERROR == ret_val)
    {
      *err_reason =
        PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_SET_RCV_BUF_SIZE;
      break;
    }

    sock_opt_val = TRUE;

    ret_val = dss_setsockopt(sockfd,
                             DSS_SOL_SOCKET,
                             DSS_SO_SYS_SOCK,
                             &sock_opt_val,
                             &sock_opt_val_len,
                             dss_errno);
    if (DSS_ERROR == ret_val)
    {
      *err_reason =
        PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_SET_SYS_SOCK_OPT;
      break;
    }

    /*-----------------------------------------------------------------------
      Bind the socket
    -----------------------------------------------------------------------*/
    if (IP_V4 == fltr_ptr->ip_vsn)
    {
      v4_sockaddr.ps_sin_family         = DSS_AF_INET;
      v4_sockaddr.ps_sin_port           = fltr_ptr->next_prot_hdr.udp.dst.port;
      v4_sockaddr.ps_sin_addr.ps_s_addr = PS_INADDR_ANY;

      sockaddr_ptr = (struct ps_sockaddr *) &v4_sockaddr;
      addr_len     = sizeof(struct ps_sockaddr_in);
    }
    else
    {
      memset(&v6_sockaddr, 0, sizeof(struct ps_sockaddr_in6));
      v6_sockaddr.ps_sin6_family   = DSS_AF_INET6;
      v6_sockaddr.ps_sin6_port     = fltr_ptr->next_prot_hdr.udp.dst.port;

      memscpy(&v6_sockaddr.ps_sin6_addr,
              sizeof(struct ps_in6_addr),
              &fltr_ptr->ip_hdr.v6.dst.addr,
              sizeof(struct ps_in6_addr));

      sockaddr_ptr = (struct ps_sockaddr *) &v6_sockaddr;
      addr_len     = sizeof(struct ps_sockaddr_in6);
    }

    ret_val = dss_bind(sockfd, sockaddr_ptr, addr_len, dss_errno);
    if (DSS_ERROR == ret_val)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_BIND_SOCK;
      break;
    }

    /*-----------------------------------------------------------------------
      Register socket filter
    -----------------------------------------------------------------------*/
    ret_val = dss_reg_ip_filter(sockfd,
                                iface_id,
                                IPFLTR_DEFAULT_TYPE,
                                1,
                                fltr_ptr,
                                dss_errno);
    if (DSS_ERROR == ret_val)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_REG_FLTR;
      break;
    }

    fltr_info_ptr->sockfd    = sockfd;
    fltr_info_ptr->iface_id  = iface_id;
    fltr_info_ptr->ip_family = family;

    ret_val = sockfd;
  } while (0);

  return ret_val;
} /* ps_audio_offload_clienti_open_socket() */


/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_CLOSE_SOCKET()

  DESCRIPTION:
    Closes a socket with the given filter handle

  INPUT/OUTPUT PARAMETERS:
  fltr_info_ptr - Filter handle whose socket needs to be closed

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
===========================================================================*/
static void ps_audio_offload_clienti_close_socket
(
  ps_audio_offload_clienti_fltr_info_type  * fltr_info_ptr
)
{
  int16   ret_val = DSS_ERROR;
  int16   dss_errno;
  int32   err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    /*-------------------------------------------------------------------------
      Deregister flters
    -------------------------------------------------------------------------*/
    ret_val = dss_dereg_ip_filter(fltr_info_ptr->sockfd,
                                  fltr_info_ptr->iface_id,
                                  &dss_errno);
    if (DSS_ERROR == ret_val)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_DEREG_FLTR;
      ASSERT(0);
      break;
    }

    /*-------------------------------------------------------------------------
      Close socket
    -------------------------------------------------------------------------*/
    ret_val = dss_close(fltr_info_ptr->sockfd, &dss_errno);
    if (DSS_ERROR == ret_val)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_CLOSE_SOCK;
      ASSERT(0);
      break;
    }

    /*-------------------------------------------------------------------------
      Free DSM items
    -------------------------------------------------------------------------*/
    if (NULL != fltr_info_ptr->read_dsm_item_ptr)
    {
      dsm_free_packet(&(fltr_info_ptr->read_dsm_item_ptr));
    }

    if (NULL != fltr_info_ptr->write_dsm_item_ptr)
    {
      dsm_free_packet(&(fltr_info_ptr->write_dsm_item_ptr));
    }
  } while (0); 

  if (DSS_ERROR == ret_val)
  {
    LOG_MSG_FUNCTION_EXIT_2("ps_audio_offload_clienti_close_socket(): "
                                "err reason %d fltr handle 0x%x",
                                err_reason,
                                fltr_info_ptr->audio_offload_client_handle);
  }
  
  return;
} /* ps_audio_offload_clienti_close_socket() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_READ_SOCKET()

  DESCRIPTION:
    The function first write any outstanding data to SIO port and then reads
    on the socket in a loop until there is no data to be read. The IP address
    and family is prepended to the received payload before sending on to
    the SIO port

  INPUT/OUTPUT PARAMETERS:
  fltr_info_ptr - Filter handle on which read socket operation is requested
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any
 
  RETURN VALUE:
    Success if able to read on the socket otherwise an error
 
    On error, return DSS_ERROR and places the error condition value in
    *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
static int32 ps_audio_offload_clienti_read_socket
(
  ps_audio_offload_clienti_fltr_info_type   * fltr_info_ptr,
  int32                                     * err_reason,
  int16                                     * dss_errno
)
{
  struct ps_sockaddr        * sockaddr_ptr;
  int32                       bytes_read;
  int32                       bytes_tx;
  int32                       bytes_pushed_down;
  int32                       ret_val;
  uint16                      addr_len;

  ps_audio_offload_clienti_data_hdr offload_data_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (DSS_AF_INET == fltr_info_ptr->ip_family)
  {
    sockaddr_ptr = (struct ps_sockaddr *) &(offload_data_hdr.sock_addr.v4_sockaddr);
    addr_len = sizeof(struct ps_sockaddr_in);
  }
  else
  {
    sockaddr_ptr = (struct ps_sockaddr *) &(offload_data_hdr.sock_addr.v6_sockaddr);
    addr_len = sizeof(struct ps_sockaddr_in6);
  }

  while (1)
  {
    if (NULL != fltr_info_ptr->read_dsm_item_ptr)
    {
      bytes_tx = ps_audio_offload_client_sio_write
                 (
                   fltr_info_ptr->audio_offload_client_handle,
                   &(fltr_info_ptr->read_dsm_item_ptr),
                   err_reason,
                   dss_errno
                 );
      if (-1 == bytes_tx)
      {        
        ret_val = -1;
        break;
      }
      else if (0 == bytes_tx)
      {
        fltr_info_ptr->is_sio_write_enabled = FALSE;
        ret_val = 0;
        break;
      }
    }

    bytes_read = dss_recvfrom_dsm_chain(fltr_info_ptr->sockfd,
                                  &(fltr_info_ptr->read_dsm_item_ptr),
                                  0,
                                  sockaddr_ptr,
                                  &addr_len,
                                  dss_errno);
    if (-1 == bytes_read)
    {
      if (DS_EWOULDBLOCK != *dss_errno)
      {
        *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_READ_SOCK;
        ps_audio_offload_clienti_set_error(fltr_info_ptr, err_reason, dss_errno);
        ret_val = -1;
      }
      else
      {
        ret_val = dss_async_select(fltr_info_ptr->sockfd, DS_READ_EVENT, dss_errno);

        if (DSS_SUCCESS != ret_val)
        {
          *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SOCK_COULD_NOT_REG_EVENT;
          ps_audio_offload_clienti_set_error(fltr_info_ptr, err_reason, dss_errno);
          ret_val = -1;
        }
        else
        {
          fltr_info_ptr->socket_event_mask |= DS_READ_EVENT;
          ret_val = 0;
        }
      }
      break;
    }
    else if (0 < bytes_read)
    {
      if (bytes_read > PS_AUDIO_OFFLOAD_CLIENT_MAX_PACKET_SIZE)
      {
        LOG_MSG_INFO1_1("ps_audio_offload_clienti_read_socket(): "
                            "Packet size > 1 KB, packet size: %d", bytes_read);
        dsm_free_packet(&(fltr_info_ptr->read_dsm_item_ptr));
        continue;
      }
 
      offload_data_hdr.payload_len   = bytes_read;
      offload_data_hdr.sock_addr_len = addr_len;

      bytes_pushed_down = dsm_pushdown(&(fltr_info_ptr->read_dsm_item_ptr),
                                       &offload_data_hdr,
                                       sizeof(offload_data_hdr),
                                       DSM_DS_SMALL_ITEM_POOL);

      if (sizeof(offload_data_hdr) != bytes_pushed_down)
      {
        *err_reason =
          PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_PUSH_DOWN_ADDR;

        LOG_MSG_ERROR_2("ps_audio_offload_clienti_read_socket(): "
              "Not able to push down the offload data hdr. Bytes pulled up %d"
              "sizeof(offload_data_hdr): %d", bytes_pushed_down,
               sizeof(offload_data_hdr));
        ret_val = -1;
        break;
      }

      fltr_info_ptr->stats.bytes_rx += bytes_read;
      fltr_info_ptr->stats.pkts_rx++;
    }
  }

  return ret_val;
} /* ps_audio_offload_clienti_read_socket() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_WRITE_SOCKET()

  DESCRIPTION:
    The function first sends the data via socket and read for more data
    available from SIO port in a loop until there is no data to be sent

  INPUT/OUTPUT PARAMETERS:
  fltr_info_ptr - Filter handle on which write socket operation is
                       requested
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any
 
  RETURN VALUE:
    Success if able to read on the socket otherwise an error
 
    On error, return DSS_ERROR and places the error condition value in
    *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
static int32 ps_audio_offload_clienti_write_socket
(
  ps_audio_offload_clienti_fltr_info_type  * fltr_info_ptr,
  int32                                    * err_reason,
  int16                                    * dss_errno
)
{
  struct ps_sockaddr        * sockaddr_ptr;
  dsm_item_type             * tmp_dsm_item_ptr;
  int32                       bytes_pulled_up;
  int32                       bytes_pushed_down;
  int32                       bytes_read;
  int32                       bytes_tx = 0;
  int32                       ret_val = 0;
  uint16                      addr_len;


  ps_audio_offload_clienti_data_hdr   offload_data_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (DSS_AF_INET == fltr_info_ptr->ip_family)
  {
    sockaddr_ptr = (struct ps_sockaddr *) &(offload_data_hdr.sock_addr.v4_sockaddr);
    addr_len     = sizeof(struct ps_sockaddr_in);
  }
  else
  {
    sockaddr_ptr = (struct ps_sockaddr *) &(offload_data_hdr.sock_addr.v6_sockaddr);
    addr_len     = sizeof(struct ps_sockaddr_in6);
  }

  /*-------------------------------------------------------------------------
      First check if there is any outstanding DSM item that we have to send,
      if not, then retrieve incoming data from SMDL port and then transfer onto
      the wire
   -------------------------------------------------------------------------*/
  while (1)
  {
    if (NULL != fltr_info_ptr->write_dsm_item_ptr)
    {
      tmp_dsm_item_ptr = fltr_info_ptr->write_dsm_item_ptr;
      fltr_info_ptr->write_dsm_item_ptr = NULL;
    }
    else
    {
      bytes_read = ps_audio_offload_client_sio_read
                    (
                     fltr_info_ptr->audio_offload_client_handle,
                     &tmp_dsm_item_ptr,
                     err_reason,
                     dss_errno
                    );
      if (-1 == bytes_read)
      {
        ret_val = -1;
        break;
      }
      else if (0 == bytes_read)
      {
        break;
      }
    }

    bytes_pulled_up = PS_UTILS_DSM_PULLUP(&tmp_dsm_item_ptr,
                                 &offload_data_hdr,
                                 sizeof(offload_data_hdr));
    if (sizeof(offload_data_hdr) != bytes_pulled_up)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_PULL_UP_ADDR;
      dsm_free_buffer(tmp_dsm_item_ptr);
      ret_val = -1;
      ASSERT(0);
      break;
    }

    bytes_tx = dss_sendto_dsm_chain(fltr_info_ptr->sockfd,
                                    &tmp_dsm_item_ptr,
                                    0,
                                    sockaddr_ptr,
                                    addr_len,
                                    dss_errno);
    if (-1 == bytes_tx)
    {
      if (DS_EWOULDBLOCK == *dss_errno)
      {
        ret_val =
          dss_async_select(fltr_info_ptr->sockfd, DS_WRITE_EVENT, dss_errno);
        if (DSS_SUCCESS != ret_val)
        {
          *err_reason =
            PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SOCK_COULD_NOT_REG_EVENT;
        }
        else
        {
          *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SOCK_FLOW_CONTROLLED;
          fltr_info_ptr->socket_event_mask |= DS_WRITE_EVENT;
       }
      }
      else
      {
        *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_WRITE_SOCK;
        ret_val     = -1;
      }

      /*-------------------------------------------------------------------------
        Inform SIO that an error has occured while transfering the packet onto
        the wire. This is to make sure SIO/ADSP does not keep sending the data to
        offload client. Upon resetting the error, the data transfer will resume
      -------------------------------------------------------------------------*/
      ps_audio_offload_clienti_set_error(fltr_info_ptr, err_reason, dss_errno);

      bytes_pushed_down = dsm_pushdown(&tmp_dsm_item_ptr,
                                       &offload_data_hdr,
                                       sizeof(offload_data_hdr),
                                       DSM_DS_SMALL_ITEM_POOL);
      if (sizeof(offload_data_hdr) != bytes_pushed_down)
      {
        *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_PUSH_DOWN_ADDR;
        ret_val = -1;
        dsm_free_buffer(tmp_dsm_item_ptr);
        ASSERT(0);
        break;
      }

      fltr_info_ptr->write_dsm_item_ptr = tmp_dsm_item_ptr;
      break;
    }
    else if (0 < bytes_tx)
    {
      fltr_info_ptr->stats.bytes_tx += bytes_tx;
      fltr_info_ptr->stats.pkts_tx++;
    }
  }

  return ret_val;
} /* ps_audio_offload_clienti_write_socket() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_PROCESS_SOCKET_EVENT()

  DESCRIPTION:
    The function gets called from the PS task context to process the pending
    sockets READ or WRITE events

  INPUT/OUTPUT PARAMETERS:
  cmd - Command type posted 
  user_data_ptr - User provided data, given at the time of posting the
                       command
  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
===========================================================================*/
void ps_audio_offload_clienti_process_socket_event
(
  ps_cmd_enum_type    cmd,
  void              * user_data_ptr
)
{
  ps_audio_offload_clienti_socket_event_info_type  * socket_event_info_ptr = NULL;
  ps_audio_offload_clienti_fltr_info_type          * fltr_info_ptr = NULL;
  int32                                              err_reason;
  int32                                              ret_val = -1;
  int16                                              dss_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_enter_crit_section(&ps_audio_offload_client_crit_sect);

  do
  {
    if (NULL == user_data_ptr)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NULL_ARG;
      dss_errno  = DS_EFAULT;
      break;
    }

    socket_event_info_ptr =
      (ps_audio_offload_clienti_socket_event_info_type *) user_data_ptr;

    fltr_info_ptr = ps_audio_offload_clienti_get_fltr_info_by_handle
                    (
                      socket_event_info_ptr->audio_offload_client_handle,
                      &err_reason
                    );
    if (NULL == fltr_info_ptr)
    {
      break;
    }

    fltr_info_ptr->socket_event_mask &= ~(socket_event_info_ptr->event_mask);

    if (DS_READ_EVENT & socket_event_info_ptr->event_mask)
    {
      if (TRUE == fltr_info_ptr->is_sio_write_enabled)
      {
        ret_val = ps_audio_offload_clienti_read_socket(fltr_info_ptr,
                                                       &err_reason,
                                                       &dss_errno);
        if (-1 == ret_val)
        {
          break;
        }
      }
      else
      {       
        ret_val = 0;
      }
    }

    if (DS_WRITE_EVENT & socket_event_info_ptr->event_mask)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SOCK_FLOW_CONTROLLED;

      ps_audio_offload_clienti_reset_error(fltr_info_ptr,
                                           &err_reason,
                                           &dss_errno);

      ret_val = ps_audio_offload_clienti_write_socket(fltr_info_ptr,
                                                      &err_reason,
                                                      &dss_errno);
      if (-1 == ret_val)
      {
        break;
      }
    }

    ret_val = 0;
  } while (0);

  if (socket_event_info_ptr)
  {

    if (DSS_ERROR == ret_val)
    {
      LOG_MSG_FUNCTION_EXIT_2("ps_audio_offload_clienti_process_socket_event(): "
                            "err reason %d fltr handle 0x%x",
                            err_reason,
                            socket_event_info_ptr->audio_offload_client_handle);
    }
    else
    {
      LOG_MSG_FUNCTION_EXIT_6("ps_audio_offload_clienti_process_socket_event(): "
                              "Processed event mask %d for fltr handle 0x%x "
                              "bytes_tx %d bytes_rx %d pkts_tx %d pkts_rx %d",
                              socket_event_info_ptr->event_mask,
                              fltr_info_ptr->audio_offload_client_handle,
                              fltr_info_ptr->stats.bytes_tx,
                              fltr_info_ptr->stats.bytes_rx,
                              fltr_info_ptr->stats.pkts_tx,
                              fltr_info_ptr->stats.pkts_rx);
    }
  }
  else
  {
    LOG_MSG_FUNCTION_EXIT_1("ps_audio_offload_clienti_process_socket_event(): "
                            "err reason %d",
                            err_reason);
  }

  PS_SYSTEM_HEAP_MEM_FREE(user_data_ptr);

  ps_leave_crit_section(&ps_audio_offload_client_crit_sect);

  return;
} /* ps_audio_offload_clienti_process_socket_event() */


/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_SIO_EVENT_CBACK()

  DESCRIPTION:
    The callback function occurs whenever there is any SIO event pending on
    the associated socket. The callback routine is passed duringthe opening
    up of the socket

  INPUT/OUTPUT PARAMETERS:
  audio_offload_client_handle - Client handle to derive the filter handle
  sio_event - Event occured on the SIO port

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
===========================================================================*/
void ps_audio_offload_clienti_sio_event_cback
(
  ps_audio_offload_client_handle_type          audio_offload_client_handle,
  ps_audio_offload_client_sio_event_enum_type  sio_event
)
{
  ps_audio_offload_clienti_fltr_info_type          * fltr_info_ptr = NULL;
  int32                                              err_reason;
  int32                                              ret_val = -1;
  int16                                              dss_errno;
  uint8                                              fltr_mask;
  int16                                              fltr_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_enter_crit_section(&ps_audio_offload_client_crit_sect);

  do
  {
    fltr_info_ptr = ps_audio_offload_clienti_get_fltr_info_by_handle
                    (
                      audio_offload_client_handle,
                      &err_reason
                    );
    if (NULL == fltr_info_ptr)
    {
      if (PS_AUDIO_OFFLOAD_CLIENT_SIO_CLOSE_EV == sio_event)
      {
        ret_val = 0;
      }
      break;
    }

    switch (sio_event)
    {
      case PS_AUDIO_OFFLOAD_CLIENT_SIO_OPEN_EV:
      {
        fltr_info_ptr->is_sio_write_enabled = TRUE;

        ret_val =
          dss_async_select(fltr_info_ptr->sockfd, DS_READ_EVENT, &dss_errno);
        if (DSS_SUCCESS != ret_val)
        {
          err_reason =
            PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SOCK_COULD_NOT_REG_EVENT;
          break;
        }

        fltr_info_ptr->socket_event_mask |= DS_READ_EVENT;
        break;
      }

      case PS_AUDIO_OFFLOAD_CLIENT_SIO_WRITE_EV:
      {
        fltr_info_ptr->is_sio_write_enabled = TRUE;

        if ((DS_READ_EVENT & fltr_info_ptr->socket_event_mask) == 0)
        {
          ret_val = ps_audio_offload_clienti_read_socket(fltr_info_ptr,
                                                         &err_reason,
                                                         &dss_errno);
        }
        break;
      }

      case PS_AUDIO_OFFLOAD_CLIENT_SIO_READ_EV:
      {
        if ((DS_WRITE_EVENT & fltr_info_ptr->socket_event_mask) == 0)
        {
          ret_val = ps_audio_offload_clienti_write_socket(fltr_info_ptr,
                                                          &err_reason,
                                                          &dss_errno);
        }

        break;
      }

      case PS_AUDIO_OFFLOAD_CLIENT_SIO_CLOSE_EV:
      {
        LOG_MSG_INFO1_0("ps_audio_offload_clienti_sio_event_cback(): Close EV");

        fltr_id = ps_handle_mgr_get_index
                  (
                      PS_HANDLE_MGR_CLIENT_AUDIO_OFFLOAD_CLIENT,
                      audio_offload_client_handle
                   );

        if (PS_HANDLE_MGR_INVALID_INDEX == fltr_id ||
            0 > fltr_id ||
            PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR <= fltr_id)
        {
          err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_HANDLE;
          break;
        }

        fltr_mask = ps_audio_offload_clienti_convert_id_to_filter_mask
                     (
                        (uint8)fltr_id
                     );

        LOG_MSG_INFO1_1("ps_audio_offload_clienti_sio_event_cback(): "
                            "Close EV: fltr mask is %d", fltr_mask);

        fltr_info_ptr->is_sio_write_enabled = FALSE;

        if (NULL != fltr_info_ptr->write_dsm_item_ptr)
        {
          dsm_free_packet(&(fltr_info_ptr->write_dsm_item_ptr));
        }

        if (NULL != fltr_info_ptr->read_dsm_item_ptr)
        {
          dsm_free_packet(&(fltr_info_ptr->read_dsm_item_ptr));
        }
     
        ret_val = ps_audio_offload_client_sio_close
                  (
                    fltr_mask,
                    audio_offload_client_handle,
                    &err_reason,
                    &dss_errno
                  );
        break;
      }
    }

    ret_val = 0;
  } while (0);

  if (DSS_ERROR == ret_val)
  {
    LOG_MSG_FUNCTION_EXIT_2("ps_audio_offload_clienti_sio_event_cback(): "
                                "Error occured, reason: %d fltr handle: 0x%x",
                                err_reason, audio_offload_client_handle);
  }
  else
  {
    LOG_MSG_FUNCTION_EXIT_2("ps_audio_offload_clienti_sio_event_cback(): "
                          "Processed SIO event %d for fltr handle 0x%x ",
                            sio_event,
                            audio_offload_client_handle);
  }

  ps_leave_crit_section(&ps_audio_offload_client_crit_sect);

  return;
} /* ps_audio_offload_clienti_sio_event_cback() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_CONVERT_FILTER_MASK_TO_ID()

  DESCRIPTION:
  The function converts the given mask, always multiple of two, to an ID

  INPUT/OUTPUT PARAMETERS:
  filter_mask - Mask having only one bit set
    
  RETURN VALUE:
    Corresponding ID

  SIDE EFFECTS:
    None
===========================================================================*/
static uint8 ps_audio_offload_clienti_convert_filter_mask_to_id
(
  uint8 filter_mask
)
{
  int   index  = 0;
  uint8 filter_id = 0;

  for (index = 0; index < PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR; index++)
  {
    if ((filter_mask >> index) & 0x01)
    {
      filter_id = index;
      break;
    }
  }

  return filter_id;
}

/*===========================================================================
  FUNCTION: 
  PS_AUDIO_OFFLOAD_CLIENT_DSM_EVENT_CBACK()

  DESCRIPTION:
  This function updates the SIO layer as per the DSM memory event

  INPUT/OUTPUT PARAMETERS:
  pool_id - Pool ID against which event is dispatched
  event - Event occured
  op - unused in this function

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None
===========================================================================*/
static void ps_audio_offload_client_dsm_event_cback
(
  dsm_mempool_id_enum_type pool_id,
  dsm_mem_level_enum_type event,
  dsm_mem_op_enum_type op
)
{
  int32                                           err_reason;
  ps_audio_offload_clienti_dsm_event_info_type  * dsm_event_info_ptr = NULL;

  (void) op;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_ERROR_0("ps_audio_offload_clienti_dsm_event_cback(): Enter");

  /*-------------------------------------------------------------------------
    Allocate memory for event info
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC
    (
    dsm_event_info_ptr,
    sizeof(ps_audio_offload_clienti_dsm_event_info_type),
    ps_audio_offload_clienti_dsm_event_info_type*
    );
  if (NULL == dsm_event_info_ptr)
  {
    err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_OUT_OF_SYSTEM_HEAP_MEM;
    LOG_MSG_ERROR_1("ps_audio_offload_clienti_dsm_event_cback(): "
                  "Unable to allocate DSM event memory %d", err_reason);
  }
  else
  {
    dsm_event_info_ptr->pool_id = pool_id;
    dsm_event_info_ptr->event   = event;

    /*-------------------------------------------------------------------------
      Post a command to PS task to handle this event
    -------------------------------------------------------------------------*/
    ps_send_cmd(PS_AUDIO_OFFLOAD_CLIENT_DSM_EVENT_CMD, dsm_event_info_ptr );
  }
} /* ps_audio_offload_clienti_dsm_event_cback() */

/*===========================================================================
  FUNCTION: 
  PS_AUDIO_OFFLOAD_CLIENTI_PROCESS_DSM_EVENT()

  DESCRIPTION:
  This function cmd handler to process DSM events

  INPUT/OUTPUT PARAMETERS:
  cmd - PS command ID
  user_data_ptr - User data provided by the caller routine

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None
===========================================================================*/
static void ps_audio_offload_clienti_process_dsm_event
(
  ps_cmd_enum_type    cmd,
  void              * user_data_ptr
)
{
  int32                                          err_reason;
  int16                                          dss_errno;
  ps_audio_offload_clienti_dsm_event_info_type * dsm_event_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if (NULL == user_data_ptr)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NULL_ARG;
      dss_errno  = DS_EFAULT;
      break;
    }

    dsm_event_info_ptr =
      (ps_audio_offload_clienti_dsm_event_info_type *) user_data_ptr;

   
    LOG_MSG_INFO1_1("ps_audio_offload_clienti_process_dsm_event(): "
                        "DSM event type %d ", dsm_event_info_ptr->event);

    if(dsm_event_info_ptr->pool_id != DSM_DS_LARGE_ITEM_POOL)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_POOL_ID;
      dss_errno  = DS_EFAULT;
      break;
    }

    switch (dsm_event_info_ptr->event)
    {
      case DSM_MEM_LEVEL_MANY:
      {
          err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_DSM_FLOW_CONTROLLED;
          ps_audio_offload_clienti_reset_error(NULL, &err_reason, &dss_errno);
          break;
      }

      case DSM_MEM_LEVEL_FEW:
      {
          err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_DSM_FLOW_CONTROLLED;
          ps_audio_offload_clienti_set_error(NULL, &err_reason, &dss_errno);
          break;
      }

      default:
        break;

    }
  }
  while (0); 

  if (DSS_ERROR == DS_EFAULT)
  {
    LOG_MSG_FUNCTION_EXIT_1("ps_audio_offload_clienti_process_dsm_event(): "
                                "Error occured. Reason %d ", err_reason);
  }

  PS_SYSTEM_HEAP_MEM_FREE(user_data_ptr);

} /* ps_audio_offload_clienti_process_dsm_event() */


/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_POWERUP()

  DESCRIPTION:
    This functional gets called during device boot up, from PS task
    It initilizes SIO module and handle manager for this component

  INPUT/OUTPUT PARAMETERS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
===========================================================================*/
void ps_audio_offload_client_powerup
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ps_init_crit_section(&ps_audio_offload_client_crit_sect);

  ps_audio_offload_client_sio_powerup
  (
    (ps_audio_offload_client_sio_event_cback_type) ps_audio_offload_clienti_sio_event_cback
  );

  /*-------------------------------------------------------------------------
    Init handle mgr for audio offload client
  -------------------------------------------------------------------------*/
  ps_handle_mgr_init_client(PS_HANDLE_MGR_CLIENT_AUDIO_OFFLOAD_CLIENT,
                            PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR,
                            0,
                            0);

  ps_set_cmd_handler(PS_AUDIO_OFFLOAD_CLIENT_SOCKET_EVENT_CMD,
                         ps_audio_offload_clienti_process_socket_event);

  ps_set_cmd_handler(PS_AUDIO_OFFLOAD_CLIENT_DSM_EVENT_CMD,
                         ps_audio_offload_clienti_process_dsm_event);

  /*-------------------------------------------------------------------------
    Register for DSM memory events for LARGE items
  -------------------------------------------------------------------------*/
  dsm_reg_mem_event_cb( DSM_DS_LARGE_ITEM_POOL,
                        DSM_MEM_LEVEL_MANY,
                        DSM_MEM_OP_FREE,
                        ps_audio_offload_client_dsm_event_cback);

  dsm_reg_mem_event_cb( DSM_DS_LARGE_ITEM_POOL,
                        DSM_MEM_LEVEL_FEW,
                        DSM_MEM_OP_NEW,
                        ps_audio_offload_client_dsm_event_cback);
  
  return;
} /* ps_audio_offload_client_powerup() */


/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_ADD_FLTR()

  DESCRIPTION:
    Clients like QMI layer, call this function to install an audio offload filter.
    It opens a socket and an SIO port to attach to the filter. After successfull
    addition of the filter, all the targetted trafic gets routed to OSA/ADSP
    module instead of its original path to app processor.

  INPUT/OUTPUT PARAMETERS:
  fltr_mask - Filter mask passed by the client layer. Contains one bit as set
  iface_id - Interface ID on which this filter has to be installed
  fltr_spec - IP filter parameters
  dss_errno - Contains error code in case if the call is not successfull
    
  RETURN VALUE:
  filter ID or failture - If positive no, means call is successfull and
                            filter ID is returned.
 
    On error, return DSS_ERROR and places the error condition value in
    *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
int32 ps_audio_offload_client_add_fltr
(
  uint8                  fltr_mask,
  ps_iface_id_type       iface_id,
  ip_filter_spec_type  * fltr_spec,
  int16                * dss_errno
)
{
  ps_audio_offload_clienti_fltr_info_type  * fltr_info_ptr = NULL;
  ip_filter_type                           * ip_fltr_ptr;
  ps_iface_type                            * iface_ptr;
  ps_ifacei_v6_addr_type                   * v6_addr_ptr = NULL;
  struct ps_in6_addr                         ip_addr;

  int32  ret_val = DSS_ERROR;
  int32  err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NONE;
  int16  tmp_dss_errno;
  uint8  fltr_id = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_enter_crit_section(&ps_audio_offload_client_crit_sect);

  do
  {
    /*-----------------------------------------------------------------------
      Validate inputs. Valid mask values are 1, 2, 4, 8, 16, 64 and 128
    -----------------------------------------------------------------------*/
    if ( (fltr_mask != 1) &&
         ((fltr_mask > 128) || (fltr_mask % 2)) )
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_FLTR_MASK;
      break;
    }
    
    if (NULL == dss_errno)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NULL_ARG;
      break;
    }

    if (NULL == fltr_spec)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NULL_ARG;
      *dss_errno = DS_EFAULT;
      break;
    }

    fltr_id = ps_audio_offload_clienti_convert_filter_mask_to_id(fltr_mask);
    
    /*-----------------------------------------------------------------------
      Validate filter
        1. Filter spec must have only one filter
        2. Filter's IP version must be either IP_V4 or IP_V6
        3. Filter's transport protocol must be UDP
        4. Filter's UDP port must be non-zero
    -----------------------------------------------------------------------*/
    if (fltr_spec->num_filters != 1)
    {
      err_reason =
        PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_NUM_FLTR_IN_FLTR_SPEC;
      *dss_errno = DS_EINVAL;
      break;
    }

    ip_fltr_ptr = &fltr_spec->list_ptr[0];

    if (IP_V4 != ip_fltr_ptr->ip_vsn && IP_V6 != ip_fltr_ptr->ip_vsn)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_IP_VSN;
      *dss_errno = DS_EINVAL;
      break;
    }

   /*-------------------------------------------------------------------------
    Determine the specific ps_iface_ptr based on the iface ID
    -------------------------------------------------------------------------*/
    iface_ptr = ps_iface_get_handle( iface_id );
    if (iface_ptr == NULL)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_IFACE_PTR;
      *dss_errno = DS_EFAULT;
      break;
    }

    if (IP_V4 == ip_fltr_ptr->ip_vsn)
    {
      if (!(IPFLTR_MASK_IP4_NEXT_HDR_PROT &
              ip_fltr_ptr->ip_hdr.v4.field_mask) ||
          PS_IPPROTO_UDP != ip_fltr_ptr->ip_hdr.v4.next_hdr_prot)
      {
        err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_NEXT_HDR_PROT;
        *dss_errno = DS_EINVAL;
        break;
      }

      if (ps_iface_get_v4_addr(iface_ptr) != ip_fltr_ptr->ip_hdr.v4.dst.addr.ps_s_addr)
      {
        err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_IP_ADDR_MISMATCHED;
        *dss_errno = DS_EINVAL;
        break;
      }
    }
    else
    {
      if (!(IPFLTR_MASK_IP6_NEXT_HDR_PROT &
              ip_fltr_ptr->ip_hdr.v6.field_mask) ||
          PS_IPPROTO_UDP != ip_fltr_ptr->ip_hdr.v6.next_hdr_prot)
      {
        err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_NEXT_HDR_PROT;
        *dss_errno = DS_EINVAL;
        break;
      }

      ip_addr.ps_s6_addr64[0] = ip_fltr_ptr->ip_hdr.v6.dst.addr.in6_u.u6_addr64[0];
      ip_addr.ps_s6_addr64[1] = ip_fltr_ptr->ip_hdr.v6.dst.addr.in6_u.u6_addr64[1];

      if(FALSE == ps_iface_find_ipv6_addr(&ip_addr, &v6_addr_ptr, &iface_ptr))
      {
        err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_IP_ADDR_MISMATCHED;
        *dss_errno = DS_EINVAL;
        break; 
      }
    }

    if (!(IPFLTR_MASK_UDP_DST_PORT &
            ip_fltr_ptr->next_prot_hdr.udp.field_mask) ||
        0 == ip_fltr_ptr->next_prot_hdr.udp.dst.port)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_UDP_DST_PORT;
      *dss_errno = DS_EINVAL;
      break;
    }

    /*-----------------------------------------------------------------------
      Allocate fltr info corresponding to the fltr id
    -----------------------------------------------------------------------*/
    fltr_info_ptr = ps_audio_offload_clienti_alloc_fltr_info(fltr_id,
                                                             &err_reason,
                                                             dss_errno);
    if (NULL == fltr_info_ptr)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Open UDP socket and register for filters
    -----------------------------------------------------------------------*/
    ret_val = ps_audio_offload_clienti_open_socket(fltr_info_ptr,
                                                   iface_id,
                                                   ip_fltr_ptr,
                                                   &err_reason,
                                                   dss_errno);
    if (DSS_ERROR == ret_val)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Open SIO data port
    -----------------------------------------------------------------------*/

    ret_val = ps_audio_offload_client_sio_open
              (
                fltr_mask,
                fltr_info_ptr->audio_offload_client_handle,
                (uint8)ip_fltr_ptr->ip_vsn,
                &err_reason,
                dss_errno
              );
    if (DSS_SUCCESS != ret_val )
    {
          LOG_MSG_ERROR_1("ps_audio_offload_client_add_fltr(): "
                              "Failed to open the data SIO port: reason %",
                              err_reason);
      break;
    }

    ret_val = DSS_SUCCESS;
  } while (0);

  /*-------------------------------------------------------------------------
    Perform error handling
      1. Close socket
      2. Close SIO port
      3. Free up dynamically allocated memory
  -------------------------------------------------------------------------*/
  if (DSS_ERROR == ret_val)
  {
    if (NULL != fltr_info_ptr)
    {
      (void) ps_audio_offload_client_delete_fltr
	  		 (fltr_info_ptr->audio_offload_client_handle,
			  &tmp_dss_errno);
    }

    LOG_MSG_ERROR_2("ps_audio_offload_client_add_fltr(): "
                        "Failed to add the fltr err reason %d fltr id %d",
                        err_reason, fltr_id);
  }
  else
  {
    LOG_MSG_INFO1_3("ps_audio_offload_client_add_fltr(): "
                        "Added filter with handle %d iface id 0x%x sockfd %d",
                        fltr_info_ptr->audio_offload_client_handle,
                        iface_id,
                        fltr_info_ptr->sockfd);
    ret_val = fltr_info_ptr->audio_offload_client_handle;
  }

  ps_leave_crit_section(&ps_audio_offload_client_crit_sect);

  return ret_val;
} /* ps_audio_offload_client_add_fltr() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_DELETE_FLTR()

  DESCRIPTION:
    Function delets the filter given the filter handle. All the associated
    resources are deallocated

  INPUT/OUTPUT PARAMETERS:
  fltr_handle - Filter handle, initially returned to the caller on
                  ps_audio_offload_client_add_fltr() call
  dss_errno - Contains error code in case if the call is not successfull
    
  RETURN VALUE:
    Success or failure

  SIDE EFFECTS:
    None
===========================================================================*/
int32 ps_audio_offload_client_delete_fltr
(
  int32     fltr_handle,
  int16   * dss_errno
)
{
  ps_audio_offload_clienti_fltr_info_type  * fltr_info_ptr = NULL;
  ps_audio_offload_client_handle_type        audio_offload_client_handle = 0;

  int32  err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NONE;
  int32  ret_val    = DSS_ERROR;
  int16  sockfd     = 0;
  int32  fltr_id    = 0;
  uint8  fltr_mask  = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_enter_crit_section(&ps_audio_offload_client_crit_sect);

  do
  {
    /*-----------------------------------------------------------------------
      Validate input
    -----------------------------------------------------------------------*/
    if (NULL == dss_errno)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NULL_ARG;
      break;
    }

    fltr_id = ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_AUDIO_OFFLOAD_CLIENT,
                                      fltr_handle);
    /*-----------------------------------------------------------------------
      Fetch the filter info corresponding to the filter ID
    -----------------------------------------------------------------------*/
    fltr_info_ptr =
      ps_audio_offload_clienti_get_fltr_info(fltr_id, &err_reason, dss_errno);
    if (NULL == fltr_info_ptr)
    {
      break;
    }

    fltr_mask = ps_audio_offload_clienti_convert_id_to_filter_mask((uint8)fltr_id);


    /*-----------------------------------------------------------------------
      Perform clean up
        1. Close SIO port
        2. Close socket
        3. Free up filter info
    -----------------------------------------------------------------------*/
    ret_val = ps_audio_offload_client_sio_close
              (
                fltr_mask,
                fltr_info_ptr->audio_offload_client_handle,
                &err_reason,
                dss_errno
              );
    if (DSS_ERROR == ret_val)
    {
      ps_audio_offload_clienti_set_error(fltr_info_ptr, &err_reason, dss_errno);
    }

    sockfd                      = fltr_info_ptr->sockfd;
    audio_offload_client_handle = fltr_info_ptr->audio_offload_client_handle;

    ps_audio_offload_clienti_close_socket(fltr_info_ptr);
    ps_audio_offload_clienti_free_fltr_info(fltr_info_ptr, fltr_id);

    ret_val = DSS_SUCCESS;
  } while (0);


  if (DSS_ERROR == ret_val)
  {
    LOG_MSG_FUNCTION_EXIT_2("ps_audio_offload_client_delete_fltr(): "
                          "err reason %d fltr handle 0x%x",
                          err_reason,
                          audio_offload_client_handle);
  }
  else
  {
    LOG_MSG_FUNCTION_EXIT_2("ps_audio_offload_client_delete_fltr(): "
                                "Deleted fltr handle 0x%x sockfd %d",
                                audio_offload_client_handle, sockfd);
  }

  ps_leave_crit_section(&ps_audio_offload_client_crit_sect);

  return ret_val;
} /* ps_audio_offload_client_delete_fltr() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_GET_STATS()

  DESCRIPTION:
    Function retrieves the traffic statistics associated with the given filter 
    
  INPUT/OUTPUT PARAMETERS:
  fltr_handle - Filter handle for which traffic statistics
                        are required
  offload_stats_ptr - User space pointer where statistics have to be returned
  dss_errno - Contains error code in case if the call is not successfull

  RETURN VALUE:
  Success or failure

  SIDE EFFECTS:
    None
===========================================================================*/
int32 ps_audio_offload_client_get_stats
(
  int32                                 fltr_handle,
  ps_audio_offload_client_stats_type  * offload_stats_ptr,
  int16                               * dss_errno
)
{
  ps_audio_offload_clienti_fltr_info_type  * fltr_info_ptr = NULL;

  int32  err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NONE;
  int32  ret_val    = DSS_ERROR;
  int32  fltr_id    = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_enter_crit_section(&ps_audio_offload_client_crit_sect);

  do
  {
    /*-----------------------------------------------------------------------
      Validate input
    -----------------------------------------------------------------------*/
    if (NULL == dss_errno || NULL == offload_stats_ptr)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NULL_ARG;
      break;
    }
	
    fltr_id = ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_AUDIO_OFFLOAD_CLIENT, fltr_handle);
	
    /*-----------------------------------------------------------------------
      Fetch the filter info corresponding to the filter ID
    -----------------------------------------------------------------------*/
    fltr_info_ptr =
      ps_audio_offload_clienti_get_fltr_info(fltr_id, &err_reason, dss_errno);
    if (NULL == fltr_info_ptr)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_FLTR_PTR;
      break;
    }

    /*-----------------------------------------------------------------------
      Fetch the stats from filter info
    -----------------------------------------------------------------------*/
    (void) memscpy(offload_stats_ptr,
                   sizeof(ps_audio_offload_client_stats_type),
                   &(fltr_info_ptr->stats),
                   sizeof(ps_audio_offload_client_stats_type));

    ret_val = DSS_SUCCESS;
  } while (0);

  if (fltr_info_ptr)
  {
    if (DSS_ERROR == ret_val)
    {
      LOG_MSG_FUNCTION_EXIT_2("ps_audio_offload_client_get_stats(): "
                            "err reason %d fltr handle 0x%x",
                            err_reason,
                            fltr_info_ptr->audio_offload_client_handle);
    }
    else
    {
      LOG_MSG_FUNCTION_EXIT_5("ps_audio_offload_client_get_stats(): "
                              "Fltr handle 0x%x bytes_tx %d bytes_rx %d "
                              "pkts_tx %d pkts_rx %d",
                              fltr_info_ptr->audio_offload_client_handle,
                              fltr_info_ptr->stats.bytes_tx,
                              fltr_info_ptr->stats.bytes_rx,
                              fltr_info_ptr->stats.pkts_tx,
                              fltr_info_ptr->stats.pkts_rx);
    }
  }
  else
  {
    LOG_MSG_FUNCTION_EXIT_1("ps_audio_offload_client_get_stats(): "
                            "err reason %d",
                            err_reason);
  }

  ps_leave_crit_section(&ps_audio_offload_client_crit_sect);

  return ret_val;
} /* ps_audio_offload_client_get_stats() */


/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_GET_MAX_FLTR()

  DESCRIPTION:
    Function returns the maximum no. of filters that can be supported
    
  INPUT/OUTPUT PARAMETERS:
    None
 
  RETURN VALUE:
    Maximum no of filters

  SIDE EFFECTS:
    None
===========================================================================*/
uint8 ps_audio_offload_client_get_max_fltr
(
   void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR;
} /* ps_audio_offload_client_get_max_fltr() */

#endif
