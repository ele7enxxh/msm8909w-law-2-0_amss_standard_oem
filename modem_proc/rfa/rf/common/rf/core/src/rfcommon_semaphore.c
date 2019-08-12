/*! 
  @file
  rfcommon_semaphore.c
 
  @brief
  This file includes functions pertaining to semaphore for RF COMMON Operation
 
  @addtogroup RFCOMMON_SEMAPHORE
  @{
*/
 
/*==============================================================================

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_semaphore.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/19/12   aki     Add deinit functions 
07/23/12   Saul    RF COMMON. MDSP/MSGR framework.
06/29/12   spa     Compiler warning fix  
05/09/12   aro     Code to check if the sem_token from SemPool is in used_list
                   or not before doing POST
05/09/12   aro     Added fields in sem token to indicate if the token is
                   part of SemPool
05/09/12   aro     Bug fix in semaphore POP
05/08/12   aro     Code to pop and push the semaphor items from and to the
                   semaphore pool
05/08/12   aro     Initial support for semaphore POOL
05/08/12   aro     Renamed function to sem_create and senm_destroy
05/02/12   aro     Support to detect POST-BEFORE-WAIT
04/03/12   aro     Perform SEM_WAIT for any sem count
03/28/12   aro     Documentation update
02/15/12   aro     Added support for Semaphore Wait and Post logging
01/05/12   aro     F3 message fix
12/08/11   aro     Added descriptive error message
11/10/11   aro     Defined CDMA semaphore as generic CDMA type
10/30/11   aro     Added interface to post a semaphore so that the blocked 
                   call with sempahore wait can be ended 
10/30/11   aro     Added interface to wait on semaphore token until the 
                   semaphore is posted.
10/30/11   aro     Added function to de-initialize semaphore 
10/30/11   aro     Added function to initialize semaphore 
10/29/11   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfcommon_semaphore.h"
#include "rfcommon_msg.h"
#include "rfcommon_data.h"
#include "err.h"

/*============================================================================*/
/*!
  @name Logging

  @brief
  This section contains functions used for to Semaphore logging.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Log the Semaphore Wait
 
  @details
  This function will log all the semaphore that are waited for. the 
  logging will done on circular buffer.
 
  @param sem_data
  Pointer to the Semaphore data structure which contain log buffer
 
  @param sem_token
  Pointer to semaphore token
*/
static void
rfcommon_semaphore_log_wait
(
  rfcommon_semaphore_data_type *sem_data,
  rfcommon_semaphore_token_type *sem_token
)
{
  /* Since the SemPost log is a circular buffer, reset the index, if the
  current entry is the last one.*/
  if( sem_data->wait_idx >= RFCOMMON_SEMA_TOKEN_LIST_MAX )
  {
    sem_data->wait_idx = 0;
  }

  /* Log the current semaphore token */
  sem_data->sem_wait_log[sem_data->wait_idx].sem_token = (uint32)sem_token;

  /* Log the time when the semaphore was posted */
  sem_data->sem_wait_log[sem_data->wait_idx].token_time = timetick_get();

  if( sem_data->dbg_msg_en == TRUE )
  {
    RF_MSG_3( RF_HIGH, "rfcommon_semaphore_log_wait: ID %d Sem 0x%x Time %d", 
              sem_data->wait_idx,
              sem_data->sem_wait_log[sem_data->wait_idx].sem_token,
              sem_data->sem_wait_log[sem_data->wait_idx].token_time );
  }

  /* Increment the Log Index */
  sem_data->wait_idx = sem_data->wait_idx + 1;

} /* rfcommon_semaphore_log_wait */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Log the Semaphore posted
 
  @details
  This function will log all the semaphore posted. the logging
  will done on circular buffer
 
  @param sem_data
  Pointer to the Semaphore data structure which contain log buffer
 
  @param sem_token
  Pointer to semaphore token
