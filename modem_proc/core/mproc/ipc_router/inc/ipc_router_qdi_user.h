#ifndef IPC_ROUTER_QDI_USER_H_
#define IPC_ROUTER_QDI_USER_H_
/*===========================================================================

                  I P C    R O U T E R   Q D I   U S E R

   This file provides the stubs to access the IPC Router QDI Driver

  ---------------------------------------------------------------------------
  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/ipc_router/inc/ipc_router_qdi_user.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/12   aep     Initial Creation
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_types.h"
#include "ipc_router_core.h"


/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

typedef struct ipc_router_qdi_struct ipc_router_qdi_handle;

/* Buf has to be freed by the user 
 * addr has to be copied if it is needed after returning from this callback */
typedef void (*ipc_router_qdi_rx_cb_type)
(
  ipc_router_qdi_handle *handle,
  void *cb_data,
  void *buf,
  ipc_router_address_type *src_addr,
  uint32_t len,
  uint32_t confirm_rx
);

typedef void (*ipc_router_qdi_event_cb_type)
(
 ipc_router_qdi_handle      *handle,
 void                       *cb_data,
 ipc_router_event_type      event,
 ipc_router_event_info_type *info
);

/*===========================================================================
                          GLOBAL FUNCTIONS
===========================================================================*/

/* Init and de-init functions, once per process only */
void ipc_router_qdi_init(void);
void ipc_router_qdi_deinit(void);

int ipc_router_qdi_open
(
  ipc_router_qdi_handle **out_handle,
  uint32_t port_id,
  ipc_router_qdi_rx_cb_type rx_cb,
  void *rx_cb_data,
  ipc_router_qdi_event_cb_type event_cb,
  void *event_cb_data,
  ipc_router_core_options_type *options
);

void ipc_router_qdi_close
(
  ipc_router_qdi_handle *handle
);

int ipc_router_qdi_send
(
  ipc_router_qdi_handle *handle,
  ipc_router_address_type *dest_addr,
  void *buf,
  uint32_t len
);

int ipc_router_qdi_send_to_name
(
  ipc_router_qdi_handle *handle,
  ipc_router_service_type *service,
  void *buf,
  uint32_t len
);

int ipc_router_qdi_send_conf
(
  ipc_router_qdi_handle *handle,
  ipc_router_address_type *dest_addr
);


int ipc_router_qdi_reg_server
(
 ipc_router_qdi_handle *handle,
 ipc_router_service_type *service
);

int ipc_router_qdi_dereg_server
(
 ipc_router_qdi_handle *handle,
 ipc_router_service_type *service
);

int ipc_router_qdi_find_all_servers
(
  ipc_router_qdi_handle *handle,
  ipc_router_service_type *service,
  ipc_router_server_type *servers,
  uint32_t *num_entries,
  uint32_t *num_servers,
  uint32_t mask
);

void ipc_router_qdi_log
(
  uint32 desc,
  uint32 d1,
  uint32 d2,
  uint32 d3,
  uint32 d4,
  uint32 d5,
  uint32 d6
);

#endif
