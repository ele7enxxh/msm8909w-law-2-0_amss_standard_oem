#ifndef RTE_IPC_H
#define RTE_IPC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

   ipc_new()        - Allocate a new message
   ipc_delete()     - Free an existing message
   ipc_send()       - Send an allocated message
   ipc_receive()    - Receive a new message
   ipc_sender()     - Identifies the message sender
   ipc_msg_size()   - Get the original requested message size


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rte/rte_ipc.h#1 $
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/06    rv     Fixed warnings
01/20/06    gr     Replaced errno.h with IxErrno.h to avoid conflicts with
                   system files.
03/10/13    et     Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "IxErrno.h"
#include "ipctypes.h"


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/
/*
** These macro translate a call such as:
**   msg_ptr = ipc_new(xxxbbbapi_msgtwo_msg, IPCDEF_MSG_POOL_PUBLIC);
**   and
**   msg_ptr = ipc_new(xxxbbbapi_msgtwo_msg, 10, IPCDEF_MSG_POOL_PUBLIC);
**
** into
** 
**   msg_ptr = (xxxbbbapi_msgtwo_msg*)__ipc_new( sizeof(xxxbbbapi_msgtwo_msg),
**                                               xxxbbbapi_msgtwo_msg_id,
**                                               IPCDEF_MSG_POOL_PUBLIC );
**   and
**   msg_ptr = (xxxbbbapi_msgtwo_msg*)__ipc_new( sizeof(xxxbbbapi_msgtwo_msg)+9,
**                                               xxxbbbapi_msgtwo_msg_id,
**                                               IPCDEF_MSG_POOL_PUBLIC );
**
** avoiding potential mistake when writing this kind of code.
**    
** SEE _ipc_new() function prototype
**
*/
#define ipc_new(mt, p) \
        ((mt*)(void*)__ipc_new(sizeof(mt), mt##_id, p))

#define ipc_sized_new(mt, s, p) \
        ((mt*)__ipc_new(sizeof(mt)+s-1, mt##_id, p))

/*
** This macro translates a call such as:
**    ipc_delete(&msgtwo_ptr);
**
** into
**
**    __ipc_delete( (union ipc_msg_type**)&msgtwo_ptr );
**
** avoiding potential mistake when writing this kind of code.
** 
** PLEASE NOTE:
**    The message pointer MUST be a double pointer to begin with.
**    
** SEE _ipc_delete() function prototype
**
*/
#define ipc_delete(mpp)  __ipc_delete((union ipc_msg_type**)(mpp))

/*
** This macro translates a call such as:
**    ipc_send(&msgtwo_ptr, DESTINATION_NODE_ID);
**
** into
**
**    __ipc_send( (union ipc_msg_type**)&msgtwo_ptr, DESTINATION_NODE_ID );
**
** avoiding potential mistake when writing this kind of code.
** 
** PLEASE NOTE:
**    The message pointer MUST be a double pointer to begin with.
**    
** SEE _ipc_send() function prototype
**
*/
#define ipc_send(mpp,d)  __ipc_send((union ipc_msg_type**)(mpp), d)

/*
** This macro translates a call such as:
**    ipc_sender(&msgtwo_ptr);
**
** into
**
**    __ipc_sender( (union ipc_msg_type**)&msgtwo_ptr );
**
** avoiding potential mistake when writing this kind of code.
** 
** PLEASE NOTE:
**    The message pointer MUST be a double pointer to begin with.
**    
** SEE _ipc_sender() function prototype
**
*/
#define ipc_sender(mp)  __ipc_sender((union ipc_msg_type*)(void*)(mp))

/*===========================================================================
                  DEFINITIONS AND TYPES DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
   Global Constant Declarations
---------------------------------------------------------------------------*/


/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
=============================================================================
=============================================================================
===                                                                       ===
===            S E N D I N G   A N D   R E C E I V I N G                  ===
===                                                                       ===
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================
FUNCTION IPC_NEW()

DESCRIPTION
  Allocate a message in the specified memory pool.
  The calling entity is now the owner of the memory.

PARAMETERS
  msg_size  Size of the message to allocate memory for
  msg_id    ID of the message to allocate
  msg_pool  Memory pool from which the message will be allocated

RETURN
  Pointer to the allocated message, NULL if allocation failed.
===========================================================================*/
union ipc_msg_type* __ipc_new
(
   uint32            size,
   ipc_msg_id_type   id,
   ipc_msg_pool_type pool
);

/*===========================================================================
FUNCTION IPC_DELETE()

DESCRIPTION
  Free the memory associated with the message.
  The memory is returned to the memory pool from which
  the message was allocated.
  The calling entity looses the ownership of the memory, and the
  pointer to the message is set to NULL. 

PARAMETERS
  msg_ptr   Message pointer to deallocate

RETURN
   None
===========================================================================*/
void __ipc_delete
(
   union ipc_msg_type** msg_ptr
);


/*===========================================================================
FUNCTION IPC_SEND()

DESCRIPTION
  Post a message in the destination node queue. 
  The caller looses the ownership of the message.
  The node_id is added to the client list of used modules if new
  The pointer to the message is set to NULL when E_SUCCESS
 
PARAMETERS
  msg_ptr   Message pointer to send
  node_id   Identification of the receiving module

RETURN
   E_SUCCESS
     IPC has accepted the message.
     Caller looses ownership which is transfered to the destination
   E_FAILURE
     IPC was unable to accept the Message.
     Caller keeps ownership of the message
===========================================================================*/
errno_enum_type __ipc_send
(
   union ipc_msg_type** msg_ptr,
   ipc_node_id_type   node_id
);

/*===========================================================================
FUNCTION IPC_RECEIVE()

DESCRIPTION
  Waits until a message is posted in the receive queue of the caller.
  This is a _blocking_ call.

  A specific set of message ids may be specified, in which case only those
  message will trigger the function to return.  Any message can be received
  by specifying an empty list (NULL, 0).

The message becomes the property of the caller of this function.

PARAMETERS
  id_list_ptr    The list of message ids wanted by the caller
  id_list_size   The number of ids in the list

RETURN
  A pointer to the message received.
==========================================================================*/
union ipc_msg_type* ipc_receive
(
   ipc_msg_id_type msg_id
);


/*===========================================================================
FUNCTION IPC_SENDER()

DESCRIPTION
   Returns the ID of the module which sent the message.

PARAMETERS
   msg_ptr   The message to look at

RETURN
   The module ID of the message sender. 
==========================================================================*/
ipc_node_id_type __ipc_sender
(
   union ipc_msg_type* msg_ptr
);


/*===========================================================================
FUNCTION IPC_CONFIGURE_MSG_PRIORITY()

DESCRIPTION
   Returns the ID of the module which sent the message.

PARAMETERS
   msg_ptr   The message to look at

RETURN
   The module ID of the message sender. 
==========================================================================*/
boolean ipc_configure_msg_priority
(
   const ipc_msg_priority_type* list_ptr,
   uint32                 count
);

/*===========================================================================
=============================================================================
=============================================================================
===                                                                       ===
===                  H E L P E R   F U N C T I O N S                      ===
===                                                                       ===
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================
FUNCTION IPC_PRINT_MSG()

DESCRIPTION
   Print the values of the internal IPC fields.

PARAMETERS
   msg_ptr   The message to look at

RETURN
   None.
==========================================================================*/
void ipc_print_msg
(
   union ipc_msg_type* msg_ptr
);

/*===========================================================================
FUNCTION INIT_IPC()

DESCRIPTION
   IPC Initialization routine to be called at task init.

PARAMETERS
   service_id         the ipc_node_id 
   ipc_receive_sig    signal used to received ipc
RETURN
   None.
==========================================================================*/
void init_ipc(ipc_node_id_type service_id,
              uint32           ipc_receive_sig 
              );

/*===========================================================================
FUNCTION ipc_send_and_recv

DESCRIPTION

PARAMETERS

RETURN VALUE
  none

SIDE EFFECTS
  Notifies potential clients.
===========================================================================*/
void ipc_send_and_recv
(
   union ipc_msg_type    *request,
   uint32                 destination,
   ipc_msg_id_type        response_id,
   union ipc_msg_type   **response
);

#define ipc_create(mt, ptr, p ) \
   __ipc_create(sizeof(mt), mt##_id, ptr, p);

/*===========================================================================
FUNCTION __ipc_create

DESCRIPTION

PARAMETERS

RETURN VALUE
  none

SIDE EFFECTS
  Notifies potential clients.
===========================================================================*/
void __ipc_create
(
   uint32                size,
   ipc_msg_id_type       id,
   union ipc_msg_type  **ptr,
   ipc_msg_pool_type     pool
);

/*===========================================================================
FUNCTION ipc_memcpy

DESCRIPTION
   "Safe" IPC version of memcpy that will validate that memory space is the
    clients and not overrun memory.

PARAMETERS

RETURN VALUE
  The value of destination.

SIDE EFFECTS
  Notifies potential clients.
===========================================================================*/
#ifndef FEATURE_QDSP6
void* ipc_memcpy
(
   void       *destination,
   const void *source,
   uint32      size
);
#endif /* ifndef FEATURE_QDSP6 */

/*===========================================================================
FUNCTION ipc_strcpy

DESCRIPTION
   "Safe" IPC version of strcpy that will validate that memory space is the
    clients and not overrun memory.

PARAMETERS

RETURN VALUE
  The destination string.

SIDE EFFECTS
  Notifies potential clients.
===========================================================================*/
char* ipc_strcpy
(
   char       *strDestination,
   const char *strSource
              );

#ifdef __cplusplus
}
#endif
#endif /* RTE_IPC_H */
