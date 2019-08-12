/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                M E S S A G E    P R I O R I T Y    Q U E U E
                             
GENERAL DESCRIPTION
  Implements a priority queue from incoming messages.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/rte/src/pq.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10-May-04  rwh     Feature-ized with FEATURE_REX_IPC
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
#include "core_variation.h"
#include <stringl/stringl.h>
#include "comdef.h"

#ifdef FEATURE_REX_IPC

#include "msg.h"

#include "ipctypes.h"
#include "ipcdefi.h"
#include "ipcmem.h"
#include "rte_ipc.h"
#include "pq.h"

/*-------------------------------------------------------------------
   Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
   Type Declarations
-------------------------------------------------------------------*/

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
static uint16 pq_lookup_msg_priority
(
   const pq_msg_queue_type* pq,
   uint32             msg_id
);



/*===================================================================
   FUNCTION DEFINITIONS
===================================================================*/
/*===========================================================================
FUNCTION PQ_PRIORITIZE_DOWN()

DESCRIPTION
   Reprioritize a queue by moving the last element to the root and pushing
   it down to its right location.

PARAMETERS
   pq:    the queue to configure
   pos:   the index the prioritize from

RETURN VALUE
   None
==========================================================================*/
static void pq_prioritize_down
(
   pq_msg_queue_type* pq,
   uint16             pos
)
{
   uint16             i, j;
   pq_table_bin_type* bin_ptr;
   
   /* Move the last element at the top of the queue */
   pq->queue.bin[pos] = pq->queue.bin[pq->queue.last_elem];
   pq->queue.last_elem--;
      
   /* Re-prioritize the queue */
   i = pos;
   while (i <= pq->queue.last_elem/2)
   {
      j = 2*i;                  /*lint !e734 17-bits to 16 bits*/

      /* In pq->queue.bin[j] & pq->queue.bin[j+1], index for bin cannot
         exceed ((PQ_QUEUE_SIZE+1)-1) as specified by the definition in pq.h */
      if (j >= PQ_QUEUE_SIZE)
      {
         MSG_MED("pq_prioritize_down array index over the limit", 0, 0, 0);            
         return;
      }

      if
      (
         (pq->queue.bin[j]->priority >= pq->queue.bin[j+1]->priority)
         &&
         (j != pq->queue.last_elem)
      )
      {
         j++;
      }

      if( (j > PQ_QUEUE_SIZE) || (i > PQ_QUEUE_SIZE) )
      {
         MSG_MED("pq_prioritize_down array index over the limit", 0, 0, 0);   
         return;
      }

      if (pq->queue.bin[i]->priority > pq->queue.bin[j]->priority)
      {
         /* Exchange previous last element
         ** with the child of lesser priority */
         bin_ptr = pq->queue.bin[i];

         pq->queue.bin[i]                 = pq->queue.bin[j];
         pq->queue.bin[i]->queue_position = j;

         pq->queue.bin[j]                 = bin_ptr;
         pq->queue.bin[j]->queue_position = i;
         i = j;
      }
      else
      {
         break;
      }
   }
}


