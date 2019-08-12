#ifndef QUEUE_SERVICES_H
#define QUEUE_SERVICES_H
/**
  @file queue_services.h
  @brief This file contains optimized queue functions for data path services.
  The queue services are built over the commonly used <queue.h> structures 
  (types and declarations).
*/
/*===========================================================================

            Q U E U E    S E R V I C E S   - SINGLE LINK AND DOUBLE LINK                
            
DESCRIPTION
 This file contains types and declarations associated with 
 SINGLY LINKED and DOUBLY LINKED Queue Services.

Copyright (c) 1992-2015 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/memory/qservices/queue_services.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/14   sh      Doubly Linked queue without feature flags used by DSM
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "queue.h"


/* Uses CORE BSP queue structures, q_type and q_link_type */
/* Both doubly linked and singly linked queue implemented without
   feature flags */
/* Optimized for data path performance with lockless and inline queue 
   funtions */


/* Keywords for queue services functions */
/* q_<op>_<s/d><_ll><_i>
  - <op> - queue operations put/get/insert...
  
  - <s/d> - Singly linked/ Doubly linked.
              operations not interchangeable, 
              type of queue is fixed during queue initialization   

  - <_ll> - Lockless functions, does not use mutex locks, hence not threadsafe
            Default/ without <_ll> - uses mutex locks
              
  - <_i> - inline functions - In Inline functions,
              validity checks of parameters are not done.
   
Unlike queue.c implementation,Queue services functions provide different
implementation of queue and it is up to the client to determine
whether queue operations need to be thread safe and if so,
initialize and use mutex locks. If queue functions in queue services are
improperly used, it can lead to unexpected behaviour.


Eg. Use Case for double link queue that needs to be thread safe:
q_init_d (Initialize Singly link queue, lock_init=1
q_put_d (put elements in queue)
q_get_d (get elements from queue)
q_destroy_d(Destroy single link queue,lock_destroy=1)

Eg. Use Case for single link queue that need not be thread safe:
q_init_s (Initialize Singly link queue, lock_init=0
q_put_s_ll_i (put elements in queue, lockless, inline)
q_get_s_ll_i (get elements from queue, lockless, inline)
q_destroy_s(Destroy single link queue,lock_destroy=0)

*/

/* - Optional mutex locks - Mutex locks for queue operations are made optional as the the clients 
   may add their own mutex locks for client operations along with queue
   operations. q_lock_s()/q_unlock_s() can be explicitly called, if needed.
   
   - Optional error checking - Inline functions 
   expects client code to do error checking for highly used data paths */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/** @addtogroup utils_queue
@{ */

/** @brief 
  This structure is interchangeable with q_link_struct for
  all single link queue implementation
*/
typedef struct q_link_struct_g
{
  struct q_link_struct_g  *next_ptr;
  union
  {
    struct q_link_struct_g  *prev_ptr; /* used by queue services for double link */
	void* usr_data; /* used by clients for usr data for single link */
  };
} q_link_g_type;

#define Q_LINK_G(link) ((q_link_g_type*)(link))
#define Q_LINK(link) ((q_link_type*)(link))

/*===========================================================================

                             Macro Definitions

===========================================================================*/
/**
  Determines if the item passed in is in a queue.

  @param[in] q_link_ptr Pointer to the item to be tested.

  @return
  TRUE -- Item is already in a queue. \n
  FALSE -- Item is not in a queue.

  @dependencies
  None.
*/
#define Q_ALREADY_QUEUED_S( q_link_ptr ) \
   (q_link_ptr->next_ptr != NULL)
#define Q_ALREADY_QUEUED_D( q_link_ptr ) \
	   (q_link_ptr->next_ptr != NULL)


