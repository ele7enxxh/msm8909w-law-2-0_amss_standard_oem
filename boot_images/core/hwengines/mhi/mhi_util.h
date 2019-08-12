#ifndef MHI_UTIL_H
#define MHI_UTIL_H
/**
  @file mhi_util.h
  @brief
  This file defines the interface for the MHI utility API's 
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/22/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "comdef.h"  

/** Defines generic linked list */ 
typedef struct _list_t
{
  struct _list_t *next;
  struct _list_t *prev;
}list_t;

/** 
 * @brief      mhi_q_init
 *
 *             Initializes the specified queue 
 *              
 * 
 * @param[in]  *q - Queue to initialize 
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_q_init(list_t *q);

/** 
 * @brief      mhi_q_queue
 *
 *             Queues an element on to the specified queue
 *              
 * 
 * @param[in]  *q - Queue to queue on 
 * @param[in]  *i - Element to queue
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR
 * 
 * @sideeffect None 
 */
uint32 mhi_q_queue(list_t *q, list_t *i);

/** 
 * @brief      mhi_q_queue_head
 *
 *             Queues an element to the head of a queue 
 *              
 * 
 * @param[in]  *q - Queue to use
 * @param[in]  *i - Element to queue
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR 
 * 
 * @sideeffect None 
 */
int32 mhi_q_queue_head(list_t *q, list_t *i);

/** 
 * @brief      mhi_q_insert_before
 *
 *             Insert a new element to before the specified queue element
 *              
 * 
 * @param[in]  *at - Element to insert before
 * @param[in]  *i  - Element to insert
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR 
 * 
 * @sideeffect None 
 */
int32 mhi_q_insert_before(list_t *at, list_t *i);

/** 
 * @brief      mhi_q_peek_head
 *
 *             Sets the paramater **i to the next element of the specified queue 
 *              
 * 
 * @param[in]  *q   - Queue 
 * @param[out]  **i - Set to next element on queue
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR 
 * 
 * @sideeffect None 
 */
int32 mhi_q_peek_head(list_t *q, list_t **i);

/** 
 * @brief      mhi_q_dequeue
 *
 *             Dequeues an element from the queue and sets the paramater **i to point to that element 
 *              
 * 
 * @param[in]  *q  - Queue 
 * @param[out] **i - Set to the next element on the queue  
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR 
 * 
 * @sideeffect None 
 */
int32 mhi_q_dequeue(list_t *q, list_t **i);

/** 
 * @brief      mhi_q_get_next
 *
 *             Gets the next element on queue starting at the **current element. Will not remove 
 *             the element off the queue
 *              
 * 
 * @param[in]  *q - Queue 
 * @param[in/out]  **current - Specifies the current element, and is set to the next element
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR 
 * 
 * @sideeffect None 
 */
int32 mhi_q_get_next(list_t *q, list_t** current);

/** 
 * @brief      mhi_q_remove
 *
 *             Remove the specified element from the queue
 *              
 * 
 * @param[in]  *i - Element to remove 
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR 
 * 
 * @sideeffect None 
 */
int32 mhi_q_remove(list_t *i); 

/** 
 * @brief      mhi_q_is_queue_empty
 *
 *             Returns TRUE if the queue is empty
 *              
 * 
 * @param[in]  *q - Queue
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             TRUE if queue is empty
 *             FALSE if not 
 *             ERROR if q is invalid 
 * 
 * @sideeffect None 
 */
uint32 mhi_q_is_queue_empty(list_t *q);

#endif
