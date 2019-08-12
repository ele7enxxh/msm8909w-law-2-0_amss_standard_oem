/*===========================================================================

               I P C    R O U T E R   X A L  S M D L  R E X

        This file provides Rex OS specific functionality of the smdl
                          XAL for the IPC Router

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/ipc_router/src/ipc_router_xal_smdl_common.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/11   aep     Initial creation
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/

#include "ipc_router_types.h"
#include "ipc_router_xal.h"
#include "ipc_router_xal_smdl.h"
#include "ipc_router_xal_smdl_common.h"
#include "ipc_router_protocol.h"
#include "ipc_router_packet.h"
#include "smd_lite.h"

/* Support for DSM based ipc router packet is 
 * deprecated. */
#ifndef IPC_ROUTER_NATIVE_PACKET
#error "Unsupported packet interface"
#endif

/*===========================================================================
                  CONSTANT / MACRO DECLARATIONS
===========================================================================*/

/*===========================================================================
                        EXPORTED FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION ipc_router_xal_smdl_handle_remote_resume

DESCRIPTION
  Function called by the IPC Router's SMDL worker thread to handle resume

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_handle_remote_resume(ipc_router_xal_smdl_port_type *port)
{
  port->tx_start = TRUE;
  ipc_router_xal_up(port->cb_handle);
  ipc_router_xal_smdl_handle_tx(port);
  ipc_router_xal_smdl_handle_rx(port);
}

/*===========================================================================
FUNCTION ipc_router_xal_smdl_handle_remote_close

DESCRIPTION
  Function called by the IPC Router's SMDL worker thread to handle remote
  close

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_handle_remote_close(ipc_router_xal_smdl_port_type *port)
{
  ipc_router_packet_type *pkt = NULL;

  /* Notify that smd is down */
  ipc_router_xal_down(port->cb_handle);

  port->tx_start = FALSE;
  /* Flush all pending packets */
  while((pkt = ipc_router_packet_queue_get(&port->tx_queue)) != NULL)
  {
    ipc_router_packet_free(&pkt);
  }
  if(port->tx_leftover)
  {
     ipc_router_packet_free(&port->tx_leftover);
     port->tx_leftover = NULL;
  }
  if(port->rx_leftover)
  {
     ipc_router_packet_free(&port->rx_leftover);
     port->rx_leftover = NULL;
  }
}


/*===========================================================================
FUNCTION ipc_router_xal_smdl_handle_rx

DESCRIPTION
  Function called by the IPC Router's SMDL worker thread to handle RX

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_handle_rx(ipc_router_xal_smdl_port_type *port)
{
  int32 bytes;
  ipc_router_packet_type *packet;

  if(port->rx_start == FALSE || port->tx_start == FALSE)
  {
    return;
  }

  ipc_router_os_sem_lock(&port->lock);
  port->rx_ref++;
  if(port->rx_ref > 1)
  {
    ipc_router_os_sem_unlock(&port->lock);
    return;
  }

  do {
    ipc_router_os_sem_unlock(&port->lock);
    
    while((bytes = smdl_rx_peek(port->handle)) > 0)
    {
      if(port->rx_leftover)
      {
        packet = port->rx_leftover;
      }
      else
      {
        port->rx_leftover = packet = ipc_router_packet_new(IPC_ROUTER_MAX_HEADER_SIZE, bytes);
        if(!packet)
        {
          /* TODO: Log Error */
          break;
        }
        ((smdl_iovec_type *)packet)->length = bytes;
      }

      if(smdl_readv(port->handle, (smdl_iovec_type *)packet, SMDL_READ_FLAG_LONG) != SMDL_CONTINUE)
      {
        ipc_router_xal_recv(port->rx_leftover, port->cb_handle);
        port->rx_leftover = NULL;
      }
    }
  
    ipc_router_os_sem_lock(&port->lock);
    port->rx_ref--;
  } while(port->rx_ref > 0);
  ipc_router_os_sem_unlock(&port->lock);
}