/*===========================================================================

                            Function Declarations

===========================================================================*/
#ifdef __cplusplus
   extern "C" {
#endif

/**
  Initializes a queue. This function must be called for each
  queue before using the queue with other Queue Utility Services.
  q_init_s - Initializes singly linked queue
  q_init_d - Initializes doubly linked queue

  @param[in] q_ptr Pointer to the queue to initialize.
  @param[in] lock_init Boolean to initialize queue mutex lock

  @return
  A pointer to the initialized queue.

  @dependencies
  None.
*/
q_type* q_init_s ( q_type *q_ptr ,boolean lock_init);
q_type* q_init_d ( q_type *q_ptr ,boolean lock_init);


/**
  Locks a queue explicitly.
  
  q_lock_s - Lock singly linked queue link
  q_lock_d - Lock doubly linked queue link
 
  @param[in] q_ptr Pointer to the queue to initialize.

  @return
  None

  @dependencies
  None.
*/
#define q_lock_d(X) q_lock_s(X)
void q_lock_s( q_type *q_ptr );

/**
  UnLocks a queue explicitly.
  
  q_unlock_s - Lock singly linked queue link
  q_unlock_d - Lock doubly linked queue link
 
  @param[in] q_ptr Pointer to the queue to initialize.

  @return
  None

  @dependencies
  None.
*/

#define q_unlock_d(X) q_unlock_s(X)
void q_unlock_s( q_type *q_ptr );

/**
  Enqueues an item to a specified queue using a specific link.
    q_put_s - Enqueues to singly linked queue
    q_put_d - Enqueues to doubly linked queue
 
  @param[in] q_ptr    Pointer to the item to be enqueued.
  @param[in] link_ptr Pointer to the link where the item is to be placed.
  

  @return
  None.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init(). \n
  The specified link field of the item must have been initialized previously 
  via a call to q_link().

  @sideeffects
  The specified item is placed at the tail of a specific queue.
*/
void  q_put_s  ( q_type *q_ptr, q_link_type *link_ptr );
void  q_put_d  ( q_type *q_ptr, q_link_type *link_ptr );

static __inline void q_put_s_ll_i  ( q_type *q_ptr, q_link_type *link_ptr )
{
  /* No validity checks made in inline functions
     Validity checks to be made in client code */
  link_ptr->next_ptr = (q_link_type *)&q_ptr->link;

  /* for single_link queue, q_ptr->link.prev_ptr is used as tail_ptr */
  q_ptr->link.prev_ptr->next_ptr = link_ptr;
  q_ptr->link.prev_ptr 		  = link_ptr; 
	
  q_ptr->cnt++;
}

/**
  Enqueues all items from a specified (source) queue to a 
  specific (target) queue.
  q_m_put_s() - for single link queue,
  q_m_put_d() - for double link queue
 
  @param[in] target_q_ptr Pointer to the target queue.
  @param[in] source_q_ptr Pointer to the source queue.

  @return
  None.

  @dependencies
  The specified queues must have been initialized previously via a call
  to q_init().

  @sideeffects
  The items in the specified source queue are placed at the tail of a  
  specific target queue.
*/
void q_m_put_s(
   q_type       *target_q_ptr,
   q_type       *source_q_ptr
);
void q_m_put_d(
   q_type       *target_q_ptr,
   q_type       *source_q_ptr
);



/**
  Removes an item from the head of a specified queue.

  @param[in] q_ptr Pointer to the queue.
  
  q_get_s - Dequeues from singly linked queue
  q_get_d - Dequeues from doubly linked queue

  @return
  Returns a pointer to the dequeued item, or NULL if the specified queue
  is empty.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void* q_get_s ( q_type *q_ptr );
void* q_get_d ( q_type *q_ptr );

static __inline void* q_get_s_ll_i(q_type  *q_ptr ){
   q_link_type  *link_ptr = NULL;
   /* No validity checks made in inline functions
   Validity checks to be made in client code */

   if( q_ptr->cnt > 0 )
   {
      link_ptr = q_ptr->link.next_ptr;
      q_ptr->link.next_ptr = link_ptr->next_ptr;
      /* for single_link queue, q_ptr->link.prev_ptr is used as tail_ptr */
      if (q_ptr->link.prev_ptr == link_ptr)
      {
    	  q_ptr->link.prev_ptr = &q_ptr->link;
      } 
      q_ptr->cnt--;
      link_ptr->next_ptr = NULL;	
   }
   return link_ptr;
} /* END q_get_lockless */

/**
  Removes an item from the tail of a specified queue.
  
  q_last_get_s() - for single link queue,
  q_last_get_d() - for double link queue

  @param[in] q_ptr Pointer to the queue.

  @return
  Returns a pointer to the dequeued item, or NULL if the specified queue
  is empty.
 
  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void* q_last_get_s ( q_type *q_ptr );
void* q_last_get_d ( q_type *q_ptr );

/**
  Set user data on a single link queue node
  
  q_set_user_data_s() - for single link queue,
  for double link queue, not valid
 
  @param[in] q_ptr Pointer to the queue.

  @return
  User data value

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
static __inline void q_set_usr_data_s( q_link_type *q_link_ptr, void* data )
{
/* No validity checks made in inline functions
   Validity checks to be made in client code */
  (Q_LINK_G(q_link_ptr))->usr_data = data;
} 