*/
static void
rfcommon_semaphore_log_post
(
  rfcommon_semaphore_data_type *sem_data,
  rfcommon_semaphore_token_type *sem_token
)
{
  /* Since the SemPost log is a circular buffer, reset the index, if the
  current entry is the last one.*/
  if( sem_data->post_idx >= RFCOMMON_SEMA_TOKEN_LIST_MAX )
  {
    sem_data->post_idx = 0;
  }

  /* Log the current semaphore token */
  sem_data->sem_post_log[sem_data->post_idx].sem_token = (uint32)sem_token;

  /* Log the time when the semaphore was posted */
  sem_data->sem_post_log[sem_data->post_idx].token_time = timetick_get();

  if( sem_data->dbg_msg_en == TRUE )
  {
    RF_MSG_3( RF_HIGH, "rfcommon_semaphore_log_post: ID %d Sem 0x%x Time %d", 
              sem_data->post_idx,
              sem_data->sem_post_log[sem_data->post_idx].sem_token,
              sem_data->sem_post_log[sem_data->post_idx].token_time );
  }

  /* Increment the Log Index */
  sem_data->post_idx = sem_data->post_idx + 1;

} /* rfcommon_semaphore_log_post */

/*! @} */

/*============================================================================*/
/*!
  @name Semaphore Pool Doubly Linked List

  @brief
  Helper functions to be manage semaphore token Lists
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Insert Item to doubly linked list
  
  @details
  Inserts a rfcommon_semaphore_item_type into a doubly-linked list of 
  rfcommon_semaphore_item_type.  The item is inserted after the item passed
  in as item_before.
 
  This function will form an double linked list as shown in the diagram.
 
  @image html rfcommon_intelliceiver_doubly_linkedlist.jpg
 
  @param item_before
  Semaphore Item to which a new item is to be inserted
 
  @param item_to_insert
  New Semaphore Item to be inserted
*/
static void
rfcommon_semaphore_insert_item
(
  rfcommon_semaphore_item_type *item_before, 
  rfcommon_semaphore_item_type *item_to_insert
)
{

  item_to_insert->next = item_before->next;
  (item_before->next)->prev = item_to_insert;
  item_before->next = item_to_insert;
  item_to_insert->prev = item_before;

} /* rfcommon_semaphore_insert_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Remove semaphore Item to doubly linked list
  
  @details
  Removes a rfcommon_semaphore_item_type into a doubly-linked list of 
  rfcommon_semaphore_item_type. 
 
  @param item_to_remove
  Semaphore Item to be removed
*/
static void
rfcommon_semaphore_remove_item
(
  const rfcommon_semaphore_item_type *item_to_remove
)
{
  (item_to_remove->prev)->next = item_to_remove->next;
  (item_to_remove->next)->prev = item_to_remove->prev;

} /* rfcommon_semaphore_remove_item */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Find semaphore Item containg given item ID in used list
 
  @details
  This function locates a rfcommon_semaphore_item_type in the "used" list. The 
  item ID is used as the comparison key. If an item is found matching the 
  input comparison key, a pointer to that item is returned. If a match is not
  found a pointer to the sentinel is returned
 
  @param sem_data
  Pointer to the semaphore data
 
  @param sem_token
  Semaphore token to be used as a comparisson key to match in the semaphore
  item
 
  @return
  If an item is found matching the input comparison key, a pointer to that item 
  is returned. If a match is not found a pointer to the sentinel is returned.
*/
static rfcommon_semaphore_item_type*
rfcommon_semaphore_find_item
(
  rfcommon_semaphore_data_type *sem_data,
  rfcommon_semaphore_token_type *sem_token
)
{
  /* Initialize the variable to point to next item pointed by sentinel */
  rfcommon_semaphore_item_type *found = 
                         sem_data->pool.sentinel_used_items.next;

  /* Loop through the linked list to find the item containing the
  given semaphore */
  while ( found != &(sem_data->pool.sentinel_used_items) )
  {
    if ( &(found->sem_token) == sem_token )
    {
      /* Match is found. Therefore break */
      break;
    } /* if ( &(found->sem_token) == sem_token ) */
    else
    {
      /* No match, traverse through the linked list */
      found = found->next;
    } /* if ! ( &(found->sem_token) == sem_token ) */
  } /* while ( found != &(sem_data->pool.sentinel_used_items) ) */

  /* Check if the an item with tghe semaphore was found in the uised list.
  If not found, then return NULL*/
  if( found == &(sem_data->pool.sentinel_used_items) )
  {
    found = NULL;
  }

  /* return the item */
  return found;

} /* rfcommon_semaphore_find_item */

/*! @} */

