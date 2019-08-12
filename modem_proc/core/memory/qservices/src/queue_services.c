/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Q U E U E    S E R V I C E S - SINGLY LINKED and
                                                       DOUBLY LINKED

GENERAL DESCRIPTION

  This file contains optimized queue functions for data path services.
	The queue services are built over the commonly used <queue.h> structures 
	(types and declarations).


  A queue is a simple data structure used for logically storing and re-
  trieving data blocks, in a first in - first out (FIFO) order without
  physically copying them. Software tasks and interrupt handlers can use
  queues for cleanly transferring data blocks to one another.

                        +-------+     +-------+     +-------+
                        | DATA  |     | DATA  |     | DATA  |
                        | BLOCK |     | BLOCK |     | BLOCK |
          +-------+     +-------+     +-------+     +-------+
          | QUEUE |---->| LINK  |---->| LINK  |---->| LINK  |---- |
   |----->+-------+     +-------+     +-------+     +-------+     |
   |      |       |     |       |     |       |     |       |     |
   |      +-------+     |       |     |       |     |       |     |
   |                    +-------+     +-------+     +-------+     |
   |                                                              |
   ---------------------------------------------------------------|

  The Queue Services provide a small set of declarations and functions for
  defining and initializing a queue, defining and initializing a links with-
  in a data block, placing a data block at the tail of a queue, removing a
  data block from the head of a queue, and removing a data block from any
  position in a queue.

  Aside from requiring each data block to contain a link, the Queue Services
  impose no restrictions on the size of structure of data blocks used with
  with queues. This allows software to pass virtually any type of data on
  queues. Notice that a data block may contain multiple links allowing it to
  be placed simultaneously on multiple queues.


EXTERNALIZED FUNCTIONS

  refer <queue_services.h>

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Prior to use, a queue must be initialized by calling q_init_s/d. Similarly,
  a link must be initialized prior to use by calling q_link_s/d.

Copyright (c) 1990-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                              Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/queue.c_v   1.4   02 Oct 2001 10:46:56   rajeevg  $
$Header: //components/rel/core.mpss/3.5.c12.3/memory/qservices/src/queue_services.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/14   sh      Doubly Linked queue without feature flags, also used by DSM
===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */

#include "core_variation.h"
#include "queue_services.h"
#ifndef FEATURE_WINCE
#include "amssassert.h"
#endif

/* NOTUSED */
#define NOTUSED(i) if(i){}

/* ------------------------------------------------------------------------
** For Win32 Systems, users may want to enable mutual exclusion for the
** queue operations, this provides a mutex per queue along with locking
** and unlocking primitives.
** ------------------------------------------------------------------------ */

#if ((defined FEATURE_QUBE) || ((defined FEATURE_Q_WIN32_LOCKS) || (defined FEATURE_WINCE)) && !defined(FEATURE_WINCE_BOOTLOADER))

#if (((defined FEATURE_Q_WIN32_LOCKS) || (defined FEATURE_WINCE)) && !defined(FEATURE_WINCE_BOOTLOADER))
#include <windows.h>
#define USE_MUTEX
#endif

#ifdef FEATURE_QUBE
#include "qube.h"
#include "err.h"
#endif /* FEATURE_QUBE */


/* Queue  diagram - Detailed */

/*=====================Doubly linked Empty ==============
               
        Queue node (empty)
      (q_ptr->link)          
      --------------      
     | prev | next  |     
 --->|(tail)|(head)-|--------
|  --|--enq |   deq |<----   |
| |   --------------      |  |
| |                       |  |
 -|-----------------------|--  
  |                       |
   -----------------------

=====================Doubly linked with 1 node ========

               
       Queue node 
      (q_ptr->link)           first node
      --------------         --------------
     | prev | next  |       |      |       |<--------
 --->|(tail)|(head) |------>| prev | next -|--       |
|  --|--enq |   deq |<------|--    |       |  |      |
| |   --------------         --------------   |      |
| |                                           |      |
 -|-------------------------------------------       |
  |                                                  |
   --------------------------------------------------

=====================Singly linked Empty ===================

        Queue node (empty)
      (q_ptr->link)
      --------------
     | prev | next  |
 --->|(tail)|(head)-|--------
|  --|--enq |   deq |<----   |
| |   --------------      |  |
| |                       |  |
 -|-----------------------|--
  |                       |
   -----------------------


=====================Singly linked with 1 node ========

       Queue node 
      (q_ptr->link)           first node
      --------------         --------------
     | prev | next  |       |      |       |<--------
 --->|(tail)|(head) |------>| usr_ | next -|--       |
|  --|--enq |   deq |       | data |       |  |      |
| |   --------------         --------------   |      |
| |                                           |      |
 -|-------------------------------------------       |
  |                                                  |
   --------------------------------------------------


==========================================================*/