/**
  Get user data on a single link queue node
  
  q_get_user_data_s() - for single link queue,
  for double link queue, not valid
 
  @param[in] q_ptr Pointer to the queue.

  @return
  User data value

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
static __inline void* q_get_usr_data_s( q_link_type *q_link_ptr )
{
/* No validity checks made in inline functions
   Validity checks to be made in client code */
  return (Q_LINK_G(q_link_ptr))->usr_data;
} 


/**
  Initializes a specified link. This function must be called for each
  link before using the link with other Queue Utility Services.
  
  q_link_s - Initializes singly linked queue link
  q_link_d - Initializes doubly linked queue link
 
  @param[in] item_ptr Item to be initialized.
  @param[in] link_ptr Link to be initialized.

  @return
  A pointer to the initialized link.

  @dependencies
  None.
*/
q_link_type*  q_link_s (q_link_type *link_ptr );
q_link_type*  q_link_d (q_link_type *link_ptr );
static __inline void  q_link_s_i( q_link_type *link_ptr )
{
  /* No validity checks made in inline functions
	 Validity checks to be made in client code */
  link_ptr->next_ptr = NULL;
  q_set_usr_data_s(link_ptr,NULL);
  /* No return VALUE */
}

/**
  Make a chain of links, to use in q_put_chain_s() function
  
  @param[in] chain_tail_ptr Tail of chain
  @param[in] link_ptr Link to be added to tail of chain

  @return
  None

  @dependencies
  None.
*/
static __inline void  q_chain_link_s_i( q_link_type *chain_tail_ptr ,q_link_type *link_ptr )
{
  /* No validity checks made in inline functions
	 Validity checks to be made in client code */	
  chain_tail_ptr->next_ptr = link_ptr;  
}

/**
  Remove iteratively from a chain of items
  
  @param[in] chain_head_ptr Head of chain
  @param[out] chain_head_ptr updated to new head

  @return
  Link removed from chain _head_ptr

  @dependencies
  None.
*/
static __inline void* q_chain_delink_s_i( q_link_type **chain_head_ptr )
{
  /* No validity checks made in inline functions
	 Validity checks to be made in client code */	

  q_link_type* next = *chain_head_ptr; 
  if (next !=NULL)
  {
    *chain_head_ptr = next->next_ptr;
    next->next_ptr = NULL;
  }
  return next;  
}
/**
  Iterate through chain of links,

  @param[in] chain_ptr chain

  @return
     Head of chain_ptr

  @dependencies
  None.
*/
static __inline q_link_type*  q_chain_next_s_i( q_link_type *chain_ptr )
{
  /* No validity checks made in inline functions
	 Validity checks to be made in client code */
  return chain_ptr->next_ptr;
}

/**
  Enqueues a chain of items to a specified queue using 
  head and tail of the chain.
    q_chain_put_s - Enqueues a chain of items to singly linked queue
 
  @param[in] q_ptr    Pointer to the item to be enqueued.
  @param[in] chain_head_link_ptr Pointer to the item of the chain's head.
  @param[in] chain_tail_link_ptr Pointer to the item of the chain's tail.
  @param[in] num_items_in_chain Number of items in input chain

  @return
  None.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init(). 
  Chain should have been linked with q_chain_link_s() function

  @sideeffects
  The chain is placed at the tail of a specific queue.
*/

static __inline void q_chain_put_s_ll_i  ( q_type *q_ptr, 
	                    q_link_type *chain_head_link_ptr,
	                    q_link_type *chain_tail_link_ptr,
	                     uint32 num_items_in_chain)
{
  /* No validity checks made in inline functions
     Validity checks to be made in client code */
  chain_tail_link_ptr->next_ptr = (q_link_type *)&q_ptr->link;

  /* for single_link queue, q_ptr->link.prev_ptr is used as tail_ptr */
  q_ptr->link.prev_ptr->next_ptr = chain_head_link_ptr;
  q_ptr->link.prev_ptr 		  = chain_tail_link_ptr; 
	
  q_ptr->cnt+=num_items_in_chain;
}

/**
  Dequeues a chain of items from queue.
    q_chain_get_s - Dequeues a chain of items from singly linked queue
 
  @param[in] q_ptr    Pointer to the item to be enqueued.
  @param[in] num_items Number of items to dequeue

  @return
  Head of dequeued chain of items
  Number of items in chain is MIN(num_items,total_items in queue)

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().  

  @sideeffects
  Output chain to be delinked using q_chain_delink_s() function
*/
void* q_chain_get_s_ll(q_type  *q_ptr, uint32 num_items );

