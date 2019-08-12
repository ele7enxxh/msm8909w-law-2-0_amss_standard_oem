#ifndef PS_DNSI_IO_MGR_H
#define PS_DNSI_IO_MGR_H
/*===========================================================================

              D N S   I O   M A N A G E R   H E A D E R   F I L E

DESCRIPTION

  The Data Services DNS Subsystem IO manager header file. Contains
  declarations of functions, variables, macros, structs and enums
  used by IO manager. This module is internal to the DNS subsystem.

EXTERNALIZED FUNCTIONS

  ps_dnsi_io_mgr_create_instance()
    Creates an instance of the IO manager.

  ps_dnsi_io_mgr_send_query()
    Sends query to the DNS server in a specified instance, waits for the
    reply and notifies client after receiving the reply.

  ps_dnsi_io_mgr_delete_instance()
    Deletes an instance of IO manager. Closes corresponding socket and
    frees the IO manager control block.


INTIALIZATIONS AND SEQUENCING REQUIREMENTS
  This module should be run only in PS task context.

Copyright (c) 2007-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"


#include "dsm.h"
#include "dssocket.h"



/*===========================================================================

                                DATA STRUCTURES

===========================================================================*/
typedef   int32   ps_dnsi_io_mgr_handle_type;
#define  PS_DNSI_IO_MGR_INVALID_HANDLE  (-1)

/*---------------------------------------------------------------------------
  Types of events that the IO manager can notify its client.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_DNSI_IO_MGR_READ_EVENT  = 0,
  PS_DNSI_IO_MGR_ERROR_EVENT = 1
} ps_dnsi_io_mgr_event_enum_type;

/*===========================================================================
TYPEDEF FUNCTION PS_DNSI_IO_MGR_NOTIFY_F_TYPE

DESCRIPTION
  This is the function notifies the client of the IO manager of events.
  The IO manager calls this function when the query operations are complete
  or when there is some sort of error.

  DNS response is freed once the callback is returned. Client needs to dup
  the DSM item if it needs a copy

PARAMETERS
  io_mgr_handle    - This is the handle which was returned to the client
                     after call to ps_dnsi_io_mgr_send_query()
  event            - Type of event that occurred. Currently only two events
                     are reported, READ_EVENT and ERROR_EVENT. When READ_EVENT
                     is notified, the dsm_reply_ptr must contain results of the
                     read operation.
  dns_response_ptr - Pointer to the reply message. This field is valid when the
                     event being notified is READ_EVENT. If any error occurs,
                     then this field is set to NULL.
  cback_f_ptr      - Client's callback notification function.
  user_data_ptr    - Callers original user data pointer
  errno            - Error code. Meaningful if event is ERROR_EVENT.

DEPENDENCIES
  None

RETURN VALUE
  None but on error, the parameter error contains the error code

  errno values
  ------------
  DS_EHOSTNOTFOUND
  DS_ETRYAGAIN
  DS_ENOMEM
  DS_ENORECOVERY
  DS_ESYSTEM

SIDE EFFECTS
  None
===========================================================================*/
typedef void (*ps_dnsi_io_mgr_notify_f_type)
(
  ps_dnsi_io_mgr_handle_type        io_mgr_handle,
  ps_dnsi_io_mgr_event_enum_type    io_mgr_event,
  dsm_item_type                   * dns_response_ptr,
  void                            * user_data_ptr,
  int16                             ps_errno
);

/*--------------------------------------------------------------------------
  IO manager control block.

  io_mgr_handle     - Control block Identifier
  dns_query_ptr     - Pointer to DSM item pointer.
  net_policy        - Network policy associated with this IO manager.
  cback_f_ptr       - Client's callback notification function. This function
                      is invoked only in case of error or successful read. If
                      the dsm_item_ptr returned in the callback is NULL, then it
                      notifies the client of an irrecoverable error condition.
  user_data_ptr     - Client's callback notification user data.
  server_sockaddr   - Pointer to DNS server ps_sockaddr.
  sockfd            - Socket descriptor. Required to handle EWOULDBLOCK recvd
                      after socket read and writes.
  is_registered_to_filters - Flag that indicates if the socket was registered
                             to filters, thus deregistration is needed.
---------------------------------------------------------------------------*/
typedef struct ps_dnsi_io_mgr_cb_s_type
{
  ps_dnsi_io_mgr_handle_type      io_mgr_handle;
  dsm_item_type                 * dns_query_ptr;
  int16                           sockfd;
  dss_net_policy_info_type        net_policy;
  ps_dnsi_io_mgr_notify_f_type    cback_f_ptr;
  void                          * user_data_ptr;
  struct ps_sockaddr_storage      server_sockaddr;
  uint32                          sock_event_mask;
  dss_iface_id_type               iface_id;
  boolean                         is_registered_to_filters;
  struct ps_sockaddr_storage      *p_pending_servers;
  uint32                          max_pending_servers_index;
} ps_dnsi_io_mgr_cb_type;


