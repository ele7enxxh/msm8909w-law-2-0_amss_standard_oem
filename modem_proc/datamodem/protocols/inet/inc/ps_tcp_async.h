#ifndef PS_TCP_ASYNC_H
#define PS_TCP_ASYNC_H
/*===========================================================================

  @file PS_TCP_ASYNC.h

  This file extends implementation of SocketIO abstract class to support a
  data pipe which doesn't maintain message boundaries. This class is used by
  TCP sockets.
 
  The Class Definitions section is applicable only to C++ code and should be
  removed for C.

  Copyright (c) 2010 - 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================

===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpsubr.c_v   1.0   08 Aug 2002 11:19:52   akhare  $
  $Header $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
08/29/11   dvk     Fixed KW error.
02/24/11   ttv     Fixed compilation warnings.
08/27/10   sn      Fixed compilation error.
07/26/10   ps      Created the module.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "target.h"

#ifdef FEATURE_DATA_IS707_ASYNC
#include "ps_socki_defs.h"
#include "ps_in.h"
#include "dsm.h"


/*===========================================================================
                        MACROS, TYPEDEFS AND VARIABLES
===========================================================================*/

typedef enum
{
  ASYNC_TCP_ESTABLISHED,
  ASYNC_TCP_CLOSE_WAIT,
  ASYNC_TCP_CLOSED
} async_tcp_state_async_type;


typedef struct
{

  /*---------------------------------------------------------------------
    This function will be invoked by the TCP input routine upon completion of TCP processing 
    of received payload data bytes.This function will send the TCP payload for 617 and telnet
    processing and then output to the serial port.
  -------------------------------------------------------------------------*/
  void( *r_upcall ) ( dsm_item_type*, uint16 );

  /*-------------------------------------------------------------------------
  This function will be invoked by the TCP input routine when the TCP state changes. 
  This function will use the old and the new TCP states to determine the processing 
  requirements
  ---------------------------------------------------------------------------*/
  void(*s_upcall) ( async_tcp_state_async_type );

  /*---------------------------------------------------------------------------
  This function will be invoked by the TCP input routine when the TCP TX window changes to 
  allow more bytes to be sent. This function will determine if the Lowater mark has been 
  reached for the appropriate Watermark item and invoke the Lowater function if appropriate.
  ----------------------------------------------------------------------------*/
  void (*t_upcall) (void);
 
} ps_async_func_tbl_type; 


/*===========================================================================

                  C Wrappers For C++ Interface Methods

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
FUNCTION PS_TCP_ASYNC_CREATE_HANDLE()

DESCRIPTION
  This function will setup TCB and will perform necessary initializations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_tcp_async_create_handle
( 
  ps_async_func_tbl_type *func_table,
  ps_iface_type *iface_ptr
);

/*===========================================================================
FUNCTION PS_TCP_ASYNC_RESET_TCB()

DESCRIPTION
  This function performs TCB cleanup operations after the end of an IS-707 
  async data call. It gets called either upon expiration of a timer when TCP 
  closes gracefully or it's explicitly called when DS asks PS to abort protocols
  forcibly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  ps_tcp_async_reset_tcp 
( 
  int32 client_handle
);

/*===========================================================================
FUNCTION PS_TCP_ASYNC_SEND_DATA()

DESCRIPTION
  This function will queue the passed data for TXing on the SendQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_tcp_async_send_data
(
  int32          client_handle,
  dsm_item_type *new_item_ptr
);

/*===========================================================================
FUNCTION PS_TCP_OUTSTANDING_DATA()

DESCRIPTION
  This is a utility function to return the net outstanding data on SenQ, 
  this includes un-acknowledged seq# as well.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 ps_tcp_async_outstanding_data
(
  void
);

/*===========================================================================
FUNCTION PS_TCP_ASYNC_CLOSE_TCP()

DESCRIPTION
  This is an API exposed to Async module to close the associated TCP 
  connection. This really means "I have no more data to send". It only 
  closes the connection in one direction, and we can continue to receive 
  data indefinitely.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_tcp_async_close_tcp 
(
  int32 client_handle
);

/*===========================================================================
FUNCTION PS_TCP_ASYNC_TCP_OUTPUT()

DESCRIPTION
  This is an API exposed to Async module to send a TCP segment on the
  associated connection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_tcp_async_tcp_output 
(
  int32 client_handle
);

/*===========================================================================
FUNCTION PS_TCP_ASYNC_RECIEVE_DATA()

DESCRIPTION
  This function will notify the reader for the availibility
  of data on the RecieveQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_tcp_async_recieve_data
( 
  int32 client_handle
);

/*===========================================================================
FUNCTION PS_TCP_ASYNC_TCP_OPEN

DESCRIPTION
  This is an API exposed to Async module and will Open a TCP connection 
  using the specified parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  ps_tcp_async_tcp_open
(
  int32   client_handle,
  struct connection *ps707_async_connection,
  word    mode,
  uint16  rwindow,
  uint16  mss
);

/*===========================================================================
FUNCTION PS_TCP_ASYNC_GET_REMOTE_ADDRESS

DESCRIPTION
  This is an API exposed to Async module and will Open a TCP connection 
  using the specified parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
struct ps_in6_addr ps_tcp_async_get_remote_address
(
  void
);

/*===========================================================================
FUNCTION PS_TCP_ASYNC_GET_LOCAL_ADDRESS

DESCRIPTION
  This is an API exposed to Async module and will Open a TCP connection 
  using the specified parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
struct ps_in6_addr ps_tcp_async_get_local_address
(
  void
);

/*===========================================================================
FUNCTION PS_TCP_ASYNC_GET_TCB_HANDLE

DESCRIPTION
  This is an API exposed to Async module and will Open a TCP connection 
  using the specified parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void* ps_tcp_async_get_tcb_handle
(
  void
);

/*===========================================================================
FUNCTION PS_TCP_ASYNC_STATE_CHANGE_UPCALL()

DESCRIPTION
  This function is an wraper and will be invoked by the TCP input 
  routine when the TCP state changes.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_tcp_async_state_change_upcall 
(
 async_tcp_state_async_type state
);

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_DATA_IS707_ASYNC */
#endif  /* PS_TCP_ASYNC_H */