/**
  Returns the number of items currently queued on a specified queue.
  
  q_cnt_s() - for single link queue,
  q_cnt_d() - for double link queue
 
  @param[in] q_ptr Pointer to the queue.

  @return
  The number of items.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
static __inline int q_cnt_s(q_type  *q_ptr){
	/* No validity checks made in inline functions
   Validity checks to be made in client code */
   return q_ptr->cnt;
} 
static __inline int q_cnt_d(q_type  *q_ptr){
	/* No validity checks made in inline functions
   Validity checks to be made in client code */
   return q_ptr->cnt;
} 

/**
  Returns a pointer to the data block at the head of the queue. The data 
  block is not removed from the queue.
  
  q_check_s() - for single link queue,
  q_check_d() - for double link queue

  @param[in] q_ptr Pointer to the queue.

  @return
  Returns a pointer to the queue item, or NULL if the specified queue
  is empty.
 
  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/

void* q_check_s(q_type  *q_ptr);
void* q_check_d(q_type  *q_ptr);

static __inline void* q_check_s_ll_i (q_type  *q_ptr) {
	/* No validity checks made in inline functions
	   Validity checks to be made in client code */
   if( q_ptr->cnt > 0 )
   {
      return q_ptr->link.next_ptr;
   }
   else
   {
     return NULL;
   }
}
static __inline void* q_check_d_ll_i (q_type  *q_ptr) {
	/* No validity checks made in inline functions
	   Validity checks to be made in client code */
   if( q_ptr->cnt > 0 )
   {
      return q_ptr->link.next_ptr;
   }
   else
   {
     return NULL;
   }
}

/**
  Returns a pointer to the data block at the tail of the queue. The data 
  block is not removed from the queue.

  q_last_check_s() - for single link queue,
  q_last_check_d() - for double link queue
 
  @param[in] q_ptr Pointer to the queue.

  @return
  Returns a pointer to the queue item, or NULL if the specified queue
  is empty.
 
  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
static __inline void* q_last_check_s_ll_i ( q_type *q_ptr ){
  if ( q_ptr->cnt > 0 )
  {
	/* for single_link queue, q_ptr->link.prev_ptr is used as tail_ptr */
	return q_ptr->link.prev_ptr; 
  }
  else
  {
    return NULL;
  }
}
static __inline void* q_last_check_d_ll_i ( q_type *q_ptr ){
  if ( q_ptr->cnt > 0 )
  {
	/* for single_link queue, q_ptr->link.prev_ptr is used as tail_ptr */
	return q_ptr->link.prev_ptr; 
  }
  else
  {
    return NULL;
  }
}


/**
  Returns a pointer to the next item on the queue.

  q_next_s() - for single link queue,
  q_next_d() - for double link queue, not implemented

  @param[in] q_ptr    Pointer to the queue.
  @param[in] link_ptr Pointer to the link on the queue.

  @return
  Returns a pointer to the next item on the queue, or NULL if the end of 
  the queue is reached.
 
  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void* q_next_s  ( q_type *q_ptr, q_link_type *q_link_ptr );
static __inline void* q_next_s_ll_i( q_type *q_ptr, q_link_type *q_link_ptr )
{

  /* No validity checks made in inline functions
     Validity checks to be made in client code */
  if(q_link_ptr->next_ptr != (q_link_type *) q_ptr )
  {
    return q_link_ptr->next_ptr;
  }
  else
  {
    return NULL;
  }
}

