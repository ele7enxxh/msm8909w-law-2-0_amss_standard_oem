#ifndef PS_SOCKET_EVENT_DEFS_H
#define PS_SOCKET_EVENT_DEFS_H

/*===========================================================================

          S O C K E T S  I N T E R N A L  H E A D E R  F I L E

DESCRIPTION

 The Data Services Internal Socket Header File. Contains shared variables and
 enums, as well as declarations for functions for each protocol.


Copyright (c) 1998-2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/inc/ps_socket_event_defs.h#1 $
  $DateTime: 2007/06/08 12:26:28

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


#include "ps_socki_defs.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  PS_SOCKET_EVENT_MIN                 = 0,
  PS_SOCKET_TCP_CONNECTED_EV          = PS_SOCKET_EVENT_MIN,
  PS_SOCKET_PCB_CLOSED_EV             = PS_SOCKET_EVENT_MIN + 1,
  PS_SOCKET_ICMP_ERROR_EV             = PS_SOCKET_EVENT_MIN + 2,
  PS_SOCKET_DSM_ITEM_NOT_AVAILABLE_EV = PS_SOCKET_EVENT_MIN + 3,
  PS_SOCKET_DSM_ITEM_AVAILABLE_EV     = PS_SOCKET_EVENT_MIN + 4,
  PS_SOCKET_UDP_CONNECTED_EV          = PS_SOCKET_EVENT_MIN + 5,
  PS_SOCKET_EVENT_MAX
} ps_socket_event_enum_type;

typedef struct
{
  void *                    scb_ptr;

  union
  {
    struct
    {
      struct  connection    conn;
      int32                 options_mask;
      uint32                mss;
    } tcp_connected_event;

    struct  connection      conn;
    uint8                   pcb_closed_reason;
    int16                   icmp_error;

  } data;
} ps_socket_event_info_type;


typedef void (*socket_platform_event_cback_f_ptr_type)
(
  ps_socket_event_enum_type    event_name,
  ps_socket_event_info_type *  event_info_ptr
);

extern socket_platform_event_cback_f_ptr_type  socket_platform_event_cback_f_ptr;


#endif /* PS_SOCKET_EVENT_DEFS_H */
