#ifndef IPC_ROUTER_XAL_SMDL_H
#define IPC_ROUTER_XAL_SMDL_H
/*===========================================================================
                      I P C    R O U T E R    X A L    S M D L

DESCRIPTION
   This file specifies the interface of an OS independent implementation of
   the SMD Lite component of Transport Abstraction Layer for the IPC router.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/ipc_router/src/ipc_router_xal_smdl.h#1 $
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/


/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "smd_lite.h"
#include "ipc_router_xal.h"

/*===========================================================================
                        EXPORTED FUNCTION PROTOTYPES
===========================================================================*/
/* Default port name */
#define IPC_ROUTER_XAL_SMDL_DEFAULT_NAME "IPCRTR"

/* Default SMD Edge */
#define IPC_ROUTER_XAL_SMDL_DEFAULT_EDGE SMD_APPS_MODEM

/* Default FIFO Size */
#define IPC_ROUTER_XAL_SMDL_DEFAULT_FIFO SMD_STANDARD_FIFO

/* Default open flags Currently only packet mode is supported */
#define IPC_ROUTER_XAL_SMDL_DEFAULT_MODE SMDL_OPEN_FLAGS_MODE_PACKET

/* Default taskless mode */
#define IPC_ROUTER_XAL_SMDL_DEFAULT_TASK FALSE

/* If only default parameters are required, then this can be used */
#define IPC_ROUTER_XAL_SMDL_DEFAULT_PARAM {	\
  IPC_ROUTER_XAL_SMDL_DEFAULT_NAME,		\
  IPC_ROUTER_XAL_SMDL_DEFAULT_EDGE,		\
  IPC_ROUTER_XAL_SMDL_DEFAULT_FIFO,		\
  IPC_ROUTER_XAL_SMDL_DEFAULT_MODE,		\
  IPC_ROUTER_XAL_SMDL_DEFAULT_TASK    \
}

/** Parameter structure which can be used to tweak the smd lite interface  */
typedef struct {
  char            *name;  /** Name of the smd lite port */
  smd_channel_type edge;  /** The edge to be used */
  uint32           fifo;  /** Required FIFO Size */
  uint32           mode;  /** Mode and other open flags */
  boolean          ist;   /** TRUE of this port operates without a task */
} ipc_router_xal_smdl_param_type;

extern ipc_router_xal_ops_type ipc_router_xal_smdl;

#endif /* IPC_ROUTER_XAL_SMDL_H */
