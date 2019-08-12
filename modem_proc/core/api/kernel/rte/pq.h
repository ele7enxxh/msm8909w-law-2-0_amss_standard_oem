#ifndef PQ_H
#define PQ_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                M E S S A G E    P R I O R I T Y    Q U E U E
                             
GENERAL DESCRIPTION
  Implements a priority queue from incoming messages.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rte/pq.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/13   et      Created module.

===========================================================================*/


/*===================================================================
   INCLUDES AND VARIABLE DEFINITIONS
===================================================================*/

/*-------------------------------------------------------------------
   Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
   Include Files
-------------------------------------------------------------------*/
#include "comdef.h"
#include "ipctypes.h"

/*-------------------------------------------------------------------
   Defines
-------------------------------------------------------------------*/
#define PQ_QUEUE_SIZE       16

/*-------------------------------------------------------------------
   Type Declarations
-------------------------------------------------------------------*/
typedef struct
{
   ipc_msg_priority_type* p;
   uint32                 p_count;
} pq_msg_priority_type;

typedef struct
{
   struct ipc_internal_msg_type*  head_ptr;
   struct ipc_internal_msg_type*  tail_ptr;
   uint16                         priority;
   uint16                         queue_position;
} pq_table_bin_type;

typedef struct
{
   pq_table_bin_type* bin[PQ_QUEUE_SIZE+1];/*0 is not used*/
   uint16             last_elem;
} pq_p_queue_type;

typedef struct pq_msg_queue_type
{
   pq_table_bin_type     table[PQ_QUEUE_SIZE+1];/*0 is not used*/
   pq_msg_priority_type  msg_priorities;
   pq_p_queue_type       queue;
} pq_msg_queue_type;

/*-------------------------------------------------------------------
   Global Constant Definitions
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
   Global Data Declarations
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
   Static Variable Definitions
-------------------------------------------------------------------*/
   

/*-------------------------------------------------------------------
   Function Prototypes
-------------------------------------------------------------------*/


/*===================================================================
   FUNCTION DEFINITIONS
===================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif
/*===================================================================
FUNCTION   PQ_FLUSH

DESCRIPTION
   Clear the priority queue, free the messages,
   and reset the configured priorities.

PARAMETERS
   pq:       The priority queue to flush

RETURN VALUE
   None.
===================================================================*/
void pq_flush
(
   pq_msg_queue_type* pq
);

/*===========================================================================
FUNCTION PQ_CONFIGURE_MSG_PRIORITY()

DESCRIPTION
   Retrieves the priority associated with a message id.

PARAMETERS
   pq:    the queue to configure
   list:  the list of message priorities
   count: the number of configured messages in the list

RETURN VALUE
   TRUE if the queue has been configured, FALSE otherwise
==========================================================================*/
boolean pq_configure_msg_priority
(
   pq_msg_queue_type*     pq,
   const ipc_msg_priority_type* list,
   uint32                 count
);


/*===================================================================
FUNCTION   PQ_INSERT_MSG

DESCRIPTION
   Adds a new message in the priority queue according to the priority
   previously configured.

PARAMETERS
   msg_ptr:  The msg to add
   pq:       The priority queue to add to message into

RETURN VALUE
   None.
===================================================================*/
void pq_insert_msg
(
   struct ipc_internal_msg_type* msg_ptr,
   pq_msg_queue_type*            pq
);


/*===================================================================
FUNCTION   PQ_GET_MSG

DESCRIPTION
   Retrieves the highest priority message from the queue.

PARAMETERS
   pq:       The priority queue to retrieve the message from

RETURN VALUE
   A pointer to the message.  NULL if the queue is empty.
===================================================================*/
struct ipc_internal_msg_type* pq_get_next_msg
(
   pq_msg_queue_type* pq,
   uint32             wanted_msg_id
);


/*===================================================================
FUNCTION   PQ_IS_EMPTY

DESCRIPTION
   Check to see if there are more messages in the queue

PARAMETERS
   pq:       The priority queue to check

RETURN VALUE
   TRUE if no more messages in the queue, FALSE otherwise.
===================================================================*/
boolean pq_is_empty
(
   const pq_msg_queue_type* pq
);


/*===================================================================
FUNCTION   PQ_PRINT

DESCRIPTION
   Print the priority contents to the 'console'.

PARAMETERS
   pq:       The priority queue to print

RETURN VALUE
   None.
===================================================================*/
void pq_print
(
   const pq_msg_queue_type* pq
);

#ifdef __cplusplus
}
#endif

#endif /* PQ_H */