/*============================================================================*/
/*!
  @name Semaphore Pool

  @brief
  Helper functions to manage semaphore pool
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Semaphore pool
  
  @details
  This will initialize semaphore pool linked list.
 
  @param sem_data
  Pointer to semaphore data
 
  @return
  Status of semaphore pool init
*/
static boolean
rfcommon_semaphore_pool_init
(
  rfcommon_semaphore_data_type *sem_data
)
{
  uint32 idx = 0;
  boolean ret_val = TRUE;

  /* build the items into the doubly-linked list */

  sem_data->pool.sentinel_spare_items.next = 
                    &(sem_data->pool.sentinel_spare_items);
  sem_data->pool.sentinel_spare_items.prev = 
                    &(sem_data->pool.sentinel_spare_items);

  /* Insert items into Spare list */
  for ( idx = 0; idx < RFCOMMON_SEMAPHORE_ITEM_NUM; idx++ )
  {
    ret_val &= rfcommon_semaphore_create( 
                            &(sem_data->pool.item_list[idx].sem_token) );
    sem_data->pool.item_list[idx].item_id = (rfcommon_semaphore_item_id_type)idx;
    sem_data->pool.item_list[idx].sem_token.from_sem_pool = TRUE;
    rfcommon_semaphore_insert_item( &(sem_data->pool.sentinel_spare_items),
                                   &sem_data->pool.item_list[idx] );
  } /* for ( idx = 0; idx < RFCOMMON_SEMAPHORE_ITEM_NUM; idx++ ) */

  sem_data->pool.sentinel_used_items.next = 
                     &(sem_data->pool.sentinel_used_items);
  sem_data->pool.sentinel_used_items.prev = 
                     &(sem_data->pool.sentinel_used_items);

  sem_data->pool.num_of_spare_items = RFCOMMON_SEMAPHORE_ITEM_NUM;

  rf_common_init_critical_section( &(sem_data->pool.critical_sect) );

  return ret_val;

} /* rfcommon_semaphore_pool_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Semaphore pool
  
  @details
  This will deinitialize semaphore pool linked list.
 
  @param sem_data
  Pointer to semaphore data
 
  @return
  Status of semaphore pool deinit
*/
static boolean
rfcommon_semaphore_pool_deinit
(
  rfcommon_semaphore_data_type *sem_data
)
{
  boolean ret_val = TRUE;

  /* build the items into the doubly-linked list */
  uint32 idx = 0;
  for ( idx = 0; idx < RFCOMMON_SEMAPHORE_ITEM_NUM; idx++ )
  {
    rfcommon_semaphore_destroy(&sem_data->pool.item_list[idx].sem_token);
    rfcommon_semaphore_remove_item(&sem_data->pool.item_list[idx]);
  }

  return ret_val;
} /* rfcommon_semaphore_pool_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Pop semaphore item from SemPool
 
  @details
  This function will be used pop any available spare semaphore item from
  the SemPool. If no spare semaphore is found, it is a unrecoverable situation.
  Thus an ERR_FATAL will be done.
 
  Semaphore value for the _found_ semaphore should always be zero. If the
  value is non-zero, then it is a unrecoverable situation.
 
  @return
  Pointer to the semaphore token which is poped to be used by client
*/
rfcommon_semaphore_token_type*
rfcommon_semaphore_pop_item
(
  void
)
{
  rfcommon_semaphore_data_type *sem_data; /* Semaphore Data */
  rfcommon_semaphore_pool_type *sem_pool; /* semaphore pool */
  rfcommon_semaphore_item_type *found = NULL; /* Variable to store the item */
  int sem_val = 0xDDDD0227; /* semaphore value: Initializing to some value */
  boolean sem_found = TRUE;

  /* Get Semaphore data */
  sem_data = rfcommon_get_semaphore_data();

  /* NULL pointer check */
  if( sem_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_semaphore_pop_item: NULL Sem Data" );
    sem_found = FALSE;
    return NULL;
  } /* if( sem_data == NULL ) */

  /* Get semaphore Pool */ 
  sem_pool = &(sem_data->pool);

  /* Enter Critical Section */
  rf_common_enter_critical_section( &(sem_pool->critical_sect) );

  /*--------------------------------------------------------------------------*/
  /* Get a spare item from sentinel_spare_items */
  /*--------------------------------------------------------------------------*/

  /* Check if the "Spare" list has atleast one item */
  if ( sem_pool->sentinel_spare_items.next != 
       &(sem_pool->sentinel_spare_items)
     )
  {
    /* There is free item in Spare list. Use that to push to "Used" list */
    found = sem_pool->sentinel_spare_items.next;

    /* Semaphore value for the _found_ semaphore should always be zero. 
    If the value is non-zero, then it is a unrecoverable situation */

    /* Get Sem Value */
    sem_getvalue( &(found->sem_token.semaphore), &sem_val );

    /* Make sure that Sem value is not greater than 0 */
    if ( sem_val > 0 )
    {
      /* Spare item not found */
      ERR_FATAL( "rfcommon_semaphore_pop_item: POP [%d] Sem @ 0x%x Value "
                 "%d > 0", found->item_id, &(found->sem_token), sem_val );
      sem_found = FALSE;
    } /* if ( sem_val > 0 ) */
    else
    {
      /* Remove the item from the list */
      rfcommon_semaphore_remove_item( found );

      /* Mark as part of USED LIST */
      found->sem_token.in_sem_pool_used_list = TRUE;
  
      /* Insert it at the head of the used list */
      rfcommon_semaphore_insert_item( &(sem_pool->sentinel_used_items), found );

      /* Decrease the number of spare items */
      sem_data->pool.num_of_spare_items--;
  
      RF_MSG_3( RF_MED, "rfcommon_semaphore_pop_item: POP [%d] Sem @ 0x%x "
                "{Items left %d}", found->item_id, &(found->sem_token),
                sem_data->pool.num_of_spare_items );

      sem_found = TRUE;

    } /* if ! ( sem_val > 0 ) */

  } /* if ( sem_pool->sentinel_spare_items.next != 
       &(sem_pool->sentinel_spare_items)
     ) */
  else
  {
    /* Spare item not found */
    ERR_FATAL( "rfcommon_semaphore_pop_item: no SPARE semaphore", 0, 0, 0 );
    sem_found = FALSE;
  } /* if ! ( sem_pool->sentinel_spare_items.next != 
       &(sem_pool->sentinel_spare_items)
     ) */

  /* Leave Critical Section */
  rf_common_leave_critical_section( &(sem_pool->critical_sect) );

  if ( sem_found == TRUE )
  {
    return &(found->sem_token);
  }
  else
  {
    return NULL;
  }

} /* rfcommon_semaphore_pop_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Pop semaphore item from SemPool
 
  @details
  This function will be used pop any available spare semaphore item from
  the SemPool. If no spare semaphore is found, it is a unrecoverable situation.
  Thus an ERR_FATAL will be done.
 
  Semaphore value for the _found_ semaphore should always be zero. If the
  value is non-zero, then it is a unrecoverable situation.
*/
boolean
rfcommon_semaphore_push_item
(
  rfcommon_semaphore_token_type *sem_token
)
{
  rfcommon_semaphore_data_type *sem_data; /* Semaphore Data */
  rfcommon_semaphore_pool_type *sem_pool; /* semaphore pool */
  rfcommon_semaphore_item_type *found = NULL; /* Variable to store the item */
  boolean ret_val = TRUE;

  /* Get Semaphore data */
  sem_data = rfcommon_get_semaphore_data();

  /* NULL pointer check */
  if( sem_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_semaphore_push_item: NULL Sem Data" );
    return FALSE;
  } /* if( sem_data == NULL ) */

  /* Get semaphore Pool */ 
  sem_pool = &(sem_data->pool);

  /* Enter Critical Section */
  rf_common_enter_critical_section( &(sem_pool->critical_sect) );

  /* Find the semaphore token in used list */
  found = rfcommon_semaphore_find_item( sem_data, sem_token );

  /* Check if the token is found or not */
  if( found == NULL )
  {
    /* Spare item not found */
    ERR_FATAL( "rfcommon_semaphore_push_item: PUSH Sem @ 0x%x not found", 
               &(found->sem_token), 0, 0 );

    ret_val = FALSE;
  } /* if( found == NULL ) */
  else
  {
    /* Remove the item from the list */
    rfcommon_semaphore_remove_item( found );

    /* Mark as part of USED LIST */
    found->sem_token.in_sem_pool_used_list = FALSE;
    
    /* Insert it at the head of the spare list */
    rfcommon_semaphore_insert_item( &(sem_pool->sentinel_spare_items), found );

    /* Increase the number of spare items */
    sem_data->pool.num_of_spare_items++;

    RF_MSG_3( RF_MED, "rfcommon_semaphore_push_item: PUSH [%d] Sem @ 0x%x "
              "{Items left %d}", found->item_id, &(found->sem_token), 
              sem_data->pool.num_of_spare_items );

    ret_val &= TRUE;
  } /* if ! ( found == NULL ) */

  /* Leave Critical Section */
  rf_common_leave_critical_section( &(sem_pool->critical_sect) );

  return ret_val;

} /* rfcommon_semaphore_push_item */