/* ==========================================================================
FUNCTION q_lock_s_INIT
DESCRIPTION
   Initializes the mutex for the given queue. It is expected that this call 
   happens only once for every queue. If calling multiple times for a given 
   queue, ensure that the mutex is deleted/freed (q_lock_s_delete), prior to it.
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
============================================================================= */
static void q_lock_s_init( q_type *q_ptr )
{
  #ifdef USE_MUTEX
    q_ptr->mutex = CreateMutex( NULL, FALSE, NULL );
  #elif defined FEATURE_QUBE
   int status;
   qmutex_attr_t  qmutexattr;
   qmutexattr.type = QMUTEX_LOCAL;
   
   ASSERT( q_ptr != NULL );
   status = qmutex_create(&(q_ptr->mutex), &qmutexattr);
   ASSERT( status == EOK );
  #else
	/* Allocate space for the Critical Section object */
	//q_ptr->pkcsQueueCriticalSection = (KxCriticalSection *)malloc(sizeof(KxCriticalSection));

	/* Now initialize it */
	if( !( KxMutex_Init( &(q_ptr->pkcsQueueCriticalSection), OBJ_NAME) ) ){

		/* Huge issue -- log it, but continue */
		//ERR("Kx Init returned false!\n",0,0,0);

	}
	sectionCounter++;
#endif //USE_MUTEX

	return;
} /* END q_lock_s_init */

/* ==========================================================================
FUNCTION q_lock_s_DELETE
DESCRIPTION
   Deletes the mutex for the given queue
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
============================================================================= */
static void q_lock_s_delete( q_type *q_ptr )
{
#ifdef FEATURE_QUBE
   int status;
#endif

   ASSERT( q_ptr != NULL );
  
   if(NULL == q_ptr->mutex)
   {
      /* error case : can the value of q_ptr->mutex be "0" ? */
      return; /* or ASSERT(0); */
   }
  #ifdef USE_MUTEX
   ASSERT ( (ReleaseMutex(q_ptr->mutex)) );
  #elif defined FEATURE_QUBE
   status = qmutex_delete (q_ptr->mutex);
   ASSERT ( status == EOK );
  #else
   /* q_ptr->pkcsQueueCriticalSection - NULL Check ? */
   KxMutex_Free ( &(q_ptr->pkcsQueueCriticalSection));
   sectionCounter--;
  #endif //USE_MUTEX
  #ifndef FEATURE_WINCE
   q_ptr->mutex = NULL;
  #endif

	return;
}  /* END q_lock_s_delete */


/* ==========================================================================
FUNCTION q_lock_s
DESCRIPTION
   Locks the mutex for the given queue
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
============================================================================= */
void q_lock_s( q_type *q_ptr )
{
  #ifdef USE_MUTEX
   WaitForSingleObject( q_ptr->mutex, INFINITE );
  #elif defined FEATURE_QUBE
   int status;
   ASSERT( q_ptr != NULL );
   status = qmutex_lock((qmutex_t)q_ptr->mutex);
   ASSERT( status == EOK );
  #else 
	(void)KxMutex_Lock( &(q_ptr->pkcsQueueCriticalSection) );
  #endif //USE_MUTEX
	return;
} /* END q_lock_s */

/* ==========================================================================
FUNCTION q_unlock_s
DESCRIPTION
   Frees the mutex for the given queue
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
============================================================================= */
void q_unlock_s( q_type *q_ptr )
{
  #ifdef USE_MUTEX
   ReleaseMutex( q_ptr->mutex );
  #elif defined FEATURE_QUBE
   int status;
   ASSERT( q_ptr != NULL );
   status = qmutex_unlock((qmutex_t)q_ptr->mutex);
   ASSERT( status == EOK );
  #else 
	KxMutex_Unlock( &(q_ptr->pkcsQueueCriticalSection) );
#endif //USE_MUTEX
  return;
} /* END q_unlock_s */

#else  
   #define q_lock_s( q ) INTLOCK( )
   #define q_unlock_s( q ) INTFREE( )   
   #define q_lock_s_init( q )
   #define q_lock_s_delete( q )
#endif /* FEATURE_Q_WIN32_LOCKS */

