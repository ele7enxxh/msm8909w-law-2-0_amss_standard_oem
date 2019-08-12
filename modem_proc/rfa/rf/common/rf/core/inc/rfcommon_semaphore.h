#ifndef RFCOMMON_SEMAPHORE_H
#define RFCOMMON_SEMAPHORE_H
/*! 
  @file
  rfcommon_semaphore.h
 
  @brief
  This file includes functions pertaining to semaphore for RF COMMON Operation
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_semaphore.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/19/12   aki     Add rfcommon_semaphore_deinit function
07/23/12   Saul    RF COMMON. MDSP/MSGR framework.
05/09/12   aro     Added fields in sem token to indicate if the token is
                   part of SemPool
05/08/12   aro     Code to pop and push the semaphor items from and to the
                   semaphore pool
05/08/12   aro     Initial support for semaphore POOL
05/08/12   aro     Renamed function to sem_create and senm_destroy
05/02/12   aro     Support to detect POST-BEFORE-WAIT
02/15/12   aro     Added support for Semaphore Wait and Post logging
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
#include "semaphore.h"
#include "timetick.h"
#include "rfcommon_locks.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Number of Status Items */
#define RFCOMMON_SEMAPHORE_ITEM_NUM 10

/*----------------------------------------------------------------------------*/
/*! Maximum number of tokens */
#define RFCOMMON_SEMA_TOKEN_LIST_MAX 50

/*----------------------------------------------------------------------------*/
/*! Structure to store RF COMMON Token */
typedef struct
{

  sem_t semaphore;
  /*!< Semaphore */

  boolean is_sem_waiting;
  /*!< Flag indicating if the semaphore is waiting  */

  boolean post_before_wait;
  /*!< Flag indicating that Semaphores has been posted before WAIT. This may 
  or may not be an error case. Thus the semaphore client may or may not choose 
  to re-initialize semphore. This will be set to TRUE when semaphore value is 
  detected to be greater than 0, when sem_wait() is called. */

  boolean from_sem_pool;
  /*!< Flag indicating if the semphore used is from the SEMPOOL */

  boolean in_sem_pool_used_list;
  /*!< Flag indicating, if the semaphore is in the USED LIST of the SEMPOOL. 
  This is applicable, only if from_sem_pool = TRUE */

  timetick_type pbw_time;
  /*!< Time when the first post-before-wait was encountered */

} rfcommon_semaphore_token_type; 

/*----------------------------------------------------------------------------*/
/*! Semaphore token ID type */
typedef uint8 rfcommon_semaphore_item_id_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold Sempaphore Token log

  @details
  This structure holds logs related to semaphore Toke
*/
typedef struct
{

  uint32 sem_token;
  /*!< Address of Semaphore token that is to be logged */

  timetick_type token_time;
  /*!< Time when the semaphore was logged in timetick units */

} rfcommon_semaphore_token_log_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  structure definition for Semaphore Token doubly linked list
 
  @details
  Each Node will hold Semaphore token which can be used by clients. 
  This has two pointer which are pointer to next and the previous node.
*/
typedef struct rfcommon_semaphore_item
{
  rfcommon_semaphore_item_id_type item_id;
  /*!< ID for each item */

  rfcommon_semaphore_token_type sem_token;
  /*!< Semaphore token for this node */

  struct rfcommon_semaphore_item *prev;
  /*!< Pointer to Previous Node in the linked list */

  struct rfcommon_semaphore_item *next;
  /*!< Pointer to Next Node in the linked list */

} rfcommon_semaphore_item_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold semaphore Pool data */
typedef struct
{

  rfcommon_semaphore_item_type item_list[RFCOMMON_SEMAPHORE_ITEM_NUM];
  /*!< Semaphore token items where each entry will correspond to one 
  semaphore */

  rfcommon_semaphore_item_type sentinel_spare_items;
  /*!< 
  @brief 
  Sentinel for Spare items 
   
  @details 
  This list contains unallocated items. Based on client usage, 
  item status from this list will be removed and put in sentinel_used 
  list. 
  */

  rfcommon_semaphore_item_type sentinel_used_items;
  /*!< 
  @brief 
  Sentinel for Used items 
   
  @details 
  This list contains items which are used atleast once. Based on client usage, 
  item from sentinel_spare list will be removed and 
  put in this list. The items in sentinel_used are 
  maintained in time order.  The first item in the list is the most recent 
  (current) item being used
  */

  uint8 num_of_spare_items;
  /*!< Variable to indicate the total number of free items */

  rf_lock_data_type critical_sect;
  /*!< critical section protection to pop and push the semaphore items */
  
} rfcommon_semaphore_pool_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold all semaphoer related data */
typedef struct
{
  rfcommon_semaphore_pool_type pool;
  /*!< Structure to hold semaphore Pool data */

  boolean dbg_msg_en;
  /*!< Flag indicating F3 message enable/disable for CDMA semaphore module */

  uint16 wait_idx;
  /*!< Value indicating which index of the array of Semaphore Wait Log was 
    populated last */

  uint16 post_idx;
  /*!< Value indicating which index of the array of Semaphore Post Log was 
    populated last */

  rfcommon_semaphore_token_log_type sem_wait_log[RFCOMMON_SEMA_TOKEN_LIST_MAX];
  /*!< Structure to hold Sempaphore wait log. This structure will be used as 
  a circular buffer. Latest entry into the array will be indicated by wait_idx.
  This is used for debugging purpose only. */

  rfcommon_semaphore_token_log_type sem_post_log[RFCOMMON_SEMA_TOKEN_LIST_MAX];
  /*!< Structure to hold Sempaphore post log. This structure will be used as 
  a circular buffer. Latest entry into the array will be indicated by post_idx.
  This is used for debugging purpose only. */

} rfcommon_semaphore_data_type;

/*----------------------------------------------------------------------------*/
rfcommon_semaphore_token_type*
rfcommon_semaphore_pop_item
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_semaphore_push_item
(
  rfcommon_semaphore_token_type *sem_token
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_semaphore_init
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_semaphore_deinit
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_semaphore_create
(
  rfcommon_semaphore_token_type *sem_token
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_semaphore_destroy
(
  rfcommon_semaphore_token_type *sem_token
);

/*----------------------------------------------------------------------------*/
void
rfcommon_semaphore_wait
(
  rfcommon_semaphore_token_type *sem_token
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_semaphore_post
(
  rfcommon_semaphore_token_type *sem_token
);

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_SEMAPHORE_H */