/*! @} */

/*============================================================================*/
/*!
  @name Semaphore Init

  @brief
  This section contains functions used for to Semaphore init.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Semaphore Module
 
  @details
  This function will initialize the semaphore module which include
  the sem_pool initializaiton
 
  @return
  Flag indicating the success and failure of initialization
*/
boolean
rfcommon_semaphore_init
(
  void
)
{
  boolean ret_val = TRUE;
  rfcommon_semaphore_data_type *sem_data; /* Semaphore Data */

  /* Get Semaphore data */
  sem_data = rfcommon_get_semaphore_data();

  /* NULL pointer check */
  if( sem_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_semaphore_init: NULL Sem Data" );
    return FALSE;
  } /* if( sem_data == NULL ) */

  /* Initialize the Semaphore POOL */
  ret_val &= rfcommon_semaphore_pool_init(sem_data);

  /* return */
  return ret_val;

} /* rfcommon_semaphore_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Semaphore Module
 
  @details
  This function will deinitialize the semaphore module which include
  the sem_pool deinitializaiton
 
  @return
  Flag indicating the success and failure of deinitialization
*/
boolean
rfcommon_semaphore_deinit
(
  void
)
{
  boolean ret_val = TRUE;
  rfcommon_semaphore_data_type *sem_data; /* Semaphore Data */

  /* Get Semaphore data */
  sem_data = rfcommon_get_semaphore_data();

  /* NULL pointer check */
  if( sem_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_semaphore_deinit: NULL Sem Data" );
    return FALSE;
  } /* if( sem_data == NULL ) */

  /* Deinitialize the Semaphore POOL */
  ret_val &= rfcommon_semaphore_pool_deinit(sem_data);

  /* return */
  return ret_val;

} /* rfcommon_semaphore_deinit */