/**
  Inserts an item before a specified item on the queue.

  q_insert_s() - for single link queue,
  q_insert_d() - for double link queue
  q_chain_insert_after_s() - inserts a chain of items in single linkled queue

  @param[in] q_ptr        Pointer to the queue.
  @param[in] q_insert_ptr Pointer to where the item is to be inserted.
  @param[in] q_item_ptr   Pointer to the item to insert.

  @return
  None.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void q_insert_s  (q_type *q_ptr, q_link_type *q_insert_ptr, q_link_type *q_item_ptr );
void q_insert_d  (q_type *q_ptr, q_link_type *q_insert_ptr, q_link_type *q_item_ptr );

/**
  Inserts an item after a specified item on the queue.

  q_insert_after_s - Supported only for single link

  @param[in] q_ptr        Pointer to the queue.
  @param[in] q_insert_ptr Pointer to where the item is to be inserted.
  @param[in] q_item_ptr   Pointer to the item to insert.

  @return
  None.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void q_insert_after_s(
   q_type     *q_ptr,
   q_link_type  *q_insert_ptr,
   q_link_type  *q_item_ptr
);
static __inline void q_insert_after_s_ll_i(
  q_type       *q_ptr,          /* Ptr to the queue */
  q_link_type  *q_insert_ptr,   /* Ptr to link of item to insert */
  q_link_type  *q_item_ptr      /* Ptr to link item to insert after */
)
{  
  /* No validity checks made in inline functions
     Validity checks to be made in client code */
  q_insert_ptr->next_ptr = q_item_ptr->next_ptr;
  q_item_ptr->next_ptr = q_insert_ptr;
  /* for single_link queue, q_ptr->link.prev_ptr is used as tail_ptr */
  if (q_ptr->link.prev_ptr == q_item_ptr)
  {
	  q_ptr->link.prev_ptr = q_insert_ptr;
  } 
  q_ptr->cnt++;
} /* END q_insert_after_lockless */

static __inline void q_chain_insert_after_s_ll_i(
  q_type       *q_ptr,          /* Ptr to the queue */
  q_link_type  *q_insert_chain_head_ptr,   /* Ptr to chain head item to insert */
  q_link_type  *q_insert_chain_tail_ptr,	 /* Ptr to chain head item to insert */
  uint32  num_items,     /* Number of chain items*/
  q_link_type  *q_item_ptr      /* Ptr to link item to insert after */
)
{  
  /* No validity checks made in inline functions
     Validity checks to be made in client code */
  q_insert_chain_tail_ptr->next_ptr = q_item_ptr->next_ptr;
  q_item_ptr->next_ptr = q_insert_chain_head_ptr;
  /* for single_link queue, q_ptr->link.prev_ptr is used as tail_ptr */
  if (q_ptr->link.prev_ptr == q_item_ptr)
  {
	  q_ptr->link.prev_ptr = q_insert_chain_tail_ptr;
  } 
  q_ptr->cnt+=num_items;
} /* END q_insert_after_lockless */


/**
  Removes an item from a specified queue.

  q_delete_s - for single link queue
  q_delete_d - for double link queue

  @param[in] q_ptr        Pointer to the queue.
  @param[in] q_delete_ptr Pointer to the item to be removed.

  @return
  TRUE -- Item was found and removed from the queue. \n
  FALSE -- Item was not found in the queue.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
boolean q_delete_s  ( q_type *q_ptr, q_link_type *q_delete_ptr );
boolean q_delete_d  ( q_type *q_ptr, q_link_type *q_delete_ptr );


/**
  Given a comparison function, this function traverses the elements in
  a queue, calls the compare function, and returns a pointer to the
  current element being compared if the user-passed compare function
  returns nonzero.

  The user compare function is to return 0 if the current element 
  is not pertinent to the compare function.
 
  @param[in] q_ptr        Pointer to the queue to be traversed.
  @param[in] compare_func Comparison function to be used.
  @param[in] compare_val  Pointer to the value against which to compare.

  @return
  A pointer to the element.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init(). \n
  The user's queue elements must have q_link_type as the first element
  of the queued structure.
*/
void* q_linear_search_s(
  q_type             *q_ptr,  
  q_compare_func_type compare_func,
  void               *compare_val
);


/**
  Removes and returns an item after the specified item.

  @param[in] q_ptr    Pointer to the queue.
  @param[in] link_ptr Pointer to the link on the queue.

  @return
  Returns a pointer to the next item on the queue, and removes it from the queue, 
  or NULL if the end of the queue is reached.
 
  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void* q_retrieve_after_s_ll(q_type *q_ptr,q_link_type *q_link_ptr);

/**
  Destroys a specified queue. This function is to be called if the queue 
  is no longer required.
  
  q_destroy_s - for single link queue
  q_destroy_d - for double link queue
  
  @param[in] q_ptr Pointer to the queue to be destroyed.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  Elements in the queue will not be accessible. To avoid leaks, it is user's 
  responsibility to deallocate the memory allocated for the queue and its 
  elements.
*/
void q_destroy_s(
   q_type  *q_ptr,
   boolean lock_destroy
);
void q_destroy_d(
   q_type  *q_ptr,
   boolean lock_destroy
);



#ifdef __cplusplus
   }
#endif

/** @} */ /* end_addtogroup utils_queue */
#endif /* QUEUE_SERVICES_H */