/*===========================================================================
FUNCTION ipc_router_xal_smdl_handle_tx

DESCRIPTION
  Function called by the IPC Router's SMDL worker thread to handle TX

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_handle_tx(ipc_router_xal_smdl_port_type *port)
{
  ipc_router_packet_type *packet = NULL;
  int32 rc;

  if(port->rx_start == FALSE || port->tx_start == FALSE)
  {
    return;
  }

  ipc_router_os_sem_lock(&port->lock);
  port->tx_ref++;
  if(port->tx_ref > 1)
  {
    ipc_router_os_sem_unlock(&port->lock);
    return;
  }

  do {
    ipc_router_os_sem_unlock(&port->lock);

    while(port->tx_start)
    {
      port->tx_leftover = packet = port->tx_leftover ? port->tx_leftover :
                                    ipc_router_packet_queue_get(&port->tx_queue);
      if(!packet)
      {
        break;
      }

      rc = smdl_writev(port->handle, (smdl_iovec_type *)packet, SMDL_WRITE_FLAG_LONG);
      if(rc == SMDL_CONTINUE || rc <= 0)
      {
        /* Could not fit the packet in the FIFO or an error was detected.
         * So re-try again later */
        break;
      }

      /* Packet written successfully */
      ipc_router_packet_free(&packet);
      port->tx_leftover = NULL;
    }
    ipc_router_os_sem_lock(&port->lock);
    port->tx_ref--;

  } while(port->tx_ref > 0);
  ipc_router_os_sem_unlock(&port->lock);
}

/*===========================================================================
FUNCTION ipc_router_xal_smdl_notify

DESCRIPTION
  Callback called by smd_lite when an event occurs.

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
static void ipc_router_xal_smdl_notify
(
  smdl_handle_type handle,
  smdl_event_type  event,
  void             *cb_data
)
{
  ipc_router_xal_smdl_port_type *port = (ipc_router_xal_smdl_port_type *)cb_data;

  if(!port)
  {
    return;
  }

  /* If notify callback is called before smdl_open returns */
  if(!port->handle)
  {
    port->handle = handle;
  }

  switch(event)
  {
    case SMDL_EVENT_OPEN:
      /* Start and post SSR (Restart) */
      if(port->task_handle)
      {
        ipc_router_xal_smdl_os_signal_worker(port, IPC_ROUTER_XAL_SMDL_REMOTE_RESUME);
      }
      else
      {
        ipc_router_xal_smdl_handle_remote_resume(port);
      }
      break;

    case SMDL_EVENT_READ:
      if(port->task_handle)
      {
        ipc_router_xal_smdl_os_signal_worker(port, IPC_ROUTER_XAL_SMDL_READ_WRITE);
      }
      else
      {
        ipc_router_xal_smdl_handle_rx(port);
      }
      break;

    case SMDL_EVENT_WRITE:
      if(port->task_handle)
      {
        ipc_router_xal_smdl_os_signal_worker(port, IPC_ROUTER_XAL_SMDL_READ_WRITE);
      }
      else
      {
        ipc_router_xal_smdl_handle_tx(port);
      }
      break;

    case SMDL_EVENT_CLOSE_COMPLETE:
      ipc_router_xal_closed(port->cb_handle);
      if(port->task_handle)
      {
        ipc_router_xal_smdl_os_deinit(port);
      }
      ipc_router_packet_queue_deinit(&port->tx_queue);
      ipc_router_os_sem_deinit(&port->lock);
      ipc_router_os_free(port);
      break;

    case SMDL_EVENT_REMOTE_CLOSE:
      /* Pre-SSR (Shutdown) */
      if(port->task_handle)
      {
        ipc_router_xal_smdl_os_signal_worker(port, IPC_ROUTER_XAL_SMDL_REMOTE_CLOSED);
      }
      else
      {
        ipc_router_xal_smdl_handle_remote_close(port);
      }
      break;
    default:
      break;
  }
}

