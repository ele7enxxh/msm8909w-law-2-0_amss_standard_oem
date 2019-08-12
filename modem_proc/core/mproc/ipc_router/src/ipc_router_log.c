/*===========================================================================

                      I P C    R O U T E R    L O G

   This file describes the logging interface

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/ipc_router/src/ipc_router_log.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_os.h"
#include "ipc_router_types.h"
#include "ipc_router_log.h"
#include "ipc_router_packet.h"
#include "ipc_router_protocol.h"
#include "msg.h"
#include "smem_log.h"

size_t strlcpy(char *dst, const char *src, size_t siz);

/* IPC router events */
#define IPC_ROUTER_LOG_EVENT_ERROR      0x00

#define LOG_EVENT3(event, d1, d2, d3) \
  SMEM_LOG_EVENT(SMEM_LOG_IPC_ROUTER_EVENT_BASE | event, d1, d2, d3)

#define LOG_EVENT6(event, d1, d2, d3, d4, d5, d6) \
  SMEM_LOG_EVENT6(SMEM_LOG_IPC_ROUTER_EVENT_BASE | event, d1, d2, d3, \
      d4, d5, d6)

void ipc_router_log_ctrl
(
 ipc_router_log_dir dir,
 ipc_router_xport_type *xport,
 ipc_router_control_msg_body *cntl_msg
 )
{
  uint32 addr;
  uint32 type = cntl_msg->command;
  switch(type)
  {
    case IPC_ROUTER_CONTROL_NEW_SERVER:
    case IPC_ROUTER_CONTROL_REMOVE_SERVER:
      addr = (cntl_msg->arguments.server_arg.addr.processor_id << 24)
        | (cntl_msg->arguments.server_arg.addr.port_id & 0xffffff);
      LOG_EVENT3(dir | (type << 8), 
          addr, 
          cntl_msg->arguments.server_arg.service.type,
          cntl_msg->arguments.server_arg.service.instance);
      break;
    case IPC_ROUTER_CONTROL_REMOVE_CLIENT:
      LOG_EVENT3(dir | (type << 8), 
          cntl_msg->arguments.client_arg.processor_id,
          cntl_msg->arguments.client_arg.port_id, type);
      break;
    case IPC_ROUTER_CONTROL_RESUME_TX:
      LOG_EVENT3(dir | (type << 8), 
          cntl_msg->arguments.client_arg.processor_id,
          cntl_msg->arguments.client_arg.port_id, type);
      break;
    default:
      break;
  }
}
void ipc_router_log_packet
(
  ipc_router_log_dir dir,
  ipc_router_xport_type *xport,
  ipc_router_header_type *header
)
{
  uint32 desc = 0, name = 0;
  char *desc_str = xport && xport->desc ? xport->desc : "LOCL";
  char thread_name[16];

  qurt_thread_get_name(thread_name, 16);
  thread_name[15] = '\0';
  ipc_router_os_mem_copy(&name, sizeof(uint32), thread_name, sizeof(uint32));

  ipc_router_os_mem_copy(&desc, sizeof(uint32), desc_str, sizeof(uint32));
  
LOG_EVENT6(dir,
      (header->src_addr.processor_id << 24)  
      | (header->src_addr.port_id & 0xffffff),  /* 1 */
      (header->dest_addr.processor_id << 24)
      | (header->dest_addr.port_id & 0xffffff), /* 2 */
      (header->msg_type << 24) | (header->confirm_rx << 16) 
      | (header->msg_size & 0xffff),            /* 3 */
      desc,                                     /* 4 */
      (uint32)qurt_thread_get_id(),             /* 5 */
      name);
}

/*===========================================================================
FUNCTION      ipc_router_core_log_error

DESCRIPTION   Log runtime error

ARGUMENTS     filename - filename
              line - line number

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_log_error
(
 char *filename,
 unsigned int line
 )
{
  uint32 name[5];
  char *last;
  last = strrchr(filename, '/');
  if(!last)
    last = strrchr(filename, '\\');
  last = last ? (last+1) : filename;
  strlcpy((char *)name, last, sizeof(name));
  LOG_EVENT6(IPC_ROUTER_LOG_EVENT_ERROR, name[0], name[1], name[2], name[3], 
      name[4], line);
  MSG_2(MSG_SSID_ONCRPC, MSG_LEGACY_ERROR,
        "Runtime error. File 0x%s, Line: %d", filename, line);
}