/*==========================================================================
FUNCTION Q_INIT_S

DESCRIPTION
  This function initializes a specified queue. It should be called for each
  queue prior to using the queue with the other Queue Services.

DEPENDENCIES
  None.

RETURN VALUE
  A pointer to the initialized queue.

SIDE EFFECTS
  The specified queue is initialized for use with Queue Services.
===========================================================================*/
q_type* q_init_s(
   q_type  *q_ptr  /* Ptr to queue to be initialized. */,
   boolean lock_init
)
{
   ASSERT( q_ptr != NULL );
   q_ptr->link.next_ptr = (q_link_type *)(&q_ptr->link);  /* Points to q link. */

   /* for single_link queue q_ptr->link.prev_ptr is used as tail_ptr */
   q_ptr->link.prev_ptr = (q_link_type *)(&q_ptr->link); /* Points to q link. */

   q_ptr->cnt = 0;
   q_ptr->mutex = 0;

   if (lock_init)
   {

     #ifdef FEATURE_QUBE
     // Only call qmutex_create once.
     if ( ! q_ptr->mutex ) { //Assumes zero-init of q_type.mutex
     #endif

     q_lock_s_init( q_ptr );

     #ifdef FEATURE_QUBE
     }
     #endif
     } 

   return q_ptr;
} /* END q_init */

/*==========================================================================
FUNCTION Q_INIT_D

DESCRIPTION
  This function initializes a specified queue. It should be called for each
  queue prior to using the queue with the other Queue Services.

DEPENDENCIES
  None.

RETURN VALUE
  A pointer to the initialized queue.

SIDE EFFECTS
  The specified queue is initialized for use with Queue Services.
===========================================================================*/
q_type* q_init_d ( q_type *q_ptr,
                    boolean lock_init)
{
  return q_init_s(q_ptr,lock_init);
}

/*==========================================================================
FUNCTION Q_DESTROY_S

DESCRIPTION
  This function destroys a specified queue. It should be called if you
  do not require this queue anymore.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Elements in the queue will  not be accessible through this queue 
  anymore. It is user's responsibility to deallocate the memory allocated 
  for the queue and its elements to avoid leaks
===========================================================================*/
void q_destroy_s(
   q_type  *q_ptr , /* Ptr to queue to be destroyed. */
   boolean lock_destroy
)
{
   ASSERT( q_ptr != NULL );

   if (lock_destroy)
   {
     q_lock_s( q_ptr );
   }
   q_ptr->link.next_ptr = NULL;
   /* for single_link queue q_ptr->link.prev_ptr is used as tail_ptr */
   q_ptr->link.prev_ptr = NULL; 

   q_ptr->cnt = 0;
   if (lock_destroy)
   {
     q_unlock_s( q_ptr );
	 q_lock_s_delete( q_ptr );
   }   
} /* END q_destroy_s */
/*==========================================================================
FUNCTION Q_DESTROY_D

DESCRIPTION
  This function destroys a specified queue. It should be called if you
  do not require this queue anymore.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Elements in the queue will  not be accessible through this queue 
  anymore. It is user's responsibility to deallocate the memory allocated 
  for the queue and its elements to avoid leaks
===========================================================================*/
void q_destroy_d(
   q_type  *q_ptr  /* Ptr to queue to be destroyed. */,
   boolean lock_destroy
	)
{
  q_destroy_s(q_ptr,lock_destroy);
}

/*===========================================================================
FUNCTION Q_LINK_S

DESCRIPTION
  This function initializes a specified link. It should be called for each
  link prior to using the link with the other Queue Services.

DEPENDENCIES
  None.

RETURN VALUE
  A pointer to the initialized link.

SIDE EFFECTS
  The specified link is initialized for use with the Queue Services.
===========================================================================*/
q_link_type* q_link_s(
   q_link_type  *link_ptr   /* Ptr to link field within variable. */
)
{
   ASSERT( link_ptr != NULL );
   link_ptr->next_ptr = NULL;
   q_set_usr_data_s(link_ptr,NULL);
   return link_ptr;
} /* END q_link_s */

/*===========================================================================
FUNCTION Q_LINK_D

DESCRIPTION
  This function initializes a specified link. It should be called for each
  link prior to using the link with the other Queue Services.

DEPENDENCIES
  None.

RETURN VALUE
  A pointer to the initialized link.

SIDE EFFECTS
  The specified link is initialized for use with the Queue Services.
===========================================================================*/

