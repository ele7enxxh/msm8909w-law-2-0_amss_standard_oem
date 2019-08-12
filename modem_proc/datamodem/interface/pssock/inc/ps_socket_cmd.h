#ifndef PS_SOCKET_CMD_H
#define PS_SOCKET_CMD_H

/*===========================================================================

          S O C K E T S  I N T E R N A L  H E A D E R  F I L E

DESCRIPTION

 The Data Services Internal Socket Header File. Contains shared variables and
 enums, as well as declarations for functions for each protocol.


Copyright (c) 1998 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/inc/ps_socket_cmd.h#1 $
  $DateTime: 2007/06/08 12:26:28

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "queue.h"
#include "ps_in.h"
#include "ps_route_scope.h"
#include "ps_rt_meta_info.h"
#include "ps_pkt_meta_info.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*--------------------------------------------------------------------------
  Enum for types of PS commands.
--------------------------------------------------------------------------*/
typedef enum
{
  PS_SOCKET_CMD_MIN_VAL             = 0,
  PS_SOCKET_CMD_CLOSE               = 1,
  PS_SOCKET_CMD_FLOW_ENABLED_IND    = 2,
  PS_SOCKET_CMD_SET_ROUTE_META_INFO = 3,
  PS_SOCKET_CMD_SET_ROUTE_SCOPE     = 4,
  PS_SOCKET_CMD_HIGH_PRIORITY       = 5,
  PS_SOCKET_CMD_PAYLOAD             = PS_SOCKET_CMD_HIGH_PRIORITY,
  PS_SOCKET_CMD_CONNECT             = 6,
  PS_SOCKET_CMD_SHUTDOWN            = 7,
  PS_SOCKET_CMD_LISTEN              = 8,
  PS_SOCKET_CMD_SETOPT              = 9,
  PS_SOCKET_CMD_BIND                = 10,
  PS_SOCKET_CMD_LINGER_RESET        = 11,
  PS_SOCKET_CMD_MAX_VAL
} ps_socket_cmd_kind_type;

/*---------------------------------------------------------------------------
  PS command

  Since lifetime of PCB is not managed using refCnt, it could be deleted
  while platform had enqueued commands on that PCB. To make sure that these
  commands are not executed if PCB is allocated again, sockfd is sent along
  with each command. ProtocolStack ignores the command if PCB's sockfd is not
  same as the sockfd in the command info.
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type                  link;
  ps_socket_cmd_kind_type      kind;
  int32                        sockfd;
  int32                        pcb_handle;
  int32                        protocol;

  union
  {
    ps_rt_meta_info_type *     rt_meta_info_ptr;
    ps_route_scope_type        route_scope;
    struct ps_sockaddr_in6     dest_addr;
    struct ps_sockaddr_in6     local_addr;

    struct
    {
      ps_pkt_meta_info_type *  pkt_meta_info_ptr;
      struct ps_sockaddr_in6   dest_addr;
      int32                    length;
    } write_data;

    struct
    {
      int32                    backlog;
      int32                    rcv_buf;
    } listen_data;

    struct
    {
      int32                    level;
      int32                    name;
      int32                    val;
      int32                    len;
    } sock_opt_data;

    struct
    {
      boolean                  is_linger_on;
      int32                    time_in_sec;
    } linger_reset_data;

    int32                      shutdown_dir;
    boolean                    silent_close_flag;
  } user_data;

} ps_socket_cmd_type;

typedef void (*ps_socket_cmd_out_of_mem_cback_f_ptr_type)
(
  void
);


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
ps_socket_cmd_type * ps_socket_cmd_alloc
(
  ps_socket_cmd_kind_type  kind
);

void ps_socket_cmd_send
(
  ps_socket_cmd_type **   ps_cmd_buf_ptr_ptr,
  int32                   platform_sock_handle,
  int32                   pcb_handle,
  int32                   protocol
);

void ps_socket_cmd_free
(
  ps_socket_cmd_type **   ps_cmd_buf_ptr_ptr
);

void ps_socket_cmd_reg_out_of_mem_cback
(
  ps_socket_cmd_out_of_mem_cback_f_ptr_type  out_of_mem_cback_f_ptr
);

#ifdef __cplusplus
}
#endif

#endif /* PS_SOCKET_CMD_H */