/*===================================================================
FUNCTION   PQ_LOOKUP_MSG_PRIORITY

DESCRIPTION
   Retrieves the priority associated with a message id.

PARAMETERS
   msg_id:  the msg ID to look-up

RETURN VALUE
   The priority of the message.
   If the message was not associated with a priority,
   the default priority is returned.
===================================================================*/
static uint16 pq_lookup_msg_priority
(
   const pq_msg_queue_type* pq,
   uint32             msg_id
)
{
   uint32 i;

   for(i=0; i<pq->msg_priorities.p_count; ++i)
   {
      if ( pq->msg_priorities.p[i].id == msg_id )
      {
         return pq->msg_priorities.p[i].priority;
      }
   }
   return IPC_MSG_PRIORITY_DEFAULT;
}


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
)
{
   ipc_internal_msg_type* msg_ptr;
   uint32                 i;

   MSG_LOW("Message Priority Queue",0,0,0);
   MSG_LOW("----------------------",0,0,0);

   if (pq->queue.last_elem == 0)
   {
      MSG_LOW("   Empty",0,0,0);
   }
   
   for(i=1; i<=pq->queue.last_elem; ++i)
   {
      MSG_LOW("Priority [%d]", pq->queue.bin[i]->priority,0,0);
      msg_ptr = pq->queue.bin[i]->head_ptr;
      while (msg_ptr != NULL)
      {
         MSG_LOW("   Message ID: 0x%2x", msg_ptr->msg_id,0,0);
         msg_ptr = (ipc_internal_msg_type*)msg_ptr->next_ptr;
      }
   }
   MSG_LOW("\n",0,0,0);
}


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
)
{
   pq_table_bin_type*     temp_ptr;
   uint16                 msg_priority;
   uint16                 i;

   msg_ptr->next_ptr = NULL;

   /* Figure out which bin this message belongs to
   */
   msg_priority = pq_lookup_msg_priority(pq, msg_ptr->msg_id);

   if ( msg_priority > PQ_QUEUE_SIZE )
   {
      MSG_MED("pq_insert_msg array index over the limit", 0, 0, 0);
      return;
   }

   /* Add the new message at the end of the corresponding bin queue
   */
   if (pq->table[msg_priority].head_ptr != NULL)
   {
      /* Add after the existing message.
      ** No need to modify the priority queue since this bin is
      ** already there */
      pq->table[msg_priority].tail_ptr->next_ptr = msg_ptr;
      pq->table[msg_priority].tail_ptr = msg_ptr;
   }
   else
   {
      /* This bin queue becomes non empty
      **/
      pq->table[msg_priority].priority = msg_priority;
      pq->table[msg_priority].head_ptr = msg_ptr;
      pq->table[msg_priority].tail_ptr = msg_ptr;

      /* Add this new non-empty bin into the priority queue
      **/
      i = ++pq->queue.last_elem;
      pq->table[msg_priority].queue_position = i;

      if( i > PQ_QUEUE_SIZE )
      {
         MSG_MED("pq_insert_msg array index over the limit", 0, 0, 0);   
         return;
      }

      pq->queue.bin[i] = &pq->table[msg_priority];
      
      /* Move the bin up to its rightful spot in the priority queue */
      while
      (
         (i>1)
         &&
         (pq->queue.bin[i]->priority < pq->queue.bin[i/2]->priority)
      )
      {
         /* Move new element up, exchanging it with its parent if
         ** its priority is higher */
         temp_ptr                           = pq->queue.bin[i];
         
         pq->queue.bin[i]                   = pq->queue.bin[i/2];
         pq->queue.bin[i]->queue_position   = i;
         
         pq->queue.bin[i/2]                 = temp_ptr;
         pq->queue.bin[i/2]->queue_position = i/2;

         i /= 2;
      }
   }
}