q_link_type* q_link_d(
  q_link_type  *link_ptr   /* Ptr to link field within variable. */
)
{
   ASSERT( link_ptr != NULL );
   link_ptr->next_ptr = NULL;
   link_ptr->prev_ptr = NULL;
   return link_ptr;
  
} /* END q_link_d */

/*===========================================================================
FUNCTION Q_PUT_S

DESCRIPTION
  This function enqueues an item onto a specified queue using a specified
  link.

DEPENDENCIES
  The specified queue should have been previously initialized via a call
  to q_init. The specified link field of the item should have been prev-
  iously initialized via a call to q_init_link.

RETURN VALUE
  None.

SIDE EFFECTS
  The specified item is placed at the tail of the specified queue.
===========================================================================*/
void q_put_s(
   q_type       *q_ptr,    /* Ptr to queue. */
   q_link_type  *link_ptr  /* Ptr to item link to use for queueing. */
)
{
   ASSERT( q_ptr != NULL );
   ASSERT( link_ptr != NULL );
   q_lock_s( q_ptr );

   link_ptr->next_ptr = (q_link_type *)&q_ptr->link;
   /* for single_link queue q_ptr->link.prev_ptr is used as tail_ptr */
   q_ptr->link.prev_ptr->next_ptr = link_ptr;
   q_ptr->link.prev_ptr           = link_ptr; 
   q_ptr->cnt++;
   q_unlock_s( q_ptr );   
} /* END q_put_s */

/*===========================================================================
FUNCTION Q_PUT_D

DESCRIPTION
  This function enqueues an item onto a specified queue using a specified
  link.

DEPENDENCIES
  The specified queue should have been previously initialized via a call
  to q_init. The specified link field of the item should have been prev-
  iously initialized via a call to q_init_link.

RETURN VALUE
  None.

SIDE EFFECTS
  The specified item is placed at the tail of the specified queue.
===========================================================================*/
void q_put_d(
   q_type       *q_ptr,    /* Ptr to queue. */
   q_link_type  *link_ptr  /* Ptr to item link to use for queueing. */
)
{
   ASSERT( q_ptr != NULL );
   ASSERT( link_ptr != NULL );
   q_lock_s( q_ptr );

   link_ptr->next_ptr = (q_link_type *)&q_ptr->link;
   link_ptr->prev_ptr = q_ptr->link.prev_ptr;
   /* for single_link queue q_ptr->link.prev_ptr is used as tail_ptr */
   q_ptr->link.prev_ptr->next_ptr = link_ptr;
   q_ptr->link.prev_ptr           = link_ptr; 
   q_ptr->cnt++;
   q_unlock_s( q_ptr );   
} /* END q_put_d */

/*===========================================================================
FUNCTION Q_M_PUT_S

DESCRIPTION
  This function enqueues all items from a specified (source) queue into a 
  specified (target) queue.

DEPENDENCIES
  The specified queues should have been previously initialized via a call
  to q_init.

RETURN VALUE
  None.

SIDE EFFECTS
  The items in the specified (source) queue are placed at the tail of the 
  specified (target) queue.
===========================================================================*/
void q_m_put_s(
   q_type       *target_q_ptr,    /* Ptr to target queue. */
   q_type       *source_q_ptr    /* Ptr to source queue. */
)
{
  /* Head and Tail items of source queue */
  q_link_type  *source_tail_item;
   
  q_lock_s( source_q_ptr );
  if (q_cnt_s( source_q_ptr ) > 0)
  {
   q_lock_s( target_q_ptr );

   /* Get tail item of source queue */
   /* for single_link queue q_ptr->link.prev_ptr is used as tail_ptr */
   source_tail_item = source_q_ptr->link.prev_ptr;

   source_tail_item->next_ptr = (q_link_type *) &target_q_ptr->link;
   target_q_ptr->link.prev_ptr->next_ptr = source_q_ptr->link.next_ptr;
   target_q_ptr->link.prev_ptr = source_tail_item;

   target_q_ptr->cnt += source_q_ptr->cnt;

   //Q_XCEPT_Q_PUT( q_ptr, link_ptr );

   source_q_ptr->link.next_ptr = (q_link_type *)(&source_q_ptr->link);  /* Points to q link. */
   source_q_ptr->link.prev_ptr = (q_link_type *)(&source_q_ptr->link); /* Points to q link. */
   source_q_ptr->cnt = 0;
   
   q_unlock_s( target_q_ptr );
  }
  q_unlock_s( source_q_ptr );
} /* END q_m_put_s */



