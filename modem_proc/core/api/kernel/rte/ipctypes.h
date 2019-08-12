#ifndef IPCTYPES_H
#define IPCTYPES_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rte/ipctypes.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/
/*
** Node Range definition
** Defines the static and dynamic ranges for nodes in the system
** Node 0 is reserved for internal use
*/
#define IPC_NODE_RESERVED_ID         0
           
#define IPC_NODE_FIRST_STATIC_ID     1
#define IPC_NODE_LAST_STATIC_ID    255
           
#define IPC_NODE_FIRST_DYNAMIC_ID  (IPC_NODE_LAST_STATIC_ID+1)
#define IPC_NODE_LAST_DYNAMIC_ID   1023
#define IPC_NODE_MAX_ID            IPC_NODE_LAST_DYNAMIC_ID

#define KX_TERMINATE_MSG 0x00000000

/*
** Identify memory pool to allocate message from
*/
#define IPC_MSG_POOL_PUBLIC         0
#define IPC_MSG_POOL_PROTECTED      1

/*
** Used by ipc_receive() to accept any incoming message
*/
#define IPC_MSG_ANY                 0

/*
** Define priorities for messages
*/
#define IPC_MSG_PRIORITY_MIN        1
#define IPC_MSG_PRIORITY_MAX       16
#define IPC_MSG_PRIORITY_DEFAULT   (IPC_MSG_PRIORITY_MAX/2+1)

/*===========================================================================
              DEFINITIONS AND TYPES DECLARATIONS
===========================================================================*/

/* The id of the message sent by IPC */
typedef uint32 ipc_msg_id_type;

/* The id of the a message pool */
typedef uint32 ipc_msg_pool_type;

/* The id of an IPC node */
/*typedef rte_thread_id_type ipcns_node_id_type;*/
typedef uint32 ipc_node_id_type;

/* Pair defining the priority for a specific message */
typedef struct
{
   ipc_msg_id_type id;      
   uint16          priority;
} ipc_msg_priority_type;

typedef void (*rte_thread_fptr_type)(dword dummy);
typedef rte_thread_fptr_type rte_process_fptr_type;


#endif /* IPCTYPES_H */
