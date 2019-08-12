#ifndef DSSTCP_ASYNC_H
#define DSSTCP_ASYNC_H
/*===========================================================================

  @file DSSTCP_ASYNC.h

  This file extends implementation of SocketIO abstract class to support a
  data pipe which doesn't maintain message boundaries. This class is used by
  TCP sockets.
 
  The Class Definitions section is applicable only to C++ code and should be
  removed for C.

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================

===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpsubr.c_v   1.0   08 Aug 2002 11:19:52   akhare  $
  $Header $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
02/23/11   ttv     Fixed compilation warnings.
07/26/10   ps      created the module.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "target.h"

#ifdef FEATURE_DATA_IS707_ASYNC
#include "ps_tcp_hdr.h"
#include "dsstcp.h"


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================
FUNCTION DSSTCP_SOCKET()

DESCRIPTION
  This function is the protocol-specific socket function.  For TCP, it
  just sets the socket state to INIT.

  This function is called from the context of the application task.

DEPENDENCIES
  None

RETURN VALUE
  On success, returns DSS_SUCCESS
  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EPROTOTYPE       specified protocol invalid for socket type
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dsstcp_create_async_socket
(
  int32      sockfd,
  int32   *  pcb_handle_ptr,
  void    *  reader_ptr,
  void    *  writer_ptr,
  int16   *  errno_ptr
);

/*===========================================================================
FUNCTION DSSTCP_ASYNC_WRITE()

DESCRIPTION
  Sends dsm memory item out to TCP.
  This function is called from the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  Uses return values from send_tcp().  On error, the function returns -1.
  Otherwise, it returns the number of bytes appended to TCP send queue.

SIDE EFFECTS
  None.

===========================================================================*/
void dsstcp_async_write
(
  int32                     pcb_handle,
  int32                     sockfd,
  int32                     pkt_length,
  struct ps_sockaddr_in6  * ta_ptr,
  ps_pkt_meta_info_type   * pkt_meta_info_ptr
);

/*===========================================================================
FUNCTION DSSTCP_CLEANUP_ALL_RESEQ()

DESCRIPTION
  This function cleans up the resequencing queue of all the TCBs. This
  fucntion should be called to free up memory potentially consumed by the
  segments lying in the resequencing queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

struct ps_in6_addr dsstcp_async_get_remote_address
(
  int32 pcb_handle
);

/*===========================================================================
FUNCTION DSSTCP_CLEANUP_ALL_RESEQ()

DESCRIPTION
  This function cleans up the resequencing queue of all the TCBs. This
  fucntion should be called to free up memory potentially consumed by the
  segments lying in the resequencing queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

struct ps_in6_addr dsstcp_async_get_local_address
(
  int32 pcb_handle
);

/*===========================================================================
FUNCTION DSSTCP_CLEANUP_ALL_RESEQ()

DESCRIPTION
  This function cleans up the resequencing queue of all the TCBs. This
  fucntion should be called to free up memory potentially consumed by the
  segments lying in the resequencing queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

uint32 dsstcp_async_outstanding_data 
(
  int32 pcb_handle
);

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_DATA_IS707_ASYNC */
#endif /* DSSTCP_ASYNC_H */