/*===========================================================================
FUNCTION Q_M_PUT_D

DESCRIPTION
  This function enqueues all items from a specified (source) queue into a 
  specified (target) queue.

DEPENDENCIES
  The specified queues should have been previously initialized via a call
  to q_init.

RETURN VALUE
  None.

SIDE EFFECTS
  The items in the specified (source) queue are placed at the tail of the 
  specified (target) queue.
===========================================================================*/
void q_m_put_d(
   q_type       *target_q_ptr,    /* Ptr to target queue. */
   q_type       *source_q_ptr    /* Ptr to source queue. */
)
{
  q_m_put_s(target_q_ptr,source_q_ptr);  
} /* END q_m_put_d */

/*===========================================================================
FUNCTION Q_GET_S

DESCRIPTION
  This function removes an item from the head of a specified queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

RETURN VALUE
  A pointer to the dequeued item. If the specified queue is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the specified queue.
===========================================================================*/
void* q_get_s(
  q_type  *q_ptr  /* Ptr to queue. */
)
{
   q_link_type  *link_ptr;
   q_link_type  *ret_ptr = NULL;

   ASSERT( q_ptr != NULL );
   q_lock_s( q_ptr );

   /* Get ptr to 1st queue item.   */
   link_ptr = q_ptr->link.next_ptr;

   /* Can only get an item if the queue is non empty
   */
   if( q_ptr->cnt > 0 )
   {
     q_ptr->link.next_ptr = link_ptr->next_ptr;
	 if (q_ptr->link.prev_ptr == link_ptr)
     {
   	  q_ptr->link.prev_ptr = &q_ptr->link;
     } 
     q_ptr->cnt--;
     /* Mark item as no longer in a queue.
     */
     link_ptr->next_ptr = NULL;
     ret_ptr = link_ptr;
   }
   q_unlock_s( q_ptr );
   return (void *)ret_ptr;
} /* END q_get_s */

/*===========================================================================
FUNCTION Q_GET_D

DESCRIPTION
  This function removes an item from the head of a specified queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

RETURN VALUE
  A pointer to the dequeued item. If the specified queue is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the specified queue.
===========================================================================*/
void* q_get_d(
  q_type  *q_ptr  /* Ptr to queue. */
)
{
   q_link_type  *link_ptr;
   q_link_type  *ret_ptr = NULL;

   ASSERT( q_ptr != NULL );
   q_lock_s( q_ptr );

   /* Get ptr to 1st queue item.
   */
   link_ptr = q_ptr->link.next_ptr;

   /* Can only get an item if the queue is non empty
   */
   if( q_ptr->cnt > 0 )
   {
     q_ptr->link.next_ptr = link_ptr->next_ptr;
     link_ptr->next_ptr->prev_ptr = &q_ptr->link;
     link_ptr->prev_ptr = NULL;  
     q_ptr->cnt--;
     /* Mark item as no longer in a queue.
     */
     link_ptr->next_ptr = NULL;
     ret_ptr = link_ptr;
   }
   q_unlock_s( q_ptr );
   return (void *)ret_ptr;
} /* END q_get_d */

/*===========================================================================
FUNCTION Q_LAST_GET_S

DESCRIPTION
  This function returns the item which was most recently enqueued in a queue.

  Note, this is different from q_get() which returns the oldest item in a
  queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void * q_last_get_s(
   q_type* q_ptr
)
{
   q_link_type  *link_ptr;
   q_link_type  *ret_ptr=NULL;

   ASSERT( q_ptr != NULL );
   q_lock_s( q_ptr );

   /* Tail_ptr is q_ptr->link.prev_ptr */
   for( link_ptr           =  &q_ptr->link;
        link_ptr->next_ptr != q_ptr->link.prev_ptr;
        link_ptr           =  link_ptr->next_ptr
      );         
   if( q_ptr->cnt > 0 )
   {
      ret_ptr              = link_ptr->next_ptr;
      q_ptr->link.prev_ptr      = link_ptr;
      link_ptr->next_ptr   = &q_ptr->link;
      q_ptr->cnt--;
      ret_ptr->next_ptr = NULL;
   }
   q_unlock_s( q_ptr );
   return (void *)ret_ptr;
}  /* q_last_get_s */

