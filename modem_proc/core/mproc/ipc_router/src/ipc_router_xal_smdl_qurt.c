/*===========================================================================

              I P C    R O U T E R   X A L   S M D L   Q U R T

        This file provides QURT OS specific functionality of the smdl
                          XAL for the IPC Router

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/ipc_router/src/ipc_router_xal_smdl_qurt.c#1 $
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
#include "ipc_router_xal_smdl_common.h"
#include "qurt.h"

/*===========================================================================
                  CONSTANT / MACRO DECLARATIONS
===========================================================================*/

/** The worker thread's stack size */
#define IPC_ROUTER_XAL_SMDL_STACK_SIZE (1024*3)

/** The signal the worker thread waits on */
#define IPC_ROUTER_XAL_WORKER_SIGNAL_READ_WRITE    (0x00010000)
#define IPC_ROUTER_XAL_WORKER_SIGNAL_REMOTE_CLOSED (0x00020000)
#define IPC_ROUTER_XAL_WORKER_SIGNAL_REMOTE_RESUME (0x00040000)
#define IPC_ROUTER_XAL_WORKER_SIGNAL_KILL          (0x00080000)

#define IPC_ROUTER_XAL_WORKER_SIGNAL (\
    IPC_ROUTER_XAL_WORKER_SIGNAL_READ_WRITE |\
    IPC_ROUTER_XAL_WORKER_SIGNAL_REMOTE_CLOSED |\
    IPC_ROUTER_XAL_WORKER_SIGNAL_REMOTE_RESUME|\
    IPC_ROUTER_XAL_WORKER_SIGNAL_KILL)

/*===========================================================================
                        PRIVATE TYPE DEFINITIONS 
===========================================================================*/

/** The OS specific private structure of the smdl port */
typedef struct {
  qurt_thread_t           tid;        /* TID Of the worker thread */
  qurt_thread_attr_t      thread_attr;/* Thread attributes */
  unsigned int            stack_size; /* Stack size of worker thread */
  void                    *stack_addr;/* Stack address */
  qurt_anysignal_t        signal;     /* SIgnal used by worker  */
} ipc_router_xal_smdl_os_port_type;

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_xal_smdl_worker_thread

DESCRIPTION   The Worker thread dedicated to handle RX and TX events
              on this SMDL port.

              priv - Private parameter (port)

RETURN VALUE  None

SIDE EFFECTS  This function never returns.
===========================================================================*/
static void ipc_router_xal_smdl_worker_thread(void *priv)
{
  ipc_router_xal_smdl_port_type *port = (ipc_router_xal_smdl_port_type *)priv;
  ipc_router_xal_smdl_os_port_type *os_priv = (ipc_router_xal_smdl_os_port_type *)port->task_handle;
  unsigned int got_sigs;

  while(1)
  {
    qurt_anysignal_wait(&os_priv->signal, IPC_ROUTER_XAL_WORKER_SIGNAL);
    got_sigs = qurt_anysignal_get(&os_priv->signal);
    qurt_anysignal_clear(&os_priv->signal, got_sigs);

    if(got_sigs & IPC_ROUTER_XAL_WORKER_SIGNAL_KILL)
    {
      break;
    }
    if(got_sigs & IPC_ROUTER_XAL_WORKER_SIGNAL_REMOTE_CLOSED)
    {
      ipc_router_xal_smdl_handle_remote_close(port);
    }
    if(got_sigs & IPC_ROUTER_XAL_WORKER_SIGNAL_REMOTE_RESUME)
    {
      ipc_router_xal_smdl_handle_remote_resume(port);
    }
    if(got_sigs & IPC_ROUTER_XAL_WORKER_SIGNAL_READ_WRITE)
    {
      ipc_router_xal_smdl_handle_rx(port);
      ipc_router_xal_smdl_handle_tx(port);
    }
  }
  qurt_thread_exit(0);
}