/*! @} */

/*============================================================================*/
/*!
  @name Setup

  @brief
  This section contains functions used for to Semaphore setup.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Semaphore
 
  @details
  This funciton will initialize a given semaphore token. The token will be
  initialized in such a way that it cannot be shared across processes.
 
  @param sem_token
  Semaphore token to be initialized
 
  @return
  Flag indicating is the semaphore is successfully initialized or not.
*/
boolean
rfcommon_semaphore_create
(
  rfcommon_semaphore_token_type *sem_token
)
{

  boolean ret_val = TRUE; /* Retuen value */

  /* rf_pshared = 0 cannot be shared across processes - as per POSIX standards*/ 
  int rf_pshared = 0; 

  /* a value of 0 makes the semaphore (SM) unavailable - hence the very 
  first wait on the SM will work as expected - i.e. will block */
  unsigned int init_value = 0;  
    
  int sem_init_res; /* result of initializing semaphore */

  /* initialize a semaphore for each of the messages. Return value of 0 means 
  success, -1 is a failure */
  sem_init_res = sem_init( &(sem_token->semaphore), rf_pshared, init_value);
  sem_token->is_sem_waiting = FALSE;
  sem_token->post_before_wait = FALSE;
  sem_token->pbw_time = 0;
  sem_token->from_sem_pool = FALSE;
  sem_token->in_sem_pool_used_list = FALSE;

  /* ensure that sem_init call passed */
  if (sem_init_res != 0)
  {
    RF_MSG_1( RF_ERROR, "rfcommon_semaphore_create: Sem @ 0x%x Init Failed",
              sem_token );
    ret_val = FALSE;
  }
  else
  {
    RF_MSG_1( RF_HIGH, "rfcommon_semaphore_create: Sem @ 0x%x Init",
              sem_token );
  }


  return ret_val;

} /* rfcommon_semaphore_create */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-initialize Semaphore
 
  @details
  This funciton will de-initialize a given semaphore token. 
 
  @param sem_token
  Semaphore token to be de-initialized
 
  @return
  Flag indicating is the semaphore is successfully de-initialized or not.