/*===========================================================================
FUNCTION Q_LAST_GET_D

DESCRIPTION
  This function returns the item which was most recently enqueued in a queue.

  Note, this is different from q_get() which returns the oldest item in a
  queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void * q_last_get_d(
   q_type* q_ptr
)
{
   q_link_type  *link_ptr;
   q_link_type  *ret_ptr=NULL;

   ASSERT( q_ptr != NULL );
   q_lock_s( q_ptr );
   link_ptr = q_ptr->link.prev_ptr;

   if ( q_ptr->cnt > 0 )
   {
     q_ptr->link.prev_ptr         = link_ptr->prev_ptr;
     link_ptr->prev_ptr->next_ptr = &q_ptr->link;
     q_ptr->cnt--;

     link_ptr->next_ptr = NULL;
     link_ptr->prev_ptr = NULL;
     ret_ptr            = link_ptr;         
   }
   q_unlock_s( q_ptr );

   return (void *)ret_ptr;
}  /* q_last_get_d */


/*===========================================================================
FUNCTION Q_CHECK_S

DESCRIPTION
  This function returns a pointer to the data block at the head of the queue.
  The data block is not removed from the queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

RETURN VALUE
  A pointer to the queue item. If the specified queue is empty, then
  NULL is returned.

SIDE EFFECTS
  None
===========================================================================*/
void* q_check_s(
  q_type  *q_ptr
)
{
   q_link_type  *ret_ptr = NULL;
   ASSERT( q_ptr != NULL );
   q_lock_s( q_ptr );

   if( q_ptr->cnt > 0 )
   {
     ret_ptr = q_ptr->link.next_ptr;
   }
   q_unlock_s( q_ptr );
   return (void *)ret_ptr;
} /* END q_check_s */

/*===========================================================================
FUNCTION Q_CHECK_D

DESCRIPTION
  This function returns a pointer to the data block at the head of the queue.
  The data block is not removed from the queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

RETURN VALUE
  A pointer to the queue item. If the specified queue is empty, then
  NULL is returned.

SIDE EFFECTS
  None
===========================================================================*/
void* q_check_d(
  q_type  *q_ptr
)
{
  return q_check_s(q_ptr);
}

/*===========================================================================
FUNCTION Q_NEXT_S

DESCRIPTION
  This function returns a pointer to the next item on the queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

RETURN VALUE
  A pointer to the next item on the queue. If the end of the queue is reached,
  then NULL is returned.

SIDE EFFECTS
  None.
===========================================================================*/
void* q_next_s(
   q_type       *q_ptr,
   q_link_type  *q_link_ptr
)
{
   void       *q_temp_ptr = NULL;
   ASSERT( q_link_ptr != NULL );
   q_lock_s( q_ptr );
   if( (void *) q_link_ptr->next_ptr != (void *) q_ptr )
   {
     q_temp_ptr = q_link_ptr->next_ptr;
   }
   q_unlock_s( q_ptr );
   return q_temp_ptr;
} /* END q_next_s */

/*===========================================================================
FUNCTION Q_INSERT_S

DESCRIPTION
  This function inserts an item before a specified item on a queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Input item is inserted before input item.
===========================================================================*/
void q_insert_s(
   q_type    *q_ptr,          /* Ptr to the queue */
   q_link_type  *q_insert_ptr,   /* Ptr to link of item to insert */
   q_link_type  *q_item_ptr      /* Ptr to link item to insert before */
)
{
   q_link_type  *link_ptr;

   ASSERT( q_ptr        != NULL );
   ASSERT( q_insert_ptr != NULL );
   q_lock_s( q_ptr );
   q_insert_ptr->next_ptr = q_item_ptr;

   /* Start at beginning of queue and find the item that will be before the
    ** new item
    */
   link_ptr = (q_link_type *) q_ptr;
   /* Tail_ptr is q_ptr->link.prev_ptr */
   if (q_item_ptr == link_ptr) {
	   q_ptr->link.prev_ptr = q_insert_ptr;
   }

   while (link_ptr->next_ptr != q_item_ptr)
   {
     link_ptr = link_ptr->next_ptr;
         ASSERT( link_ptr != NULL );
   }

   link_ptr->next_ptr = q_insert_ptr;
   q_ptr->cnt++;
   q_unlock_s( q_ptr );
} /* END q_insert_s */