/*===========================================================================
                        EXPORTED FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_xal_smdl_os_init

DESCRIPTION   Initializes the OS Private structure and spawns a worker 
              thread to handle this port.

              port - The port structure for this SMDL interface.

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success,
              error code on failure.
===========================================================================*/
int ipc_router_xal_smdl_os_init(ipc_router_xal_smdl_port_type *port)
{
  ipc_router_xal_smdl_os_port_type *os_priv;

  port->task_handle = os_priv = ipc_router_os_malloc(sizeof(ipc_router_xal_smdl_os_port_type));
  if(!os_priv)
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }

  ipc_router_os_mem_set(os_priv, 0, sizeof(ipc_router_xal_smdl_os_port_type));

  os_priv->stack_addr = ipc_router_os_malloc(IPC_ROUTER_XAL_SMDL_STACK_SIZE);
  if(!os_priv->stack_addr)
  {
    port->task_handle = NULL;
    ipc_router_os_free(os_priv);
    return IPC_ROUTER_STATUS_NO_MEM;
  }
  os_priv->stack_size = IPC_ROUTER_XAL_SMDL_STACK_SIZE;

  qurt_thread_attr_init(&os_priv->thread_attr);
  qurt_thread_attr_set_name(&os_priv->thread_attr, "IPCSMDL_W");
  qurt_thread_attr_set_priority(&os_priv->thread_attr, 204);
  qurt_thread_attr_set_stack_size(&os_priv->thread_attr, os_priv->stack_size);
  qurt_thread_attr_set_stack_addr(&os_priv->thread_attr, os_priv->stack_addr);

  qurt_anysignal_init(&os_priv->signal);

  if(qurt_thread_create(&os_priv->tid, &os_priv->thread_attr, 
      ipc_router_xal_smdl_worker_thread, (void *)port) != 0)
  {
    ipc_router_os_free(os_priv->stack_addr);
    ipc_router_os_free(os_priv);
    port->task_handle = NULL;
    return IPC_ROUTER_STATUS_FAILURE;
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}


/*===========================================================================
FUNCTION      ipc_router_xal_smdl_os_deinit

DESCRIPTION   De-initialize the OS specific elements associated with this port.

              port - The port structure for this SMDL interface.

RETURN VALUE  None

SIDE EFFECTS  The worker thread associated with this port might be killed.
===========================================================================*/
void ipc_router_xal_smdl_os_deinit(ipc_router_xal_smdl_port_type *port)
{
  int status;
  ipc_router_xal_smdl_os_port_type *os_port;
  if(!port)
  {
    return;
  }
  os_port = (ipc_router_xal_smdl_os_port_type *)port->task_handle;
  if(!os_port)
  {
    return;
  }
  qurt_anysignal_set(&os_port->signal, IPC_ROUTER_XAL_WORKER_SIGNAL_KILL);
  qurt_thread_join(os_port->tid, &status);

  qurt_anysignal_destroy(&os_port->signal);
  ipc_router_os_free((void *)os_port->stack_addr);
  ipc_router_os_free(os_port);
  port->task_handle = NULL;
}

/*===========================================================================
FUNCTION      ipc_router_xal_smdl_os_signal_worker

DESCRIPTION   Signal the worker to handle an event

              port - The port structure for this SMDL interface.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_xal_smdl_os_signal_worker(ipc_router_xal_smdl_port_type *port, ipc_router_xal_smdl_event_type event)
{
  ipc_router_xal_smdl_os_port_type *os = (ipc_router_xal_smdl_os_port_type *)port->task_handle;

  switch(event)
  {
    case IPC_ROUTER_XAL_SMDL_READ_WRITE:
      qurt_anysignal_set(&os->signal, IPC_ROUTER_XAL_WORKER_SIGNAL_READ_WRITE);
      break;
    case IPC_ROUTER_XAL_SMDL_REMOTE_CLOSED:
      qurt_anysignal_set(&os->signal, IPC_ROUTER_XAL_WORKER_SIGNAL_REMOTE_CLOSED);
      break;
    case IPC_ROUTER_XAL_SMDL_REMOTE_RESUME:
      qurt_anysignal_set(&os->signal, IPC_ROUTER_XAL_WORKER_SIGNAL_REMOTE_RESUME);
      break;
  }
}