*/
boolean
rfcommon_semaphore_destroy
(
  rfcommon_semaphore_token_type *sem_token
)
{
  int sem_ret;

  /* destroy the associated semaphore */
  sem_ret = sem_destroy( &(sem_token->semaphore) );
  sem_token->is_sem_waiting = FALSE;
  sem_token->post_before_wait = FALSE;
  sem_token->pbw_time = 0;

  if( sem_ret != 0 )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_semaphore_destroy: Sem @ 0x%x destroy failed",
              sem_token );
  }
  else
  {
    RF_MSG_1( RF_HIGH, "rfcommon_semaphore_destroy: Sem @ 0x%x destroyed",
              sem_token );
  }

  return TRUE;

} /* rfcommon_semaphore_destroy */

/*! @} */

/*============================================================================*/
/*!
  @name Blocking Call

  @brief
  This section contains functions used for to handle blocking call.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Wait for Semaphore
 
  @details
  This function will wait on a semaphore until it is posted. This is used
  when a block call is to be made for some response.

  @param sem_token
  Semaphore token on which WAIT is to be done
 */
void
rfcommon_semaphore_wait
(
  rfcommon_semaphore_token_type *sem_token
)
{
  int sem_val = 0xAD090226; /* semaphore value: Initializing to some value */
  rfcommon_semaphore_data_type *sem_data; /* Semaphore Data */
  int sem_ret; /* Semaphore return value */

  if ( sem_token == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_semaphore_post: NULL Sem Token" );
    return;
  } /* if ( sem_token == NULL ) */

  /* Get Semaphore data */
  sem_data = rfcommon_get_semaphore_data();

  /* NULL pointer check */
  if( sem_data == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_semaphore_wait: Sem @ 0x%x NULL Sem Data",
              sem_token );
    return;
  } /* if( sem_data == NULL ) */

  /* Get current Semaphore value */
  sem_ret = sem_getvalue( &(sem_token->semaphore), &sem_val );

  if( sem_ret != 0 )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_semaphore_wait : Sem @ 0x%x Get value failed",
              sem_token );
  } /* if( sem_ret != 0 ) */
  
  if( sem_data->dbg_msg_en == TRUE )
  {
    RF_MSG_2( RF_LOW, "rfcommon_semaphore_wait: Sem @ %d BEFORE wait %d", 
              sem_token, sem_val );
  } /* if( sem_data->dbg_msg_en == TRUE ) */
    
  /* Check for validity of semaphore value. Value of each semaphore is expected 
  to be 0. If not, it means that a "post" was done on this semaphore - hence the 
  value is greater than 0. This is not expected, as each message has an 
  associated semaphore, and the sem_wait is expected to be a blocking call.
  A value > 0 means that the semaphore is available, and results in the
  sem_wait() call NOT being blocked. */
  if (sem_val > 0)
  {
    /* Since we detected corrupted Semaphore, set the flag to re-initialize
    this semaphore */
    if( sem_token->post_before_wait == FALSE )
    {
      /* Update the structure only the first time when POST-BEFORE-WAIT is
      encountered. These values will be retained until semaphores are
      re-initialized.*/
      sem_token->pbw_time = timetick_get();
      sem_token->post_before_wait = TRUE;
    } /* if( sem_token->post_before_wait == FALSE ) */

    RF_MSG_2( RF_HIGH, "rfcommon_semaphore_wait: Sem @ 0x%x value %d"
                        "*****Possible POST-BEFORE-WAIT***** !", sem_token,
                        sem_val );
  } /* if (sem_val > 0) */

  RF_MSG_2( RF_LOW, "rfcommon_semaphore_wait: Waiting for Sem @ 0x%x with "
                    "value %d", sem_token, sem_val );

  /* Log the Semaphore wait */
  rfcommon_semaphore_log_wait( sem_data, sem_token );

  /* Wait for semaphore to be posted */
  sem_token->is_sem_waiting = TRUE;
  sem_ret = sem_wait( &(sem_token->semaphore) );

  if( sem_ret != 0 )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_semaphore_wait : Sem @ 0x%x Wait failed",
              sem_token );
  } /* if( sem_ret != 0 ) */

  if(sem_data->dbg_msg_en == TRUE)
  {
    sem_getvalue( &(sem_token->semaphore), &sem_val );

    if( sem_ret != 0 )
    {
      RF_MSG_1( RF_ERROR, "rfcommon_semaphore_wait : Sem @ 0x%x Get value "
                          "failed", sem_token );
    } /* if( sem_ret != 0 ) */

    RF_MSG_2( RF_LOW, "rfcommon_semaphore_wait: Sem @ 0x%x AFTER wait %d", 
              sem_token, sem_val );
  } /* if(sem_data->dbg_msg_en == TRUE) */

  return;

} /* rfcommon_semaphore_wait */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Post the sempahore to end sempphore wait
 
  @details
  This function will set the semaphore; so that the blocked call will be 
  unblocked.
  
  @param sem_token
  Semaphore token on which POST is to be done
 
  @return
  TRUE if all the response is handled successfully; FALSE otherwise
