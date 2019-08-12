#ifndef _IPC_ROUTER_XAL_SMD_COMMON_H_
#define _IPC_ROUTER_XAL_SMD_COMMON_H_
/*===========================================================================

                      I P C    R O U T E R    X A L
                           S M D L  C O M M O N

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/ipc_router/src/ipc_router_xal_smdl_common.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/11   aep     First revision
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_packet.h"
#include "smd_lite.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

/** SMDL Specific port structur definitions */
typedef struct {
  void                         *task_handle;    /** OS Private task member */
  void                         *cb_handle;      /** Handle to call up the stack */
  boolean                      rx_start;        /** Flag indicating that rx open */
  boolean                      tx_start;        /** Flag indicating that tx open */
  uint32                       tx_ref;          /** Number of references to the TX logic */
  uint32                       rx_ref;          /** Number of references to the RX logic */
  smdl_handle_type             handle;          /** SMDL Handle returned by open */
  ipc_router_os_sem            lock;            /** Lock of this structure */
  ipc_router_packet_queue_type tx_queue;        /** TX Queue */
  ipc_router_packet_type       *rx_leftover;    /** Pending RX Packet */
  ipc_router_packet_type       *tx_leftover;    /** Pending TX Packet */
} ipc_router_xal_smdl_port_type;

/** Event types being signalled to the worker thread */
typedef enum
{
  IPC_ROUTER_XAL_SMDL_READ_WRITE,
  IPC_ROUTER_XAL_SMDL_REMOTE_CLOSED,
  IPC_ROUTER_XAL_SMDL_REMOTE_RESUME,
} ipc_router_xal_smdl_event_type;

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
int ipc_router_xal_smdl_os_init(ipc_router_xal_smdl_port_type *port);

/*===========================================================================
FUNCTION      ipc_router_xal_smdl_os_deinit

DESCRIPTION   De-initialize the OS specific elements associated with this port.

              port - The port structure for this SMDL interface.

RETURN VALUE  None

SIDE EFFECTS  The worker thread associated with this port might be killed.
===========================================================================*/
void ipc_router_xal_smdl_os_deinit(ipc_router_xal_smdl_port_type *port);

/*===========================================================================
FUNCTION      ipc_router_xal_smdl_os_signal_worker

DESCRIPTION   Signal the worker to handle an event

              port - The port structure for this SMDL interface.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_xal_smdl_os_signal_worker(ipc_router_xal_smdl_port_type *port, ipc_router_xal_smdl_event_type event);

/*===========================================================================
FUNCTION ipc_router_xal_smdl_handle_rx

DESCRIPTION
  Function called by the IPC Router's SMDL worker thread to handle RX

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_handle_rx(ipc_router_xal_smdl_port_type *port);

/*===========================================================================
FUNCTION ipc_router_xal_smdl_handle_tx

DESCRIPTION
  Function called by the IPC Router's SMDL worker thread to handle TX

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_handle_tx(ipc_router_xal_smdl_port_type *port);

/*===========================================================================
FUNCTION ipc_router_xal_smdl_handle_remote_close

DESCRIPTION
  Function called by the IPC Router's SMDL worker thread to handle remote
  close

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_handle_remote_close(ipc_router_xal_smdl_port_type *port);

/*===========================================================================
FUNCTION ipc_router_xal_smdl_handle_resume

DESCRIPTION
  Function called by the IPC Router's SMDL worker thread to handle resume

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_handle_remote_resume(ipc_router_xal_smdl_port_type *port);

#endif