/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION  PS_DNSI_IO_MGR_INIT()

DESCRIPTION
  Initialize the DNS IO manager during powerup.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_dnsi_io_mgr_init
(
  void
);

/*===========================================================================
FUNCTION  PS_DNSI_IO_MGR_CREATE_INSTANCE()

DESCRIPTION
  Creates an instance of the IO manager. Allocates and fills the IO manager
  control block and creates socket to be used to send messages to DNS server.

PARAMETERS
  family          - Address family (DSS_AF_INET/DSS_AF_INET6) to be 
                    used for this IO manager.
  net_policy_ptr  - Network policy to be used for this IO manager.
  cback_f_ptr     - Client callback function.
  user_data_ptr   - Callback user data (Optional - Can be null)
  errno           - Error code returned in case of error.

RETURN VALUE
  On success, returns a handle to the IO manager control block.
  On Failure, returns PS_DNSI_IO_MGR_INVALID_HANDLE and sets the error
  code in errno.

  errno values
  ------------
  DS_ENOMEM       - No memory for allocating control block.
  DS_SOCKERROR    - Socket subsystem error.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_dnsi_io_mgr_handle_type  ps_dnsi_io_mgr_create_instance
(
  uint16                              family,
  dss_net_policy_info_type          * net_policy_ptr,
  ps_dnsi_io_mgr_notify_f_type        cback_f_ptr,
  void                              * user_data_ptr,
  int16                             * ps_errno
);

/*===========================================================================
FUNCTION  PS_DNSI_IO_MGR_SEND_QUERY()

DESCRIPTION
  Sends a query to the address specified by server_address. When the
  operation is complete, calls the registered callback function to return
  the results. The caller must register a callback function.

PARAMETERS
  io_mgr_handle   - Handle identifying the IO manager control block.
  dns_query_ptr   - Pointer to the DSM item chain to be sent.
  server_sa_addr  - Address of the server where the query should be sent.
  errno           - Error code returned in case of error.

RETURN VALUE
  Returns DSS_ERROR on error and sets the errno to the error code. If the
  error code is DS_EWOULDBLOCK, it indicates that the results will be
  returned through callback function.

  errno values
  ------------
  DS_EWOULDBLOCK  Operation would block.
  DS_EFAULT       Invalid argument
  DS_ESOCKERROR   Error creating socket

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int16 ps_dnsi_io_mgr_send_query
(
  ps_dnsi_io_mgr_handle_type    io_mgr_handle,
  dsm_item_type               * dns_query_ptr,
  struct ps_sockaddr          * server_sa_ptr,
  int16                       * ps_errno
);

/*===========================================================================
FUNCTION  PS_DNSI_IO_MGR_DELETE_INSTANCE()

DESCRIPTION
  This function is called when the client of IO manager times out in waiting
  for a reply or the client is through with all the queries and wants to free
  the IO manager. This function cleans up the IO manager control block, closes
  the sockets.

PARAMETERS
  io_mgr_handle - Handle to identify the IO manager control block. This was
                  returned to the client from create instance.
  errno         - Error code incase of error.

RETURN VALUE
  Returns DSS_SUCCESS in case of success.
  Returns DSS_ERROR on error and sets errno to the error code.

  errno values
  ------------
  DS_EFAULT - if invalid/already cleaned IO manager handle is given as argument

DEPENDENCIES
  None

SIDE EFFECTS
  Aborts all pending operations initiated by ps_dnsi_io_mgr_send_query().
===========================================================================*/
int16 ps_dnsi_io_mgr_delete_instance
(
  ps_dnsi_io_mgr_handle_type    io_mgr_handle,
  int16                       * ps_errno
);
/*===========================================================================
FUNCTION PS_DNSI_IO_MGRI_IS_HANDLE_VALID

DESCRIPTION
  Checks if the specified handle is valid.

PARAMETERS
  io_mgr_handle  - IO mgr control block handle

RETURN VALUE
  TRUE if the specified handle is valid.
  FALSE if the specified handle is not valid.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
ps_dnsi_io_mgr_cb_type * ps_dnsi_io_mgri_is_handle_valid
(
  ps_dnsi_io_mgr_handle_type  io_mgr_handle
);

#endif  /* PS_DNSI_IO_MGR_H */