*/
boolean
rfcommon_semaphore_post
(
  rfcommon_semaphore_token_type *sem_token
)
{
  boolean ret_val = TRUE;
  int sem_val; /* semaphore value */
  rfcommon_semaphore_data_type *sem_data; /* Semaphore Data */
  int sem_ret; /* Semaphore return value */

  if ( sem_token == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_semaphore_post: NULL Sem Token" );
    return FALSE;
  } /* if ( sem_token == NULL ) */

  /* Get Semaphore data */
  sem_data = rfcommon_get_semaphore_data();

  /* NULL pointer check */
  if( sem_data == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_semaphore_wait: Sem @ 0x%x NULL Sem Data",
              sem_token );
    return FALSE;
  }

  /* Addition Check needed is the semaphore is from SEMPOOL. If the
  Semaphore is not in Used List, then we cannot do POST. */
  if ( sem_token->from_sem_pool == TRUE )
  {
    if ( sem_token->in_sem_pool_used_list == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfcommon_semaphore_wait: Sem @ 0x%x not in SemPool "
                          "Used List", sem_token );
      return FALSE;
    } /* if ( sem_token->in_sem_pool_used_list == FALSE ) */
  } /* if ( sem_token->from_sem_pool == TRUE ) */

  if(sem_data->dbg_msg_en == TRUE)
  {
    /* Get current Semaphore */
    sem_ret = sem_getvalue(&(sem_token->semaphore), &sem_val);

    if( sem_ret != 0 )
    {
      RF_MSG_1( RF_ERROR, "rfcommon_semaphore_post : Sem @ 0x%x Get Val failed",
                sem_token );
    }

    RF_MSG_2( RF_LOW, "rfcommon_semaphore_post: Sem @ 0x%x BEFORE wait %d", 
              sem_token, sem_val );
  } /* if(rfcommon_data.rfcommon_msgr_enable_dbg_msgs == TRUE) */

  /* Log the Posted Semaphores */
  rfcommon_semaphore_log_post( sem_data, sem_token );

  /* Post Semaphore */
  sem_ret = sem_post( &(sem_token->semaphore) );
  sem_token->is_sem_waiting = FALSE;

  if( sem_ret != 0 )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_semaphore_post : Sem @ 0x%x Post failed",
              sem_token );
  }
  else
  {
    RF_MSG_1( RF_LOW, "rfcommon_semaphore_post: Posted for Sem @ 0x%x",
              sem_token );
  }

  if(sem_data->dbg_msg_en == TRUE)
  {
    /* Get current Semaphore */
    sem_ret = sem_getvalue(&(sem_token->semaphore), &sem_val);

    if( sem_ret != 0 )
    {
      RF_MSG_1( RF_ERROR, "rfcommon_semaphore_post : Sem @ 0x%x Get Val failed",
                sem_token );
    }

    RF_MSG_2( RF_LOW, "rfcommon_semaphore_post: Sem @ 0x%x AFTER post %d", 
              sem_token, sem_val );

  } /* if(sem_data->dbg_msg_en == TRUE) */
  
  return ret_val;

} /* rfcommon_semaphore_post */

/*! @} */

/*! @} */