/*===================================================================
FUNCTION   PQ_GET_NEXT_MSG

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
   ipc_msg_id_type    wanted_msg_id
)
{
   pq_table_bin_type*     bin_ptr;
   ipc_internal_msg_type* msg_ptr = NULL;
   ipc_internal_msg_type* previous_msg_ptr;
   uint32                 msg_prio;

   /*
   ** A Specific message is beeing requested
   ** Look for it in its associated bin in the priority queue and return it
   ** regardless of messages priority
   **/   
   if (wanted_msg_id != IPC_MSG_ANY)
   {
      /* The bin this message ID is associated with */
      msg_prio = pq_lookup_msg_priority(pq, wanted_msg_id);

      if ( msg_prio > PQ_QUEUE_SIZE )
      {
         MSG_MED("pq_get_next_msg array index over the limit", 0, 0, 0);
         return NULL;  // Correct return value?
      }

      /* Check for messages in the bin.
      ** Locate the message in the associated bin in the priority queue */
      if ( pq->table[msg_prio].head_ptr != NULL )
      {
         /* Go through the bin, looking for this message ID */
         bin_ptr          = pq->queue.bin[pq->table[msg_prio].queue_position];
         previous_msg_ptr = NULL;
         msg_ptr          = bin_ptr->head_ptr;
         
         while (msg_ptr != NULL)
         {
            /* Message is found
            **/
            if (msg_ptr->msg_id == wanted_msg_id)
            {
               /* Adjust the message list in the bin
               */
               if (previous_msg_ptr != NULL)
               {
                  /* Remove message in the middle of the queue */
                  previous_msg_ptr->next_ptr = msg_ptr->next_ptr;
                  /* Adjust the tail if the message removed is the last one */
                  if (bin_ptr->tail_ptr == msg_ptr)
                  {
                     bin_ptr->tail_ptr = previous_msg_ptr;
                  }
               }
               else
               {
                  /* Remove first message in the queue */
                  bin_ptr->head_ptr = msg_ptr->next_ptr;
               }            

               msg_ptr->next_ptr = NULL;

               /* Remove an empty bin from the priority queue
               */
               if (bin_ptr->head_ptr == NULL)
               {
                  bin_ptr->tail_ptr = NULL;
                  pq_prioritize_down(pq, pq->table[msg_prio].queue_position);
               }
               break;
            }
            
            previous_msg_ptr = msg_ptr;
            msg_ptr          = msg_ptr->next_ptr;
         }
      }
   }
   else
   {
      /*
      ** A generic message is being requested
      ** Take the next highest priority message out the queue and return it
      **/ 
      if (pq->queue.last_elem != 0)
      {
         /* Get the top priority element out of the queue
         **/
         bin_ptr           = pq->queue.bin[1];
         msg_ptr           = bin_ptr->head_ptr;
         bin_ptr->head_ptr = (ipc_internal_msg_type*)msg_ptr->next_ptr;
         msg_ptr->next_ptr = NULL;
   
         /* Remove an empty bin from the priority queue */
         if (bin_ptr->head_ptr == NULL)
         {
            bin_ptr->tail_ptr = NULL; /*  reset the bin tail pointer */
            pq_prioritize_down(pq, 1);
         }
      }
   }
   
   return msg_ptr;
}


/*===========================================================================
FUNCTION PQ_FLUSH()

DESCRIPTION
   Remove all the messages from the queue and returns them to their pool

PARAMETERS
   pq:    the queue to flush

RETURN VALUE
   None
==========================================================================*/
void pq_flush
(
   pq_msg_queue_type* pq
)
{
   ipc_internal_msg_type* msg_ptr;
   uint32                 i;

   for(i=1; i<=pq->queue.last_elem; ++i)
   {
      msg_ptr = pq->queue.bin[i]->head_ptr;
      while (msg_ptr != NULL)
      {
         ipcmem_deallocate(&msg_ptr);
         msg_ptr = (ipc_internal_msg_type*)msg_ptr->next_ptr;
      }
   }

   memset(pq, 0, sizeof(pq_msg_queue_type));
}


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
)
{
   uint32                 i;

   /* Verify priorities are in range */
   for(i=0; i<count; ++i)
   {
      if ( (list[i].priority < IPC_MSG_PRIORITY_MIN)
           ||
           (list[i].priority > IPC_MSG_PRIORITY_MAX) )
      {
         return FALSE;
      }
   }
   
   /* Copy list locally */
   pq->msg_priorities.p = (ipc_msg_priority_type*)__ipc_new
   (
      count * sizeof(ipc_msg_priority_type),
      0,
      IPC_MSG_POOL_PROTECTED
   );

   if ( pq->msg_priorities.p == NULL)
   {
      return FALSE;
   }

   memscpy(pq->msg_priorities.p, count * sizeof(ipc_msg_priority_type), list, count * sizeof(ipc_msg_priority_type));
   pq->msg_priorities.p_count = count;
   
   return TRUE;
}


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
)
{
   if (NULL == pq)
   {
      return TRUE;
   }

   return (pq->queue.last_elem == 0);
}

#endif /* FEATURE_REX_IPC */