/*===========================================================================
FUNCTION ipc_router_xal_smdl_open

DESCRIPTION
   Open the specified SMD port.

RETURN VALUE
   Xport handle or NULL on error

SIDE EFFECTS
===========================================================================*/
static int ipc_router_xal_smdl_open
(
 void *xport_params,
 void *cb_handle,
 void **out_handle
)
{
   ipc_router_xal_smdl_port_type *port;
   ipc_router_xal_smdl_param_type *param = 
     (ipc_router_xal_smdl_param_type *)xport_params;

   if(!param || !cb_handle || !out_handle)
   {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
   }

   port = (ipc_router_xal_smdl_port_type *)ipc_router_os_malloc(sizeof(*port));
   if (NULL == port)
   {
     return IPC_ROUTER_STATUS_NO_MEM;
   }
   *out_handle = (void *)port;

   ipc_router_os_mem_set(port, 0, sizeof(*port));

   port->cb_handle = cb_handle;
   port->rx_ref = 0;
   port->tx_ref = 0;
   port->tx_start = FALSE;
   port->rx_start = TRUE;

   ipc_router_os_sem_init(&port->lock);
   ipc_router_packet_queue_init(&port->tx_queue);

   /* We need to init the task only if the port requires a task */
   if(param->ist == FALSE && ipc_router_xal_smdl_os_init(port) != IPC_ROUTER_STATUS_SUCCESS)
   {
     goto os_init_bail;
   }

   port->handle = smdl_open(param->name, 
                            param->edge, 
                            param->mode,
                            param->fifo, 
                            ipc_router_xal_smdl_notify, 
                            port);

   if(!port->handle)
   {
     goto smdl_open_bail;
   }
   return IPC_ROUTER_STATUS_SUCCESS;

smdl_open_bail:
   if(param->ist == FALSE)
   {
       ipc_router_xal_smdl_os_deinit(port);
   }
os_init_bail:
  ipc_router_packet_queue_deinit(&port->tx_queue);
  ipc_router_os_sem_deinit(&port->lock);
  ipc_router_os_free(port);
  *out_handle = NULL;
  return IPC_ROUTER_STATUS_FAILURE;
}

/*===========================================================================
FUNCTION ipc_router_xal_smdl_close

DESCRIPTION
   Close the specified SMD port.

RETURN VALUE
   IPC_ROUTER_STATUS_SUCCESS
   IPC_ROUTER_STATUS_INVALID_PARAM
   IPC_ROUTER_STATUS_INTERRUPTED
   IPC_ROUTER_STATUS_TIMEOUT

SIDE EFFECTS
===========================================================================*/
static int ipc_router_xal_smdl_close
(
 void *handle
)
{
  ipc_router_xal_smdl_port_type *port = (ipc_router_xal_smdl_port_type *)handle;

  port->rx_start = FALSE;

  /* Finish cleanup when CLOSE_COMPLETE event is called */
  smdl_close(port->handle);

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION ipc_router_xal_smdl_write

DESCRIPTION
   Write data to the specified port.

   This is an asychronous call.  Once the function is invoked, the
   corresponding ipc_router_xal_tx_cb can happen at any time, even before
   this function returns.

RETURN VALUE
   IPC_ROUTER_STATUS_SUCCESS
   IPC_ROUTER_STATUS_INVALID_PARAM
   IPC_ROUTER_STATUS_INTERRUPTED

SIDE EFFECTS
===========================================================================*/
static int ipc_router_xal_smdl_write
(
 void *handle,
 ipc_router_packet_type *msg
 )
{
  ipc_router_xal_smdl_port_type *port = (ipc_router_xal_smdl_port_type *)handle;

  if (NULL == port)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  ipc_router_packet_queue_put(&port->tx_queue, msg);

  if(port->task_handle)
  {
    ipc_router_xal_smdl_os_signal_worker(port, IPC_ROUTER_XAL_SMDL_READ_WRITE);
  }
  else
  {
    ipc_router_xal_smdl_handle_tx(port);
  }
  
  return IPC_ROUTER_STATUS_SUCCESS;
}

ipc_router_xal_ops_type ipc_router_xal_smdl =
{
  ipc_router_xal_smdl_open,
  ipc_router_xal_smdl_close,
  ipc_router_xal_smdl_write,
};