/*===========================================================================
FUNCTION Q_INSERT_D

DESCRIPTION
  This function inserts an item before a specified item on a queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Input item is inserted before input item.
===========================================================================*/
void q_insert_d(
   q_type    *q_ptr,          /* Ptr to the queue */
   q_link_type  *q_insert_ptr,   /* Ptr to link of item to insert */
   q_link_type  *q_item_ptr      /* Ptr to link item to insert before */
)
{
   ASSERT( q_ptr        != NULL );
   ASSERT( q_insert_ptr != NULL );
   q_lock_s( q_ptr );
   q_insert_ptr->next_ptr = q_item_ptr;

   q_insert_ptr->prev_ptr = q_item_ptr->prev_ptr;
   if(q_item_ptr->prev_ptr != NULL)
   {
      q_item_ptr->prev_ptr->next_ptr = q_insert_ptr;
   }
   q_item_ptr->prev_ptr = q_insert_ptr;

   q_ptr->cnt++;
   q_unlock_s( q_ptr );
} /* END q_insert_d */

/*===========================================================================
FUNCTION Q_INSERT_AFTER_S

DESCRIPTION
  This function inserts an item after a specified item on a queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Input item is inserted after input item.
===========================================================================*/
void q_insert_after_s(
  q_type       *q_ptr,          /* Ptr to the queue */
  q_link_type  *q_insert_ptr,   /* Ptr to link of item to insert */
  q_link_type  *q_item_ptr      /* Ptr to link item to insert after */
)
{

  ASSERT( q_ptr != NULL );
  ASSERT( q_insert_ptr != NULL );
   
  q_lock_s( q_ptr );
    
  q_insert_ptr->next_ptr = q_item_ptr->next_ptr;
  q_item_ptr->next_ptr = q_insert_ptr;

  /* Tail_ptr is q_ptr->link.prev_ptr */
  if (q_ptr->link.prev_ptr == q_item_ptr)
  {
	  q_ptr->link.prev_ptr = q_insert_ptr;
  } 
  q_ptr->cnt++;
  q_unlock_s( q_ptr );
 } /* END q_insert_after_s */

/*===========================================================================
FUNCTION Q_DELETE_S

DESCRIPTION
  This function removes an item from a specified queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

RETURN VALUE
  FALSE : if the item is not found in the queue.
  TRUE  : if the item is found and removed from the queue.

SIDE EFFECTS
  Input item is deleted from the queue.
===========================================================================*/
boolean q_delete_s(
      q_type       *q_ptr,         /* Ptr to the Queue */
      q_link_type  *q_delete_ptr   /* Ptr to link of item to delete */
)
{
   q_link_type *link_ptr;
   int qcount;
   boolean item_in_q = FALSE;

   ASSERT( q_ptr        != NULL );
   ASSERT( q_delete_ptr != NULL );
   q_lock_s( q_ptr );

   for( qcount = q_ptr->cnt,
        link_ptr           =  &q_ptr->link;
        link_ptr->next_ptr != q_delete_ptr && qcount > 0;
        link_ptr           =  link_ptr->next_ptr, qcount--);

   ASSERT( link_ptr != NULL );
   if(qcount > 0)
   {
     link_ptr->next_ptr = q_delete_ptr->next_ptr;
     /* Tail_ptr is q_ptr->link.prev_ptr */
     if(q_ptr->link.prev_ptr == q_delete_ptr)
     {
    	 q_ptr->link.prev_ptr = link_ptr;
     }
     item_in_q = TRUE;
   }
    q_ptr->cnt--;
    q_delete_ptr->next_ptr = NULL;
    q_unlock_s( q_ptr );
    return item_in_q;
} /* END q_delete_s */

/*===========================================================================
FUNCTION Q_DELETE_D

DESCRIPTION
  This function removes an item from a specified queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

RETURN VALUE
  FALSE : if the item is not found in the queue.
  TRUE  : if the item is found and removed from the queue.

SIDE EFFECTS
  Input item is deleted from the queue.
===========================================================================*/
boolean q_delete_d(
      q_type       *q_ptr,         /* Ptr to the Queue */
      q_link_type  *q_delete_ptr   /* Ptr to link of item to delete */
)
{
   boolean item_in_q = FALSE;

   ASSERT( q_ptr        != NULL );
   ASSERT( q_delete_ptr != NULL );
   q_lock_s( q_ptr );
    q_delete_ptr->prev_ptr->next_ptr = q_delete_ptr->next_ptr;
    q_delete_ptr->next_ptr->prev_ptr = q_delete_ptr->prev_ptr;
	 q_delete_ptr->prev_ptr = NULL;
    item_in_q = TRUE;
    q_ptr->cnt--;
    q_delete_ptr->next_ptr = NULL;
    q_unlock_s( q_ptr );
    return item_in_q;
} /* END q_delete_ext_d */

/*===========================================================================
FUNCTION Q_LINEAR_SEARCH_S

DESCRIPTION
  Given a comparison function, this function traverses the elements in
  a queue, calls the compare function, and returns a pointer to the
  current element being compared if the user passed compare function
  returns non zero.

  The user compare function should return 0 if the current element is
  not the element in which the compare function is interested.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

  The user's queue elements must have q_link_type as the first element
  of the queued structure.

RETURN VALUE
  A pointer to the found element

SIDE EFFECTS
  None.
===========================================================================*/
void* q_linear_search_s(
   q_type             *q_ptr,
   q_compare_func_type compare_func,
   void               *compare_val
)
{
   q_generic_item_type *item_ptr = NULL;

   ASSERT( compare_func != NULL );
   item_ptr = (q_generic_item_type*)q_check_s( q_ptr );

   while( item_ptr != NULL )
   {
      if( compare_func( item_ptr, compare_val ) != 0 )
      {
         return item_ptr;
      }
      item_ptr = (q_generic_item_type*)q_next_s( q_ptr, &item_ptr->link );
   } /* END while traversing the queue */

   return NULL;
} /* END q_linear_search */

/*===========================================================================
FUNCTION Q_RETRIEVE_AFTER_S_LL

DESCRIPTION
  This function returns a pointer to the item after the given item  on the 
  queue and removes it from the queue. 

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

RETURN VALUE
  A pointer to the item after the given item on the queue. If the end 
  of the queue is reached, then NULL is returned.

SIDE EFFECTS
  The function is not THREAD SAFE
===========================================================================*/
void* q_retrieve_after_s_ll(
   q_type       *q_ptr,
   q_link_type  *q_link_ptr
)
{
   q_link_type *q_ret_ptr = NULL;

   ASSERT( q_ptr != NULL );
   ASSERT( q_link_ptr != NULL );
   if ((void *) q_link_ptr->next_ptr == NULL) return NULL;

   if( q_ptr->cnt > 0 )
   {     
     if (q_link_ptr->next_ptr != (q_link_type *)q_ptr)
     { 
       q_ret_ptr = q_link_ptr->next_ptr;

       /* Remove the item from the queue and update pointers*/
       q_link_ptr->next_ptr = q_ret_ptr->next_ptr;
       /* for single_link queue q_ptr->link.prev_ptr is used as tail_ptr */
       if(q_ptr->link.prev_ptr == q_ret_ptr)
       {
    	   q_ptr->link.prev_ptr = q_link_ptr;
       }
       q_ptr->cnt--;
    /* Mark item as no longer in a queue. */
       q_ret_ptr->next_ptr = NULL;
     }

   }
   return (void*) q_ret_ptr;

} /* END q_retrieve_after_s_ll */

/*===========================================================================
FUNCTION Q_CHAIN_GET_S_LL

DESCRIPTION
  Dequeues a chain of items from queue.
	  q_chain_get_s - Dequeues a chain of items from singly linked queue
	  Queue will not be locked
  @param[in] q_ptr    Pointer to the item to be enqueued.
  @param[in] num_items Number of items to dequeue

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

RETURN VALUE
  Head of dequeued chain of items
	Number of items in chain is MIN(num_items,total_items in queue)

SIDE EFFECTS
  Output chain to be delinked using q_chain_delink_s() function
===========================================================================*/
void* q_chain_get_s_ll(q_type  *q_ptr, uint32 num_items ){

   ASSERT( q_ptr != NULL );
   q_link_type  *link_ptr = NULL;
   q_link_type  *tail_ptr =  NULL;
   /* No validity checks made in inline functions
   Validity checks to be made in client code */
   if ((q_ptr->cnt !=0)&&(num_items>0))
   {
     link_ptr = q_ptr->link.next_ptr;	
     if( q_ptr->cnt > num_items )
     {
        q_ptr->cnt-=num_items;
        tail_ptr = link_ptr;
        num_items--;
        while(num_items >0)
        {
          tail_ptr = tail_ptr->next_ptr;
          num_items--;
        }
        q_ptr->link.next_ptr = tail_ptr->next_ptr;
        tail_ptr->next_ptr = NULL;	        
     }
     else
     {
       /* empty queue */
       q_ptr->link.prev_ptr->next_ptr = NULL;
       q_ptr->link.next_ptr = &q_ptr->link;
       q_ptr->link.prev_ptr = &q_ptr->link;
       q_ptr->cnt = 0;
     }
   }
   return link_ptr;
} /* END q_chain_get_s_ll */

